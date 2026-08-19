#pragma once
#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QVector>
#include "parameter.h"

// Ham binary ECU verisini parametre tablosuna göre çözümler.
// (aybuke branch — RawDataParser)
//
// Akış:
//   1. setParameters() ile parametre listesini yükle (ExcelParser'dan gelir)
//   2. parse(rawData, parameters) çağır → values property güncellenir
//   3. QML: rawDataParser.values["N1"] şeklinde erişir
//
// Bizim RawSignalLoader'dan farkı:
//   - Bit-level extraction (extractBits) — aynı byte içindeki birden fazla
//     parametre okunabilir
//   - QVariantMap values property — QML'e doğrudan map olarak açılır
//   - ExcelParser ile doğrudan entegrasyon
class RawDataParser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap values READ values NOTIFY valuesChanged)

public:
    explicit RawDataParser(QObject *parent = nullptr);

    void setParameters(const QVector<Parameter> &parameters);

    // rawData: DataSimulator'dan gelen 16-byte paket
    // parameters: ExcelParser'dan yüklenen parametre listesi
    void parse(const QByteArray &rawData,
               const QVector<Parameter> &parameters);

    QVariantMap values() const;

    Q_INVOKABLE QVariantMap parameterInfo(const QString &parameterName) const;

signals:
    void valuesChanged();

private:
    quint64  extractBits(const QByteArray &data, int bitOffset, int bitWidth);
    QVariant parseValue(const QByteArray &data, const QString &dataType,
                        int bitOffset, int bitWidth);
    QVariant applyConversion(const QVariant &rawValue, const QString &formula);

    QVector<Parameter> m_parameters;
    QVariantMap        m_values;
};
