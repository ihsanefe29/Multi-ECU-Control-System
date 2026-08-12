#include "livesignalsource.h"

#include <QRandomGenerator>
#include <cmath>

LiveSignalSource::LiveSignalSource(QObject *parent)
    : QObject(parent)
{
    m_timer.setInterval(int(1000.0 / kSampleRateHz)); // 50ms @ 20Hz
    connect(&m_timer, &QTimer::timeout, this, &LiveSignalSource::tick);
}

void LiveSignalSource::start()
{
    if (m_timer.isActive())
        return;
    m_elapsed = 0.0;
    m_frequencyHz = 2.0;
    m_freqDirection = 1.0;
    m_timer.start();
    emit runningChanged();
    emit paramsChanged();
}

void LiveSignalSource::stop()
{
    if (!m_timer.isActive())
        return;
    m_timer.stop();
    emit runningChanged();
}

void LiveSignalSource::tick()
{
    m_elapsed += 1.0 / kSampleRateHz;

    // Slowly sweep the frequency between 1 Hz and 5 Hz so the live
    // view has something visually changing over time, rather than a
    // perfectly static sine wave.
    m_frequencyHz += m_freqDirection * 0.02;
    if (m_frequencyHz >= 5.0) {
        m_frequencyHz = 5.0;
        m_freqDirection = -1.0;
    } else if (m_frequencyHz <= 1.0) {
        m_frequencyHz = 1.0;
        m_freqDirection = 1.0;
    }

    const double noise = 0.03 * (QRandomGenerator::global()->generateDouble() - 0.5);
    const double value = m_amplitude * std::sin(2.0 * M_PI * m_frequencyHz * m_elapsed) + noise;

    emit paramsChanged();
    emit sampleGenerated(m_elapsed, value);
}
