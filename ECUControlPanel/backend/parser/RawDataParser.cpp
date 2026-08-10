#include "RawDataParser.h"

#include <QDebug>
#include <cstring>


RawDataParser::RawDataParser(QObject *parent)
    : QObject(parent)
{
}


// ============================================================
// PARAMETRE LİSTESİNİ SAKLA
// ============================================================

void RawDataParser::setParameters(
    const QVector<Parameter> &parameters)
{
    m_parameters = parameters;
}


// ============================================================
// RAW BYTE → DEĞER
// ============================================================

QVariant RawDataParser::parseValue(
    const QByteArray &data,
    const QString &dataType)
{
    if (data.isEmpty())
    {
        return QVariant();
    }


    // --------------------------------------------------------
    // UNSIGNED
    // --------------------------------------------------------

    if (dataType == "Unsigned")
    {
        quint64 value = 0;

        for (unsigned char byte : data)
        {
            value = (value << 8) | byte;
        }

        return QVariant::fromValue(value);
    }


    // --------------------------------------------------------
    // SIGNED
    // --------------------------------------------------------

    if (dataType == "Signed")
    {
        qint64 value = 0;

        for (unsigned char byte : data)
        {
            value = (value << 8) | byte;
        }

        int bitCount = data.size() * 8;

        if (value & (quint64(1) << (bitCount - 1)))
        {
            value -= (quint64(1) << bitCount);
        }

        return QVariant::fromValue(value);
    }


    // --------------------------------------------------------
    // FLOAT
    // --------------------------------------------------------

    if (dataType == "Float")
    {
        if (data.size() != 4)
        {
            return QVariant();
        }

        quint32 rawValue = 0;

        for (unsigned char byte : data)
        {
            rawValue = (rawValue << 8) | byte;
        }

        float value;

        memcpy(
            &value,
            &rawValue,
            sizeof(float)
            );

        return QVariant::fromValue(value);
    }


    return QVariant();
}


// ============================================================
// CONVERSION FORMÜLÜ UYGULA
// ============================================================

QVariant RawDataParser::applyConversion(
    const QVariant &rawValue,
    const QString &formula)
{
    if (!rawValue.isValid())
    {
        return QVariant();
    }


    // Formül yoksa raw değeri kullan

    if (formula.isEmpty() ||
        formula == "-")
    {
        return rawValue;
    }


    // Hex gibi matematiksel olmayan dönüşümler

    if (formula.compare(
            "Hex",
            Qt::CaseInsensitive) == 0)
    {
        return rawValue;
    }


    bool ok = false;

    double x =
        rawValue.toDouble(&ok);

    if (!ok)
    {
        return rawValue;
    }


    QString expression =
        formula.trimmed();


    // --------------------------------------------------------
    // BİRİM BİLGİSİNİ KALDIR
    //
    // x * 0.001 (V)
    //       ↓
    // x * 0.001
    // --------------------------------------------------------

    int unitStart =
        expression.indexOf('(');

    if (unitStart != -1)
    {
        expression =
            expression.left(unitStart).trimmed();
    }


    // --------------------------------------------------------
    // x * katsayı
    //
    // Örnek:
    // x * 0.001
    // x * 0.01
    // x * 0.1
    // --------------------------------------------------------

    if (expression.startsWith("x *"))
    {
        QString factorText =
            expression.mid(3).trimmed();

        double factor =
            factorText.toDouble(&ok);

        if (ok)
        {
            return x * factor;
        }
    }


    // --------------------------------------------------------
    // x / sayı
    // --------------------------------------------------------

    if (expression.startsWith("x /"))
    {
        QString divisorText =
            expression.mid(3).trimmed();

        double divisor =
            divisorText.toDouble(&ok);

        if (ok && divisor != 0)
        {
            return x / divisor;
        }
    }


    // --------------------------------------------------------
    // x + sayı
    // --------------------------------------------------------

    if (expression.startsWith("x +"))
    {
        QString offsetText =
            expression.mid(3).trimmed();

        double offset =
            offsetText.toDouble(&ok);

        if (ok)
        {
            return x + offset;
        }
    }


    // --------------------------------------------------------
    // x - sayı
    // --------------------------------------------------------

    if (expression.startsWith("x -"))
    {
        QString offsetText =
            expression.mid(3).trimmed();

        double offset =
            offsetText.toDouble(&ok);

        if (ok)
        {
            return x - offset;
        }
    }


    qDebug()
        << "RawDataParser: Unsupported conversion formula:"
        << formula;

    return rawValue;
}


// ============================================================
// RAW DATA PARSE
// ============================================================

void RawDataParser::parse(
    const QByteArray &rawData,
    const QVector<Parameter> &parameters)
{
    m_values.clear();


    qDebug()
        << "RawDataParser: Raw data size:"
        << rawData.size();


    for (const Parameter &parameter : parameters)
    {
        int startAddress =
            parameter.ramAddress;

        int dataWidth =
            parameter.dataWidth;


        int endAddress =
            startAddress +
            dataWidth -
            1;


        if (startAddress < 0 ||
            endAddress >= rawData.size())
        {
            continue;
        }


        QByteArray parameterData =
            rawData.mid(
                startAddress,
                dataWidth
                );


        // ----------------------------------------------------
        // RAW BYTE → RAW VALUE
        // ----------------------------------------------------

        QVariant value =
            parseValue(
                parameterData,
                parameter.dataType
                );


        // ----------------------------------------------------
        // RAW VALUE → CONVERTED VALUE
        // ----------------------------------------------------

        QVariant convertedValue =
            applyConversion(
                value,
                parameter.conversionFormula
                );


        // ----------------------------------------------------
        // SONUCU SAKLA
        // ----------------------------------------------------

        m_values.insert(
            parameter.parameterName,
            convertedValue
            );


        qDebug()
            << "Parameter:"
            << parameter.parameterName
            << "| Raw Value:"
            << value
            << "| Converted Value:"
            << convertedValue;
    }


    emit valuesChanged();
}


// ============================================================
// PARAMETRE BİLGİSİ
// ============================================================

QVariantMap RawDataParser::parameterInfo(
    const QString &parameterName) const
{
    QVariantMap info;


    for (const Parameter &parameter :
         m_parameters)
    {
        if (parameter.parameterName ==
            parameterName)
        {
            info["address"] =
                parameter.ramAddress;

            info["width"] =
                parameter.dataWidth;

            info["type"] =
                parameter.dataType;

            info["formula"] =
                parameter.conversionFormula;

            break;
        }
    }


    return info;
}


// ============================================================
// PARSED VALUES
// ============================================================

QVariantMap RawDataParser::values() const
{
    return m_values;
}