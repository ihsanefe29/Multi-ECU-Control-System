#pragma once

#include <QObject>
#include <QString>

// The QML-facing side of one module. Lives on the GUI thread (same
// thread as the QQmlEngine) so QML property bindings on it are legal
// - this is the object Dispatcher exposes as ecuAModule1 etc.
//
// It holds NO protocol logic. User actions (connectModule/setPower/
// etc.) just emit request signals; ModuleWorker (on the ECU's worker
// thread) is what actually does the CCP/CAN FD work and reports
// results back via applyXxx() slots, wired up by Dispatcher.
class ModuleSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ecuId READ ecuId CONSTANT)
    Q_PROPERTY(int moduleNumber READ moduleNumber CONSTANT)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(QString powerState READ powerState NOTIFY powerStateChanged)
    Q_PROPERTY(QString ledState READ ledState NOTIFY ledStateChanged)

public:
    ModuleSession(QString ecuId, int moduleNumber, QObject *parent = nullptr);

    QString ecuId() const { return m_ecuId; }
    int moduleNumber() const { return m_moduleNumber; }
    bool connected() const { return m_connected; }
    QString mode() const { return m_mode; }
    QString powerState() const { return m_powerState; }
    QString ledState() const { return m_ledState; }

    // Called by Dispatcher (same thread, direct calls) in response to
    // QML's Q_INVOKABLE calls on Dispatcher.
    void connectModule() { emit requestConnect(); }
    void disconnectModule() { emit requestDisconnect(); }
    void setMode(const QString &mode) { emit requestSetMode(mode); }
    void setPower(const QString &state) { emit requestSetPower(state); }

public slots:
    // Called by ModuleWorker via queued cross-thread signal delivery.
    void applyConnected(bool connected);
    void applyMode(const QString &mode);
    void applyPowerState(const QString &state);
    void applyLedState(const QString &state);

signals:
    void connectedChanged();
    void modeChanged();
    void powerStateChanged();
    void ledStateChanged();

    // Outgoing requests toward ModuleWorker - Dispatcher connects
    // these to the matching ModuleWorker slots.
    void requestConnect();
    void requestDisconnect();
    void requestSetMode(const QString &mode);
    void requestSetPower(const QString &state);

private:
    QString m_ecuId;
    int m_moduleNumber;
    bool m_connected = false;
    QString m_mode = QStringLiteral("DataView");
    QString m_powerState = QStringLiteral("Off");
    QString m_ledState = QStringLiteral("inactive");
};
