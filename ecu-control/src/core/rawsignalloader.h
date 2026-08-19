#pragma once

#include <QObject>
#include <QUrl>
#include <QVector>
#include <QVariantList>
#include <QVariantMap>

// Integrates the three-stage signal cleaning and parsing pipeline:
//
//   1. Parameter definition loading (CSV: address, offset, width,
//      type, name, formula)
//   2. Raw hex data cleaning (strip whitespace, hex-pair → byte)
//   3. Parsing: extract each parameter's bytes from the cleaned
//      buffer, decode by type (Unsigned/Signed/Float), apply
//      conversion formula
//
// Exposed to QML through Dispatcher as dispatcher.rawSignalLoader.
class RawSignalLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ loaded NOTIFY dataChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(int parameterCount READ parameterCount NOTIFY dataChanged)
    Q_PROPERTY(int rawByteCount READ rawByteCount NOTIFY dataChanged)
    Q_PROPERTY(QVariantList parsedParameters READ parsedParameters NOTIFY dataChanged)
    Q_PROPERTY(QVariantList rawByteValues READ rawByteValues NOTIFY dataChanged)

public:
    explicit RawSignalLoader(QObject *parent = nullptr);

    bool loaded() const { return m_loaded; }
    QString lastError() const { return m_lastError; }
    int parameterCount() const { return m_parsedParams.size(); }
    int rawByteCount() const { return m_rawBytes.size(); }

    QVariantList parsedParameters() const { return m_parsedParams; }
    QVariantList rawByteValues() const;

    Q_INVOKABLE bool loadParameterFile(const QUrl &fileUrl);
    Q_INVOKABLE bool loadRawDataFile(const QUrl &fileUrl);

signals:
    void dataChanged();
    void lastErrorChanged();

private:
    struct Parameter {
        int ramAddress;
        int dataOffset;
        int dataWidth;
        QString dataType;
        QString parameterName;
        QString conversionFormula;
    };

    bool readParameterCsv(const QString &localPath);
    bool readParameterExcel(const QString &localPath);
    QByteArray cleanRawHexFile(const QString &localPath);
    void parseRawData();

    QVariant parseValue(const QByteArray &data, const QString &dataType);
    QVariant applyConversion(const QVariant &rawValue, const QString &formula);

    void setLastError(const QString &message);

    bool m_loaded = false;
    QString m_lastError;

    QVector<Parameter> m_parameters;
    QByteArray m_rawBytes;
    QVariantList m_parsedParams;
};
