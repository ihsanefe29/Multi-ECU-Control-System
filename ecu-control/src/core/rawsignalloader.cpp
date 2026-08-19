#include "rawsignalloader.h"
#include "xlsxdocument.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <cstring>
#include <cmath>

RawSignalLoader::RawSignalLoader(QObject *parent)
    : QObject(parent)
{
}

// ================================================================
// STAGE 1 — PARAMETER DEFINITIONS (CSV / EXCEL)
// ================================================================

bool RawSignalLoader::loadParameterFile(const QUrl &fileUrl)
{
    const QString localPath = fileUrl.isLocalFile()
                                  ? fileUrl.toLocalFile()
                                  : fileUrl.toString();

    bool success = false;
    if (localPath.endsWith(".xlsx", Qt::CaseInsensitive)) {
        success = readParameterExcel(localPath);
    } else {
        success = readParameterCsv(localPath);
    }

    if (!success)
        return false;

    if (!m_rawBytes.isEmpty())
        parseRawData();

    return true;
}

bool RawSignalLoader::readParameterCsv(const QString &localPath)
{
    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setLastError(tr("Parametre dosyası açılamadı: %1").arg(localPath));
        return false;
    }

    QTextStream in(&file);
    m_parameters.clear();

    bool headerSkipped = false;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }

        // CSV: Address,Offset,Width,Type,Name,Formula
        const QStringList parts = line.split(',');
        if (parts.size() < 5)
            continue;

        bool addrOk = false, offOk = false, widthOk = false;
        Parameter p;
        p.ramAddress = parts.at(0).trimmed().toInt(&addrOk);
        p.dataOffset = parts.at(1).trimmed().toInt(&offOk);
        p.dataWidth  = parts.at(2).trimmed().toInt(&widthOk);
        p.dataType   = parts.at(3).trimmed();
        p.parameterName = parts.at(4).trimmed();
        p.conversionFormula = parts.size() > 5 ? parts.at(5).trimmed() : QString();

        if (!addrOk || !widthOk)
            continue;

        m_parameters.append(p);
    }

    if (m_parameters.isEmpty()) {
        setLastError(tr("Parametre dosyasında geçerli satır bulunamadı."));
        return false;
    }

    qDebug() << "RawSignalLoader: loaded" << m_parameters.size() << "parameter definitions from CSV";
    setLastError(QString());
    emit dataChanged();
    return true;
}

bool RawSignalLoader::readParameterExcel(const QString &localPath)
{
    QXlsx::Document document(localPath);
    if (!document.load()) {
        setLastError(tr("Excel dosyası açılamadı veya yüklenemedi: %1").arg(localPath));
        return false;
    }

    m_parameters.clear();

    // Excel structure: Row 1 = header, data starts at Row 2
    for (int row = 2; ; ++row) {
        QVariant addressVal = document.read(row, 1);
        if (!addressVal.isValid() || addressVal.toString().trimmed().isEmpty())
            break;

        Parameter p;
        p.ramAddress = addressVal.toInt();
        p.dataWidth = document.read(row, 2).toInt();
        p.dataOffset = document.read(row, 3).toInt(); // Bit Offset
        p.dataType = document.read(row, 4).toString().trimmed();
        p.parameterName = document.read(row, 5).toString().trimmed();
        p.conversionFormula = document.read(row, 6).toString().trimmed();

        m_parameters.append(p);
    }

    if (m_parameters.isEmpty()) {
        setLastError(tr("Excel dosyasında geçerli parametre tanımı bulunamadı."));
        return false;
    }

    qDebug() << "RawSignalLoader: loaded" << m_parameters.size() << "parameter definitions from Excel";
    setLastError(QString());
    emit dataChanged();
    return true;
}

// ================================================================
// STAGE 2 — RAW HEX CLEANING
// ================================================================

bool RawSignalLoader::loadRawDataFile(const QUrl &fileUrl)
{
    const QString localPath = fileUrl.isLocalFile()
                                  ? fileUrl.toLocalFile()
                                  : fileUrl.toString();

    m_rawBytes = cleanRawHexFile(localPath);
    if (m_rawBytes.isEmpty())
        return false;

    if (!m_parameters.isEmpty())
        parseRawData();

    return true;
}

QByteArray RawSignalLoader::cleanRawHexFile(const QString &localPath)
{
    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setLastError(tr("Ham veri dosyası açılamadı: %1").arg(localPath));
        return QByteArray();
    }

    QByteArray fileData = file.readAll();
    file.close();

    // Strip all whitespace — same cleaning step as the original
    fileData.replace(" ", "");
    fileData.replace("\n", "");
    fileData.replace("\r", "");
    fileData.replace("\t", "");

    QByteArray rawData;
    rawData.reserve(fileData.size() / 2);

    for (int i = 0; i + 1 < fileData.size(); i += 2) {
        bool ok = false;
        int value = QString(fileData.mid(i, 2)).toInt(&ok, 16);
        if (!ok) {
            qDebug() << "RawSignalLoader: invalid hex byte:" << fileData.mid(i, 2);
            continue;
        }
        rawData.append(static_cast<char>(value));
    }

    if (rawData.isEmpty()) {
        setLastError(tr("Ham veri dosyasında geçerli hex verisi bulunamadı."));
        return QByteArray();
    }

    qDebug() << "RawSignalLoader: cleaned" << rawData.size() << "bytes from hex file";
    setLastError(QString());
    return rawData;
}

