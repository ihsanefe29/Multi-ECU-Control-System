#include "rawdataparser.h"
#include <QDebug>
#include <cstring>

RawDataParser::RawDataParser(QObject *parent)
    : QObject(parent)
{}

void RawDataParser::setParameters(const QVector<Parameter> &parameters)
{
    m_parameters = parameters;
}

// ── Bit extraction ─────────────────────────────────────────────────────────

quint64 RawDataParser::extractBits(
    const QByteArray &data,
    int bitOffset,
    int bitWidth)
{
    quint64 result = 0;

    for (int i = 0; i < bitWidth; ++i) {
        int absoluteBit = bitOffset + i;
        int byteIndex   = absoluteBit / 8;
        int bitIndex    = absoluteBit % 8;

        if (byteIndex >= data.size())
            break;

        quint8 byte = static_cast<quint8>(data[byteIndex]);
        quint64 bit = (byte >> bitIndex) & 1;
        result |= (bit << i);
    }

    return result;
}

// ── Type parse ────────────────────────────────────────────────────────────

QVariant RawDataParser::parseValue(
    const QByteArray &data,
    const QString    &dataType,
    int bitOffset,
    int bitWidth)
{
    quint64 raw = extractBits(data, bitOffset, bitWidth);

    if (dataType == "uint8"  || dataType == "uint16" ||
        dataType == "uint32" || dataType == "uint64")
    {
        return static_cast<quint64>(raw);
    }

    if (dataType == "int8"  || dataType == "int16" ||
        dataType == "int32" || dataType == "int64")
    {
        // İşaret biti kontrolü
        quint64 signBit = (1ULL << (bitWidth - 1));
        if (raw & signBit) {
            qint64 signed_val = static_cast<qint64>(raw) -
                                static_cast<qint64>(1ULL << bitWidth);
            return signed_val;
        }
        return static_cast<qint64>(raw);
    }

    if (dataType == "float" && bitWidth == 32) {
        float f;
        quint32 raw32 = static_cast<quint32>(raw);
        std::memcpy(&f, &raw32, sizeof(float));
        return static_cast<double>(f);
    }

    qDebug() << "RawDataParser: Unsupported data type:" << dataType;
    return QVariant();
}

// ── Conversion formula ─────────────────────────────────────────────────────

QVariant RawDataParser::applyConversion(
    const QVariant &rawValue,
    const QString  &formula)
{
    if (!rawValue.isValid())
        return QVariant();

    QString expression = formula.trimmed();

    if (expression.isEmpty() || expression == "-")
        return rawValue;

    if (expression.compare("Enum", Qt::CaseInsensitive) == 0)
        return rawValue;

    if (expression.compare("Hex", Qt::CaseInsensitive) == 0)
        return rawValue;

    bool ok = false;
    double x = rawValue.toDouble(&ok);
    if (!ok)
        return rawValue;

    // Birim bilgisini formülden kaldır: "x * 0.01 (A)" → "x * 0.01"
    int unitStart = expression.indexOf('(');
    if (unitStart != -1)
        expression = expression.left(unitStart).trimmed();

    if (expression == "x")
        return x;

    auto extractOperand = [&](const QString &prefix) -> double {
        return expression.mid(prefix.length()).trimmed().toDouble(&ok);
    };

    if (expression.startsWith("x *")) { double v = extractOperand("x *"); if (ok) return x * v; }
    if (expression.startsWith("x /")) { double v = extractOperand("x /"); if (ok && v != 0) return x / v; }
    if (expression.startsWith("x +")) { double v = extractOperand("x +"); if (ok) return x + v; }
    if (expression.startsWith("x -")) { double v = extractOperand("x -"); if (ok) return x - v; }

    qDebug() << "RawDataParser: Unsupported conversion formula:" << formula;
    return rawValue;
}

// ── Parse ──────────────────────────────────────────────────────────────────

void RawDataParser::parse(
    const QByteArray         &rawData,
    const QVector<Parameter> &parameters)
{
    m_values.clear();

    qDebug() << "RawDataParser: data size:" << rawData.size()
             << "parameters:" << parameters.size();

    for (const Parameter &p : parameters) {
        int requiredBits  = p.bitOffset + p.dataWidth;
        int requiredBytes = (requiredBits + 7) / 8;

        if (p.ramAddress < 0 ||
            requiredBytes <= 0 ||
            p.ramAddress + requiredBytes > rawData.size())
        {
            qDebug() << "RawDataParser: out of range:" << p.parameterName;
            continue;
        }

        QByteArray slice = rawData.mid(p.ramAddress, requiredBytes);

        QVariant raw       = parseValue(slice, p.dataType, p.bitOffset, p.dataWidth);
        QVariant converted = applyConversion(raw, p.conversionFormula);

        m_values.insert(p.parameterName, converted);

        qDebug() << "  " << p.parameterName
                 << "raw=" << raw << "→" << converted;
    }

    emit valuesChanged();
}

// ── Getters ───────────────────────────────────────────────────────────────

QVariantMap RawDataParser::parameterInfo(const QString &parameterName) const
{
    for (const Parameter &p : m_parameters) {
        if (p.parameterName == parameterName) {
            return {
                { "address",   p.ramAddress },
                { "width",     p.dataWidth },
                { "bitOffset", p.bitOffset },
                { "type",      p.dataType },
                { "formula",   p.conversionFormula }
            };
        }
    }
    return {};
}

QVariantMap RawDataParser::values() const
{
    return m_values;
}
