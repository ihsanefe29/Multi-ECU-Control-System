#include "signalfileloader.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QRegularExpression>
#include <cmath>

SignalFileLoader::SignalFileLoader(QObject *parent)
    : QObject(parent)
{
}

QVariantList SignalFileLoader::timeValues() const
{
    QVariantList list;
    list.reserve(m_time.size());
    for (float v : m_time)
        list.append(double(v));
    return list;
}

QVariantList SignalFileLoader::amplitudeValues() const
{
    QVariantList list;
    list.reserve(m_amplitude.size());
    for (float v : m_amplitude)
        list.append(double(v));
    return list;
}

bool SignalFileLoader::loadFile(const QUrl &fileUrl)
{
    const QString localPath = fileUrl.isLocalFile() ? fileUrl.toLocalFile() : fileUrl.toString();

    if (!readFile(localPath)) {
        // readFile() already called setLastError() with specifics.
        return false;
    }

    computeStatistics();
    m_fileName = QFileInfo(localPath).fileName();
    m_loaded = true;
    setLastError(QString());
    emit dataChanged();
    return true;
}

bool SignalFileLoader::readFile(const QString &localPath)
{
    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setLastError(tr("Dosya açılamadı: %1").arg(localPath));
        return false;
    }

    static const QRegularExpression separator("[,\\s]+");

    QTextStream in(&file);
    QVector<float> time;
    QVector<float> amplitude;

    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // Accepts comma- or whitespace-separated [time, amplitude].
        // Header rows are skipped naturally - non-numeric tokens
        // simply fail toFloat() and the line is dropped.
        const QStringList parts = line.split(separator, Qt::SkipEmptyParts);
        if (parts.size() < 2)
            continue;

        bool timeOk = false, ampOk = false;
        const float t = parts.at(0).toFloat(&timeOk);
        const float a = parts.at(1).toFloat(&ampOk);
        if (timeOk && ampOk) {
            time.append(t);
            amplitude.append(a);
        }
    }

    if (time.size() < 2) {
        setLastError(tr("Dosyada yeterli veri yok (en az 2 örnek gerekli, %1 okundu).").arg(time.size()));
        return false;
    }

    m_time = std::move(time);
    m_amplitude = std::move(amplitude);
    return true;
}

void SignalFileLoader::computeStatistics()
{
    const int n = m_amplitude.size();

    // Sample rate derived from the file's own time column (average
    // delta across the whole signal), same approach as the earlier
    // file loader - don't assume a fixed rate.
    const double totalSpan = double(m_time.last()) - double(m_time.first());
    m_sampleRateHz = (totalSpan > 0.0) ? double(n - 1) / totalSpan : 0.0;
    m_durationSeconds = totalSpan;

    double sum = 0.0, sumSq = 0.0;
    float minV = m_amplitude.first();
    float maxV = m_amplitude.first();
    for (float v : m_amplitude) {
        sum += v;
        sumSq += double(v) * double(v);
        if (v < minV) minV = v;
        if (v > maxV) maxV = v;
    }

    m_minValue = minV;
    m_maxValue = maxV;
    m_meanValue = sum / n;
    m_rmsValue = std::sqrt(sumSq / n);
}

void SignalFileLoader::setLastError(const QString &message)
{
    if (m_lastError == message)
        return;
    m_lastError = message;
    emit lastErrorChanged();
}
