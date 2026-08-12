#pragma once

#include <QObject>
#include <QTimer>

// A mock "live" signal source - simulates data arriving in real time
// (as opposed to SignalFileLoader, which loads a complete file at
// once). Emits one new sample per tick via sampleGenerated(), meant
// to be plotted as a scrolling/rolling window rather than a static
// full-signal plot.
//
// Not tied to real hardware yet - stands in for what would eventually
// be a live CCP/CAN FD measurement stream from a ModuleWorker.
class LiveSignalSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(double frequencyHz READ frequencyHz NOTIFY paramsChanged)
    Q_PROPERTY(double amplitude READ amplitude NOTIFY paramsChanged)
    Q_PROPERTY(double sampleRateHz READ sampleRateHz CONSTANT)

public:
    explicit LiveSignalSource(QObject *parent = nullptr);

    bool running() const { return m_timer.isActive(); }
    double frequencyHz() const { return m_frequencyHz; }
    double amplitude() const { return m_amplitude; }
    double sampleRateHz() const { return kSampleRateHz; }

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void runningChanged();
    void paramsChanged();
    // Emitted once per tick with the new sample. `time` is seconds
    // since start() was called, resets to 0 each time start() runs.
    void sampleGenerated(double time, double value);

private:
    void tick();

    QTimer m_timer;
    double m_elapsed = 0.0;
    double m_frequencyHz = 2.0;   // slowly drifts for visual interest
    double m_freqDirection = 1.0;
    double m_amplitude = 1.0;
    // Ticks per second - also displayed as the "sample rate" parameter.
    static constexpr double kSampleRateHz = 20.0;
};
