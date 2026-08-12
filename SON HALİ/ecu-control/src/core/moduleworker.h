#pragma once

#include <QObject>
#include <QString>
#include "icanfdconnector.h"

// Does the actual CCP/CAN FD work for ONE module, entirely on its
// ECU's worker thread. NEVER exposed to QML directly - QML only ever
// sees ModuleSession (GUI thread). This object talks to ModuleSession
// purely via signals, which Qt auto-delivers as queued cross-thread
// calls since sender/receiver live on different threads.
class ModuleWorker : public QObject
{
    Q_OBJECT

public:
    ModuleWorker(QString ecuId, int moduleNumber, quint32 canId,
                 ICanFdConnector *bus, QObject *parent = nullptr);

public slots:
    // Connected (via Dispatcher's wiring) to ModuleSession's request
    // signals - this is where a QML user action actually lands.
    void onRequestConnect();
    void onRequestDisconnect();
    void onRequestSetMode(const QString &mode);       // "DataView" | "FirmwareUpload"
    void onRequestSetPower(const QString &state);      // "On" | "Off" | "Kill"

signals:
    // Reported back to ModuleSession (GUI thread) after each change.
    void connectedChanged(bool connected);
    void modeChanged(const QString &mode);
    void powerStateChanged(const QString &state);
    void ledStateChanged(const QString &state); // "inactive" | "pending" | "success" | "fail"

private slots:
    void onFrameReceived(quint32 canId, QByteArray data);

private:
    void setLedState(const QString &state);
    void sendCcpFrame(const QByteArray &frame);

    QString m_ecuId;
    int m_moduleNumber;
    quint32 m_canId;
    ICanFdConnector *m_bus;

    bool m_connected = false;
    QString m_mode = QStringLiteral("DataView");
    QString m_powerState = QStringLiteral("Off"); // Off / On / Kill
    QString m_ledState = QStringLiteral("inactive");

    quint8 m_ccpCounter = 0;
    enum class PendingOp { None, Connect, GetSeed, Unlock, Disconnect } m_pendingOp = PendingOp::None;
};