// ================================================================
// STAGE 3 — PARSE + CONVERT
// ================================================================

void RawSignalLoader::parseRawData()
{
    m_parsedParams.clear();

    for (const Parameter &param : m_parameters) {
        const int startAddress = param.ramAddress + param.dataOffset;
        const int endAddress = startAddress + param.dataWidth - 1;

        QVariantMap entry;
        entry[QStringLiteral("name")]    = param.parameterName;
        entry[QStringLiteral("address")] = param.ramAddress;
        entry[QStringLiteral("offset")]  = param.dataOffset;
        entry[QStringLiteral("width")]   = param.dataWidth;
        entry[QStringLiteral("type")]    = param.dataType;
        entry[QStringLiteral("formula")] = param.conversionFormula;

        if (startAddress < 0 || endAddress >= m_rawBytes.size()) {
            entry[QStringLiteral("rawValue")]       = QStringLiteral("—");
            entry[QStringLiteral("convertedValue")] = QStringLiteral("Adres aralık dışı");
            entry[QStringLiteral("numericValue")]   = 0.0;
            m_parsedParams.append(entry);
            continue;
        }

        QByteArray parameterData = m_rawBytes.mid(startAddress, param.dataWidth);

        QVariant rawValue = parseValue(parameterData, param.dataType);
        QVariant converted = applyConversion(rawValue, param.conversionFormula);

        entry[QStringLiteral("rawValue")]       = rawValue;
        entry[QStringLiteral("convertedValue")] = converted;
        entry[QStringLiteral("numericValue")]   = converted.toDouble();
        m_parsedParams.append(entry);
    }

    m_loaded = true;
    emit dataChanged();
}

// ================================================================
// TYPE DECODING (from Aybüke's RawDataParser)
// ================================================================

QVariant RawSignalLoader::parseValue(const QByteArray &data,
                                     const QString &dataType)
{
    if (data.isEmpty())
        return QVariant();

    if (dataType == QStringLiteral("Unsigned")) {
        quint64 value = 0;
        for (unsigned char byte : data)
            value = (value << 8) | byte;
        return QVariant::fromValue(value);
    }

    if (dataType == QStringLiteral("Signed")) {
        qint64 value = 0;
        for (unsigned char byte : data)
            value = (value << 8) | byte;
        int bitCount = data.size() * 8;
        if (value & (quint64(1) << (bitCount - 1)))
            value -= (quint64(1) << bitCount);
        return QVariant::fromValue(value);
    }

    if (dataType == QStringLiteral("Float")) {
        if (data.size() != 4)
            return QVariant();
        quint32 rawValue = 0;
        for (unsigned char byte : data)
            rawValue = (rawValue << 8) | byte;
        float value;
        memcpy(&value, &rawValue, sizeof(float));
        return QVariant::fromValue(value);
    }

    return QVariant();
}

// ================================================================
// CONVERSION FORMULA (from Aybüke's RawDataParser)
// ================================================================

QVariant RawSignalLoader::applyConversion(const QVariant &rawValue,
                                          const QString &formula)
{
    if (!rawValue.isValid())
        return QVariant();

    if (formula.isEmpty() || formula == QStringLiteral("-"))
        return rawValue;

    if (formula.compare(QStringLiteral("Hex"), Qt::CaseInsensitive) == 0
        || formula.compare(QStringLiteral("Bitmask"), Qt::CaseInsensitive) == 0
        || formula.compare(QStringLiteral("Enum"), Qt::CaseInsensitive) == 0
        || formula.startsWith(QStringLiteral("Major.Minor")))
        return rawValue;

    bool ok = false;
    double x = rawValue.toDouble(&ok);
    if (!ok)
        return rawValue;

    QString expression = formula.trimmed();

    // Strip unit suffix in parentheses
    int unitStart = expression.indexOf('(');
    if (unitStart != -1)
        expression = expression.left(unitStart).trimmed();

    if (expression == QStringLiteral("x"))
        return x;

    if (expression.startsWith(QStringLiteral("x *"))) {
        double factor = expression.mid(3).trimmed().toDouble(&ok);
        if (ok) return x * factor;
    }

    if (expression.startsWith(QStringLiteral("x /"))) {
        double divisor = expression.mid(3).trimmed().toDouble(&ok);
        if (ok && divisor != 0) return x / divisor;
    }

    if (expression.startsWith(QStringLiteral("x +"))) {
        double offset = expression.mid(3).trimmed().toDouble(&ok);
        if (ok) return x + offset;
    }

    if (expression.startsWith(QStringLiteral("x -"))) {
        double offset = expression.mid(3).trimmed().toDouble(&ok);
        if (ok) return x - offset;
    }

    qDebug() << "RawSignalLoader: unsupported formula:" << formula;
    return rawValue;
}

// ================================================================
// RAW BYTES FOR CHART
// ================================================================

QVariantList RawSignalLoader::rawByteValues() const
{
    QVariantList list;
    list.reserve(m_rawBytes.size());
    for (int i = 0; i < m_rawBytes.size(); ++i)
        list.append(static_cast<int>(static_cast<unsigned char>(m_rawBytes.at(i))));
    return list;
}

// ================================================================

void RawSignalLoader::setLastError(const QString &message)
{
    if (m_lastError == message)
        return;
    m_lastError = message;
    emit lastErrorChanged();
}
