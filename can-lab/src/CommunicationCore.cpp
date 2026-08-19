#include "CommunicationCore.h"

#include "CanChannelWorker.h"

#include <QCanBus>
#include <QCanBusDeviceInfo>
#include <QDebug>
#include <QMetaObject>
#include <QThread>
#include <QTimer>

#include <utility>


namespace {
constexpr quint32 DiscoveryRequestId = 0x7A0;
constexpr quint8 DiscoveryRequest = 0xD1;

constexpr quint32 DiscoveryResponseFirstId = 0x7A1; //response id range
constexpr quint32 DiscoveryResponseLastId = 0x7AF;

constexpr quint8 DiscoveryResponse = 0xD2;

constexpr quint8 CommandOn = 0x01;
constexpr quint8 CommandOff = 0x02;
constexpr quint8 CommandRequestStatus = 0x05;

constexpr qint64 HeartbeatTimeoutMs = 500;
}

CommunicationCore::CommunicationCore(QString plugin, QObject *parent) : QObject(parent), m_plugin(std::move(plugin)) // class inherits from QObject, 
{                                                                                                                    // avoid copying in memory with move()
    // every 2sec search for new socketCAN                                                                                
    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(2000);
    connect(m_scanTimer, &QTimer::timeout, 
            this, &CommunicationCore::scanInterfaces); // polling exist maybe changed !!!

    m_discoveryTimer = new QTimer(this);
    m_discoveryTimer->setSingleShot(true); // fire once on start up
    m_discoveryTimer->setInterval(750);  // 750 ms timeout
    connect(m_discoveryTimer, &QTimer::timeout, this, [this] { // on timeout emit func
        emit ecuDiscoveryCompleted(m_routes.size());
    });

    // polling  
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setInterval(100);
    connect(m_heartbeatTimer, &QTimer::timeout,
            this, &CommunicationCore::checkHeartbeatTimeouts);
}

CommunicationCore::~CommunicationCore()
{
    stop();
}

void CommunicationCore::start()
{
    if (m_scanTimer->isActive())
        return;

    m_clock.start();
    scanInterfaces();
    m_scanTimer->start();
    m_heartbeatTimer->start();
}

void CommunicationCore::scanInterfaces()
{
    QString error;
    const auto devices = QCanBus::instance()->availableDevices(m_plugin, &error);
    if (!error.isEmpty())
        emit errorOccurred(error);

    for (const QCanBusDeviceInfo &device : devices) {
        if (device.name().isEmpty() || m_channels.contains(device.name()))
            continue;
        emit channelDiscovered(device.name(), device.hasFlexibleDataRate());
        addChannel(device.name(), device.hasFlexibleDataRate());
        m_channels.value(device.name()).thread->start();
    }

    if (devices.isEmpty() && m_channels.isEmpty()) {
        emit errorOccurred(QStringLiteral(
            "No SocketCAN interface found; scanning again in 2 seconds."));
    }
}

void CommunicationCore::addChannel(const QString &channel, bool supportsCanFd)
{
    auto *thread = new QThread(this);
    auto *worker = new CanChannelWorker(m_plugin, channel, supportsCanFd);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &CanChannelWorker::initialize);
    connect(this, &CommunicationCore::stopWorkers,
            worker, &CanChannelWorker::shutdown, Qt::QueuedConnection);
    connect(worker, &CanChannelWorker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &CanChannelWorker::connected,
            this, &CommunicationCore::workerConnected, Qt::QueuedConnection);
    connect(worker, &CanChannelWorker::frameReceived,
            this, &CommunicationCore::processIncomingFrame, Qt::QueuedConnection);
    connect(worker, &CanChannelWorker::errorOccurred,
            this, [this](const QString &name, const QString &description) {
                emit errorOccurred(name + QStringLiteral(": ") + description);
            }, Qt::QueuedConnection);

    m_channels.insert(channel, {thread, worker});
}

void CommunicationCore::workerConnected(QString channel)
{
    m_connectedChannels.insert(channel);
    emit channelConnected(channel);
    // Discover immediately on each usable bus. An unavailable adapter must
    // never block discovery on the other buses.
    broadcastEcuDiscovery(channel);
}

void CommunicationCore::broadcastEcuDiscovery(const QString &channel)
{
    const QByteArray request(1, char(DiscoveryRequest));
    if (!channel.isEmpty()) {
        sendOnChannel(channel, DiscoveryRequestId, false, request);
    } else {
        for (const QString &connected : std::as_const(m_connectedChannels))
            sendOnChannel(connected, DiscoveryRequestId, false, request);
    }
    scheduleDiscoveryCompleted();
}

void CommunicationCore::scheduleDiscoveryCompleted()
{
    // Debounce responses when multiple interfaces connect close together.
    m_discoveryTimer->start();
}

