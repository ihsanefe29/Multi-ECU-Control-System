#include "CommunicationCore.h"

#include "CanChannelWorker.h"

#include <QCanBus>
#include <QCanBusDeviceInfo>
#include <QDebug>
#include <QMetaObject>
#include <QThread>
#include <QTimer>

#include <utility>


/*namespace {
constexpr quint32 DiscoveryRequestId = 0x7A0;
constexpr quint8 DiscoveryRequest = 0xD1;

constexpr quint32 DiscoveryResponseFirstId = 0x7A1; //response id range
constexpr quint32 DiscoveryResponseLastId = 0x7AF;

constexpr quint8 DiscoveryResponse = 0xD2;

constexpr quint8 CommandOn = 0x01;
constexpr quint8 CommandOff = 0x02;
constexpr quint8 CommandRequestStatus = 0x05;

constexpr qint64 HeartbeatTimeoutMs = 500;
}*/

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

    m_clock.start(); //start or restart the timer to record current time
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
        emit channelDiscovered(device.name(), device.hasFlexibleDataRate()); // bool QCanBusDeviceInfo::hasFlexibleDataRate() const => returns true if device is CAN FD capable
        addChannel(device.name(), device.hasFlexibleDataRate());
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
    worker->moveToThread(thread); // move worker to specific thread

    connect(thread, &QThread::started, worker, &CanChannelWorker::initialize); // initialize devşce once thread started
    connect(this, &CommunicationCore::stopWorkers, worker, &CanChannelWorker::shutdown, Qt::QueuedConnection); // when stopWorkers signal emmitted shutdown channel
    connect(worker, &CanChannelWorker::finished, thread, &QThread::quit); // quit thread when worker emits finish signal
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    // worker emits signals which will be recieved by main core thread thus Qt posts 
    // a queued call for the connected slot to the receiver object's thread event queue, 
    //so that multiple calls from multiple threads will be handled in a queue.  
    connect(worker, &CanChannelWorker::connected,
            this, &CommunicationCore::workerConnected, Qt::QueuedConnection);
    connect(worker, &CanChannelWorker::frameReceived,
            this, &CommunicationCore::processIncomingFrame, Qt::QueuedConnection);
    connect(worker, &CanChannelWorker::errorOccurred,
            this, [this](const QString &name, const QString &description) {    // make sure lamda works in this thread && if this deleted remove dependincy
                emit errorOccurred(name + QStringLiteral(": ") + description);
            }, Qt::QueuedConnection);

    m_channels.insert(channel, {thread, worker}); // insert into hash map

    thread->start();
}

void CommunicationCore::workerConnected(QString channel)
{
    m_connectedChannels.insert(channel);
    emit channelConnected(channel);
    // discover immediately on each usable bus. An unavailable adapter must never block discovery on the other buses.
    broadcastEcuDiscovery(channel);
}

void CommunicationCore::broadcastEcuDiscovery(const QString &channel)
{
    const QByteArray request(1, char(DiscoveryMessage::Request)); // one byte discovery request
    if (!channel.isEmpty()) {
        sendOnChannel(channel, CanProtocol::DiscoveryRequestId, false, request);
    } else {
        // for each connected CAN send a discovery broadcast without changing any detail of  
        // 'm_connectedChannels' by stating std::as_const(m_connectedChannels)
        for (const QString &connected : std::as_const(m_connectedChannels))
            sendOnChannel(connected, CanProtocol::DiscoveryRequestId, false, request);
    }
    scheduleDiscoveryCompleted();
}

void CommunicationCore::scheduleDiscoveryCompleted()
{
    // debounce responses when multiple interfaces connect close together
    m_discoveryTimer->start();
}

