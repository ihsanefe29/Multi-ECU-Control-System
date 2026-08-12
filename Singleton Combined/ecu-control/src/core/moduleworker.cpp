#include "moduleworker.h"
#include "ccpprotocol.h"

ModuleWorker::ModuleWorker(QString ecuId, int moduleNumber, quint32 canId,
                            ICanFdConnector *bus, QObject *parent)
    : QObject(parent)
    , m_ecuId(std::move(ecuId))
    , m_moduleNumber(moduleNumber)
    , m_canId(canId)
    , m_bus(bus)
{
    connect(m_bus, &ICanFdConnector::frameReceived,
            this, &ModuleWorker::onFrameReceived, Qt::QueuedConnection);

    // Parented to `this` BEFORE Dispatcher ever calls moveToThread()
    // on this object - QObject::moveToThread() only carries along
    // actual children of the object being moved. Without this,
    // m_ccpTimeoutTimer would stay on the thread that constructed
    // ModuleWorker (the GUI thread), while start()/stop() calls on it
    // later happen from this object's own ECU thread - exactly the
    // "Timers cannot be started from another thread" warning.
    m_ccpTimeoutTimer.setParent(this);
    m_ccpTimeoutTimer.setSingleShot(true);
    m_ccpTimeoutTimer.setInterval(kCcpTimeoutMs);
    connect(&m_ccpTimeoutTimer, &QTimer::timeout, this, &ModuleWorker::onCcpTimeout);
}

void ModuleWorker::onRequestConnect()
{
    if (m_connected)
        return;

    setLedState(QStringLiteral("pending"));
    m_pendingOp = PendingOp::Connect;
    sendCcpFrame(Ccp::buildConnect(m_ccpCounter++, static_cast<quint16>(m_canId)));
}

void ModuleWorker::onRequestDisconnect()
{
    if (!m_connected)
        return;

    m_pendingOp = PendingOp::Disconnect;
    sendCcpFrame(Ccp::buildDisconnect(m_ccpCounter++, static_cast<quint16>(m_canId)));
}

void ModuleWorker::onRequestSetMode(const QString &mode)
{
    if (mode == m_mode)
        return;
    m_mode = mode;
    emit modeChanged(m_mode);
}

void ModuleWorker::onRequestSetPower(const QString &state)
{
    // Rule: every transition allowed except going straight back from
    // Kill to On.
    if (state == m_powerState)
        return;

    if (m_powerState == QStringLiteral("Kill") && state == QStringLiteral("On"))
        return; // rejected - ModuleSession simply won't see a change

    m_powerState = state;
    emit powerStateChanged(m_powerState);
}

void ModuleWorker::onFrameReceived(quint32 canId, QByteArray data)
{
    if (canId != m_canId)
        return; // shared bus - filter by CAN ID

    const Ccp::ParsedResponse response = Ccp::parseResponse(data);
    if (!response.valid)
        return;

    // A real response arrived, so whatever timeout was running for
    // the operation this response belongs to is no longer needed.
    clearCcpTimeout();

    const bool ok = (response.status == Ccp::ReturnStatus::Acknowledge);

    switch (m_pendingOp) {
    case PendingOp::Connect:
        if (ok) {
            m_pendingOp = PendingOp::GetSeed;
            sendCcpFrame(Ccp::buildGetSeed(m_ccpCounter++));
        } else {
            setLedState(QStringLiteral("fail"));
            m_pendingOp = PendingOp::None;
        }
        break;

    case PendingOp::GetSeed:
        if (ok) {
            m_pendingOp = PendingOp::Unlock;
            sendCcpFrame(Ccp::buildUnlock(m_ccpCounter++, response.payload));
        } else {
            setLedState(QStringLiteral("fail"));
            m_pendingOp = PendingOp::None;
        }
        break;

    case PendingOp::Unlock:
        m_pendingOp = PendingOp::None;
        m_connected = ok;
        emit connectedChanged(m_connected);
        setLedState(ok ? QStringLiteral("success") : QStringLiteral("fail"));
        break;

    case PendingOp::Disconnect:
        m_pendingOp = PendingOp::None;
        if (ok) {
            m_connected = false;
            emit connectedChanged(m_connected);
            setLedState(QStringLiteral("inactive"));
        }
        break;

    case PendingOp::None:
    default:
        break;
    }
}

void ModuleWorker::onCcpTimeout()
{
    // No response arrived in time for whatever CCP step was pending
    // (previously: this left the module stuck showing "pending"
    // forever with no way out short of restarting the app).
    const PendingOp timedOutOp = m_pendingOp;
    m_pendingOp = PendingOp::None;

    setLedState(QStringLiteral("fail"));

    if (timedOutOp == PendingOp::Disconnect) {
        // Disconnect timing out: stay connected rather than silently
        // flipping to disconnected with no confirmation from the ECU.
        return;
    }
    if (m_connected) {
        m_connected = false;
        emit connectedChanged(m_connected);
    }
}

void ModuleWorker::setLedState(const QString &state)
{
    if (m_ledState == state)
        return;
    m_ledState = state;
    emit ledStateChanged(m_ledState);
}

void ModuleWorker::sendCcpFrame(const QByteArray &frame)
{
    m_bus->sendFrame(m_canId, frame);
    startCcpTimeout();
}

void ModuleWorker::startCcpTimeout()
{
    m_ccpTimeoutTimer.start(); // restarts if already running
}

void ModuleWorker::clearCcpTimeout()
{
    m_ccpTimeoutTimer.stop();
}
