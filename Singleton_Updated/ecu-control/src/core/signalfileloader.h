#pragma once

#include <QObject>
#include <QVariantList>
#include <QUrl>
#include <QString>

// Loads a 2-column [time, amplitude] signal file (comma or
// whitespace separated, header row tolerated - same format as the
// earlier accelerometer file loader) and exposes:
//   - descriptive parameters about the signal itself (file name,
//     sample count, sample rate, duration)
//   - statistical measurements computed from the amplitude values
//     (min, max, mean, RMS, peak-to-peak)
//   - the raw time/amplitude arrays for plotting
//
// This is a plain time-domain plot - no FFT/windowing involved (that
// was a different, separate feature that was removed).
class SignalFileLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ loaded NOTIFY dataChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY dataChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

    // Descriptive parameters
    Q_PROPERTY(int sampleCount READ sampleCount NOTIFY dataChanged)
    Q_PROPERTY(double sampleRateHz READ sampleRateHz NOTIFY dataChanged)
    Q_PROPERTY(double durationSeconds READ durationSeconds NOTIFY dataChanged)

    // Computed measurements
    Q_PROPERTY(double minValue READ minValue NOTIFY dataChanged)
    Q_PROPERTY(double maxValue READ maxValue NOTIFY dataChanged)
    Q_PROPERTY(double meanValue READ meanValue NOTIFY dataChanged)
    Q_PROPERTY(double rmsValue READ rmsValue NOTIFY dataChanged)
    Q_PROPERTY(double peakToPeakValue READ peakToPeakValue NOTIFY dataChanged)

    // Plot data
    Q_PROPERTY(QVariantList timeValues READ timeValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList amplitudeValues READ amplitudeValues NOTIFY dataChanged)

public:
    explicit SignalFileLoader(QObject *parent = nullptr);

    bool loaded() const { return m_loaded; }
    QString fileName() const { return m_fileName; }
    QString lastError() const { return m_lastError; }

    int sampleCount() const { return m_time.size(); }
    double sampleRateHz() const { return m_sampleRateHz; }
    double durationSeconds() const { return m_durationSeconds; }

    double minValue() const { return m_minValue; }
    double maxValue() const { return m_maxValue; }
    double meanValue() const { return m_meanValue; }
    double rmsValue() const { return m_rmsValue; }
    double peakToPeakValue() const { return m_maxValue - m_minValue; }

    QVariantList timeValues() const;
    QVariantList amplitudeValues() const;

    // fileUrl comes straight from a QML FileDialog's `file` property.
    Q_INVOKABLE bool loadFile(const QUrl &fileUrl);

signals:
    void dataChanged();
    void lastErrorChanged();

private:
    bool readFile(const QString &localPath);
    void computeStatistics();
    void setLastError(const QString &message);

    bool m_loaded = false;
    QString m_fileName;
    QString m_lastError;

    QVector<float> m_time;
    QVector<float> m_amplitude;

    double m_sampleRateHz = 0.0;
    double m_durationSeconds = 0.0;
    double m_minValue = 0.0;
    double m_maxValue = 0.0;
    double m_meanValue = 0.0;
    double m_rmsValue = 0.0;
};
