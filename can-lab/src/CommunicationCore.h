#pragma once

#include "CanFrame.h"

#include <QHash>
#include <QElapsedTimer>
#include <QObject>
#include <QSet>

class CanChannelWorker;
class QThread;
class QTimer;

class CommunicationCore final : public QObject
{
    Q_OBJECT

public:
    explicit CommunicationCore(QString plugin = QStringLiteral("socketcan"), QObject *parent = nullptr); // could set to different plugin yet if null set as socketcan
                                                                                                         // if a parent setted to this object on parent object delete also delete this
    ~CommunicationCore() override; // safe disconenct on delete

public slots:
    void start();
    void stop();
    void sendToEcu(char ecu, QByteArray payload, bool isFd = false);
    void powerOn(char ecu);
    void powerOff(char ecu);
    void requestStatus(char ecu);

signals:
    void stopWorkers();
    void channelDiscovered(QString channel, bool supportsCanFd);
    void channelConnected(QString channel);
    void ecuDiscovered(char ecu, QString channel, quint32 commandId);
    void ecuDiscoveryCompleted(int ecuCount);
    void ecuHeartbeat(char ecu, quint8 counter, quint8 powerState);
    void ecuOnlineChanged(char ecu, bool online);
    void ecuStatus(char ecu, quint8 powerState, quint8 result);
    void commandSent(char ecu, quint8 command);
    void frameReceived(CanFrame frame);
    void errorOccurred(QString description);

private slots:
    void scanInterfaces();
    void workerConnected(QString channel);
    void processIncomingFrame(CanFrame frame);
    void checkHeartbeatTimeouts();

private:
    struct Route {
        QString channel;
        quint32 commandId = 0;
        quint32 statusId = 0;
        quint32 heartbeatId = 0;
    };
    struct ChannelContext {
        QThread *thread = nullptr;
        CanChannelWorker *worker = nullptr;
    };

    void addChannel(const QString &channel, bool supportsCanFd);
    void broadcastEcuDiscovery(const QString &channel = {});
    void scheduleDiscoveryCompleted();
    void sendOnChannel(const QString &channel, quint32 id, bool isFd, const QByteArray &payload);
    void sendCommand(char ecu, quint8 command);

    QString m_plugin;
    QHash<QString, ChannelContext> m_channels;
    QHash<char, Route> m_routes;
    QHash<char, qint64> m_lastHeartbeatMs;
    QSet<char> m_onlineEcus;
    QSet<QString> m_connectedChannels;
    QTimer *m_scanTimer = nullptr;
    QTimer *m_discoveryTimer = nullptr;
    QTimer *m_heartbeatTimer = nullptr;
    QElapsedTimer m_clock;
};