void CommunicationCore::processIncomingFrame(CanFrame frame)
{
    // discovery response check
    if (frame.id >= CanProtocol::DiscoveryResponseFirstId && frame.id <= CanProtocol::DiscoveryResponseLastId &&
        frame.payload.size() == 10 && quint8(frame.payload[0]) == static_cast<quint8>(DiscoveryMessage::Response)) {
            
        const char ecu = frame.payload[1]; // extract route info from payload
        Route route;
        route.channel = frame.channel;
        route.isFd = frame.isFd;
        route.commandId = (quint32(quint8(frame.payload[2])) << 8) |
                          quint32(quint8(frame.payload[3]));
        // ex: payload[2] = 0x01 && payload[3] = 0x23 ==> 0x01 << 8 = 0x0100, (0x0100 | 0x23) = 0x0123 ==> new command id
        // same calculations for rest
        route.statusId = (quint32(quint8(frame.payload[4])) << 8) |
                         quint32(quint8(frame.payload[5]));
        route.heartbeatId = (quint32(quint8(frame.payload[6])) << 8) |
                            quint32(quint8(frame.payload[7]));
        route.telemetryId =(quint16(quint8(frame.payload[8])) << 8) |
                            quint16(quint8(frame.payload[9]));

        const auto existing = m_routes.constFind(ecu); // search throgh m_routes hash map
        if (existing != m_routes.cend() && existing->channel != route.channel) { // if route is already exist in m_routes yet seen in a different channel
            emit errorOccurred(QStringLiteral("ECU %1 appeared on multiple channels")
                               .arg(QChar(ecu)));
            return;
        }
        if (existing == m_routes.cend()) { // if it is a new route add to hash map
            m_routes.insert(ecu, route);
            emit ecuDiscovered(ecu, route.channel, route.commandId);
        }
        return;
    }
    // iterate all discoverd routes
    for (auto it = m_routes.cbegin(); it != m_routes.cend(); ++it) {
        const char ecu = it.key(); // in hashed map key is our ecu name
        const Route &route = it.value();
        
        if (frame.channel == route.channel) { 

            //recieve heartbeat
            if(frame.id == route.heartbeatId && frame.payload.size() >= 3 && frame.payload[0] == ecu){
                // if new heartbeat recieved update heartbeat
                m_lastHeartbeatMs[ecu] = m_clock.elapsed();
                // if offline swtich to online 
                if (!m_onlineEcus.contains(ecu)) {
                    m_onlineEcus.insert(ecu);
                    emit ecuOnlineChanged(ecu, true);   
                }
                emit ecuHeartbeat(ecu, quint8(frame.payload[2]), quint8(frame.payload[1]));
                return;    
            }
                
            //recieve status
            if (frame.id == route.statusId && frame.payload.size() >= 2) {
                emit ecuStatus(ecu, quint8(frame.payload[0]), quint8(frame.payload[1]));
                return;
            }

            //revieve telemetry
            if(frame.id == route.telemetryId && !frame.payload.isEmpty() && frame.payload.size() % 8 == 0 ){
                EcuTelemetry telemetry;
                telemetry.ecu = ecu;

                for (int i = 0; i < frame.payload.size(); i+=8)
                {
                    TelemetryValue val;

                    val.ramAddress = (quint32(quint8(frame.payload[i]))     << 24) | (quint32(quint8(frame.payload[i + 1])) << 16) |
                                 (quint32(quint8(frame.payload[i + 2])) << 8)  |    quint32(quint8(frame.payload[i + 3]));



                    val.rawValue = (quint32(quint8(frame.payload[i + 4])) << 24) | (quint32(quint8(frame.payload[i + 5])) << 16) |
                               (quint32(quint8(frame.payload[i + 6])) << 8)  | quint32(quint8(frame.payload[i + 7]));

                    telemetry.values.append(val);   
                }
                
                /*qInfo() << "Telemetry ECU:" << ecu;

                /*for (const TelemetryValue &value : telemetry.values) {
               qInfo() << "RAM:" << value.ramAddress << "RAW:" << value.rawValue;
                }*/
                emit telemetryReceived(telemetry);
                return;
            }
        }
    }
    emit frameReceived(frame);
}

void CommunicationCore::checkHeartbeatTimeouts()
{
    const qint64 now = m_clock.elapsed(); // passed ms from start
    const QSet<char> onlineEcus = m_onlineEcus;
    for (char ecu : onlineEcus) {
        // if passed more than 500ms since last heartbeat not online
        if (now - m_lastHeartbeatMs.value(ecu, 0) > CanProtocol::HeartbeatTimeoutMs) { 
            m_onlineEcus.remove(ecu);
            emit ecuOnlineChanged(ecu, false);
        }
    }
}

/*void CommunicationCore::sendToEcu(char ecu, QByteArray payload)
{
    const auto route = m_routes.constFind(ecu);
    if (route == m_routes.cend()) {
        emit errorOccurred(QStringLiteral("ECU %1 has not been discovered")
                           .arg(QChar(ecu)));
        return;
    }
    sendOnChannel(route->channel, route->commandId, route->isFd, payload);
}*/

void CommunicationCore::sendCommand(char ecu, EcuCommand command)
{
    const auto route = m_routes.constFind(ecu);
    if (route == m_routes.cend()) {
        emit errorOccurred(QStringLiteral("ECU %1 has not been discovered")
                           .arg(QChar(ecu)));
        return;
    }
       
    const QByteArray payload(1, char(static_cast<quint8>(command)));

    sendOnChannel(route->channel, route->commandId, route->isFd, payload);
    emit commandSent(ecu, command);
}

void CommunicationCore::powerOn(char ecu)
{
    sendCommand(ecu, EcuCommand::PowerOn);
}

void CommunicationCore::powerOff(char ecu)
{
    sendCommand(ecu, EcuCommand::PowerOff);
}

void CommunicationCore::requestStatus(char ecu)
{
    sendCommand(ecu, EcuCommand::RequestStatus);
}

void CommunicationCore::sendOnChannel(const QString &channel, quint32 id, bool isFd, const QByteArray &payload)
{
    const auto context = m_channels.constFind(channel); // iterates inside hash map till end of hash
    if (context == m_channels.cend()) { // context == end of hash
        emit errorOccurred(QStringLiteral("Unknown CAN channel: ") + channel);
        return;
    }
    // by invokeMethod sends function call from core thread --> worker thread, 
    //then add sendFrame operation to queue of worker thread, but basically: context->worker->sendFrame(...) 
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
