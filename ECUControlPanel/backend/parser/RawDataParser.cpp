#include "RawDataParser.h"

#include <QDebug>
#include <QtGlobal>


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
// BIT ALANINI ÇIKAR
//
// data:
//      RAM_ADDR'dan başlayan ham byte alanı
//
// dataWidth:
//      Kullanılacak bit sayısı
//
// bitOffset:
//      Alanın içerisindeki başlangıç biti
//
// Örnek:
//
// 32 bitlik alan:
//
// 31                 16 15                  0
// ┌────────────────────┬────────────────────┐
// │       Major        │       Minor        │
// └────────────────────┴────────────────────┘
//
// Minor:
// bitOffset = 0
// dataWidth = 16
//
// Major:
// bitOffset = 16
// dataWidth = 16
// ============================================================

quint64 RawDataParser::extractBits(
    const QByteArray &data,
    int bitOffset,
    int bitWidth)
{
    if (data.isEmpty())
    {
        return 0;
    }

    if (bitOffset < 0 ||
        bitWidth <= 0 ||
        bitWidth > 64)
    {
        return 0;
    }


    quint64 value = 0;


    // --------------------------------------------------------
    // Little Endian
    //
    // Örneğin:
    //
    // 2C 01
    //
    // = 0x012C
    // = 300
    // --------------------------------------------------------

    for (int i = 0; i < data.size(); ++i)
    {
        const quint8 byte =
            static_cast<quint8>(
                data.at(i)
                );

        value |=
            (quint64(byte) << (i * 8));
    }


    // --------------------------------------------------------
    // İstenen bit alanını sağa kaydır
    // --------------------------------------------------------

    value >>= bitOffset;


    // --------------------------------------------------------
    // Mask oluştur
    //
    // 16 bit:
    //
    // 0000FFFF
    //
    // 8 bit:
    //
    // 000000FF
    // --------------------------------------------------------

    if (bitWidth < 64)
    {
        const quint64 mask =
            (quint64(1) << bitWidth) - 1;

        value &= mask;
    }


    return value;
}


// ============================================================
// RAW BYTE → DEĞER
// ============================================================

QVariant RawDataParser::parseValue(
    const QByteArray &data,
    const QString &dataType,
    int bitOffset,
    int bitWidth)
{
    if (data.isEmpty())
    {
        return QVariant();
    }


    // --------------------------------------------------------
    // FLOAT
    // --------------------------------------------------------

    if (dataType.compare(
            "float",
            Qt::CaseInsensitive) == 0)
    {
        if (bitOffset != 0 ||
            bitWidth != 32 ||
            data.size() < 4)
        {
            return QVariant();
        }


        quint32 rawValue = 0;


        for (int i = 0; i < 4; ++i)
        {
            rawValue |=
                (quint32(
                     static_cast<quint8>(
                         data.at(i)
                         )
                     ) << (i * 8));
        }


        float value = 0.0f;

        memcpy(
            &value,
            &rawValue,
            sizeof(float)
            );


        return QVariant::fromValue(value);
    }


    // --------------------------------------------------------
    // RAW BIT VALUE
    // --------------------------------------------------------

    quint64 rawValue =
        extractBits(
            data,
            bitOffset,
            bitWidth
            );


    // --------------------------------------------------------
    // UNSIGNED INTEGER
    // --------------------------------------------------------

    if (dataType.startsWith(
            "uint",
            Qt::CaseInsensitive))
    {
        return QVariant::fromValue(
            rawValue
            );
    }


    // --------------------------------------------------------
    // SIGNED INTEGER
    // --------------------------------------------------------

    if (dataType.startsWith(
            "int",
            Qt::CaseInsensitive))
    {
        qint64 signedValue =
            static_cast<qint64>(
                rawValue
                );


        // ----------------------------------------------------
        // Sign extension
        //
        // Örneğin 8 bit:
        //
        // 11111111 = -1
        // ----------------------------------------------------

        if (bitWidth < 64 &&
            (rawValue &
             (quint64(1) << (bitWidth - 1))))
        {
            const quint64 signMask =
                ~((quint64(1) << bitWidth) - 1);

            signedValue =
                static_cast<qint64>(
                    rawValue | signMask
                    );
        }


        return QVariant::fromValue(
            signedValue
            );
    }


    // --------------------------------------------------------
    // ENUM
    //
    // Şimdilik Enum'un raw sayısal değerini döndürüyoruz.
    //
    // Örneğin:
    //
    // 0 → Closed
    // 1 → Partial
    // 2 → Open
    //
    // Mapping bilgisi daha sonra ayrıca ele alınabilir.
    // --------------------------------------------------------

    if (dataType.compare(
            "Enum",
            Qt::CaseInsensitive) == 0)
    {
        return QVariant::fromValue(
            rawValue
            );
    }


    qDebug()
        << "RawDataParser: Unsupported data type:"
        << dataType;


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


    QString expression =
        formula.trimmed();


    if (expression.isEmpty() ||
        expression == "-")
    {
        return rawValue;
    }


    // --------------------------------------------------------
    // ENUM
    // --------------------------------------------------------

    if (expression.compare(
            "Enum",
            Qt::CaseInsensitive) == 0)
    {
        return rawValue;
    }


    // --------------------------------------------------------
    // HEX
    // --------------------------------------------------------

    if (expression.compare(
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


    // --------------------------------------------------------
    // Birim bilgisini formülden kaldır
    //
    // x * 0.01 (A)
    //      ↓
    // x * 0.01
    // --------------------------------------------------------

    int unitStart =
        expression.indexOf('(');


    if (unitStart != -1)
    {
        expression =
            expression.left(
                          unitStart
                          ).trimmed();
    }


    // --------------------------------------------------------
    // x
    // --------------------------------------------------------

    if (expression == "x")
    {
        return x;
    }


    // --------------------------------------------------------
    // x * sayı
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


    for (const Parameter &parameter :
         parameters)
    {
        const int startAddress =
            parameter.ramAddress;


        const int bitOffset =
            parameter.bitOffset;


        const int bitWidth =
            parameter.dataWidth;


        // ----------------------------------------------------
        // Kaç byte gerektiğini hesapla
        //
        // Örnek:
        //
        // bitOffset = 0
        // bitWidth  = 16
        //
        // → 2 byte
        //
        // bitOffset = 16
        // bitWidth  = 16
        //
        // → 4 byte
        // ----------------------------------------------------

        const int requiredBits =
            bitOffset +
            bitWidth;


        const int requiredBytes =
            (requiredBits + 7) / 8;


        if (startAddress < 0 ||
            requiredBytes <= 0 ||
            startAddress +
                    requiredBytes >
                rawData.size())
        {
            qDebug()
            << "RawDataParser: Parameter out of range:"
            << parameter.parameterName;

            continue;
        }


        // ----------------------------------------------------
        // İlgili byte alanını al
        // ----------------------------------------------------

        QByteArray parameterData =
            rawData.mid(
                startAddress,
                requiredBytes
                );


        // ----------------------------------------------------
        // RAW BIT → RAW VALUE
        // ----------------------------------------------------

        QVariant value =
            parseValue(
                parameterData,
                parameter.dataType,
                bitOffset,
                bitWidth
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
            << convertedValue
            << "| Address:"
            << parameter.ramAddress
            << "| Width:"
            << parameter.dataWidth
            << "| Bit Offset:"
            << parameter.bitOffset;
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

            info["bitOffset"] =
                parameter.bitOffset;

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