void CommunicationCore::processIncomingFrame(CanFrame frame)
{
    if (frame.id >= DiscoveryResponseFirstId &&
        frame.id <= DiscoveryResponseLastId &&
        frame.payload.size() == 8 &&
        quint8(frame.payload[0]) == DiscoveryResponse) {
        const char ecu = frame.payload[1];
        Route route;
        route.channel = frame.channel;
        route.commandId = (quint32(quint8(frame.payload[2])) << 8) |
                          quint32(quint8(frame.payload[3]));
        route.statusId = (quint32(quint8(frame.payload[4])) << 8) |
                         quint32(quint8(frame.payload[5]));
        route.heartbeatId = (quint32(quint8(frame.payload[6])) << 8) |
                            quint32(quint8(frame.payload[7]));

        const auto existing = m_routes.constFind(ecu);
        if (existing != m_routes.cend() && existing->channel != route.channel) {
            emit errorOccurred(QStringLiteral("ECU %1 appeared on multiple channels")
                               .arg(QChar(ecu)));
            return;
        }
        if (existing == m_routes.cend()) {
            m_routes.insert(ecu, route);
            emit ecuDiscovered(ecu, route.channel, route.commandId);
        }
        return;
    }

    for (auto it = m_routes.cbegin(); it != m_routes.cend(); ++it) {
        const char ecu = it.key();
        const Route &route = it.value();
        if (frame.channel == route.channel && frame.id == route.heartbeatId &&
            frame.payload.size() >= 3 && frame.payload[0] == ecu) {
            m_lastHeartbeatMs[ecu] = m_clock.elapsed();
            if (!m_onlineEcus.contains(ecu)) {
                m_onlineEcus.insert(ecu);
                emit ecuOnlineChanged(ecu, true);
            }
            emit ecuHeartbeat(ecu, quint8(frame.payload[2]),
                              quint8(frame.payload[1]));
            return;
        }
        if (frame.channel == route.channel && frame.id == route.statusId &&
            frame.payload.size() >= 2) {
            emit ecuStatus(ecu, quint8(frame.payload[0]),
                           quint8(frame.payload[1]));
            return;
        }
    }

    emit frameReceived(frame);
}

void CommunicationCore::checkHeartbeatTimeouts()
{
    const qint64 now = m_clock.elapsed();
    const QSet<char> onlineEcus = m_onlineEcus;
    for (char ecu : onlineEcus) {
        if (now - m_lastHeartbeatMs.value(ecu, 0) > HeartbeatTimeoutMs) {
            m_onlineEcus.remove(ecu);
            emit ecuOnlineChanged(ecu, false);
        }
    }
}

void CommunicationCore::sendToEcu(char ecu, QByteArray payload, bool isFd)
{
    const auto route = m_routes.constFind(ecu);
    if (route == m_routes.cend()) {
        emit errorOccurred(QStringLiteral("ECU %1 has not been discovered")
                           .arg(QChar(ecu)));
        return;
    }
    sendOnChannel(route->channel, route->commandId, isFd, payload);
}

void CommunicationCore::sendCommand(char ecu, quint8 command)
{
    if (!m_routes.contains(ecu)) {
        emit errorOccurred(QStringLiteral("ECU %1 has not been discovered")
                           .arg(QChar(ecu)));
        return;
    }
    sendToEcu(ecu, QByteArray(1, char(command)));
    emit commandSent(ecu, command);
}

void CommunicationCore::powerOn(char ecu)
{
    sendCommand(ecu, CommandOn);
}

void CommunicationCore::powerOff(char ecu)
{
    sendCommand(ecu, CommandOff);
}

void CommunicationCore::requestStatus(char ecu)
{
    sendCommand(ecu, CommandRequestStatus);
}

void CommunicationCore::sendOnChannel(const QString &channel, quint32 id,
                                      bool isFd, const QByteArray &payload)
{
    const auto context = m_channels.constFind(channel);
    if (context == m_channels.cend()) {
        emit errorOccurred(QStringLiteral("Unknown CAN channel: ") + channel);
        return;
    }
    QMetaObject::invokeMethod(context->worker, "sendFrame", Qt::QueuedConnection,
                              Q_ARG(quint32, id), Q_ARG(bool, isFd),
                              Q_ARG(QByteArray, payload));
}

void CommunicationCore::stop()
{
    m_scanTimer->stop();
    m_discoveryTimer->stop();
    m_heartbeatTimer->stop();
    emit stopWorkers();
    for (const ChannelContext &context : std::as_const(m_channels)) {
        if (context.thread->isRunning()) {
            context.thread->quit();
            context.thread->wait(3000);
        }
    }
    m_channels.clear();
    m_routes.clear();
    m_lastHeartbeatMs.clear();
    m_onlineEcus.clear();
    m_connectedChannels.clear();
}
