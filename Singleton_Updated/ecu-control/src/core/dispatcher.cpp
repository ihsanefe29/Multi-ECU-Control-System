#include "dispatcher.h"
#include <QFileInfo>
#include <QElapsedTimer>
#include <QHash>


namespace {
// Small helper to build one Session+Worker pair, reducing the
// previous 4x copy-pasted construction down to one loop below.
struct ModuleSpec {
    QString ecuId;
    int moduleNumber;
};
}

Dispatcher::Dispatcher(CommunicationCore *core, QObject *parent)
    : QObject(parent), m_core(core)
{
     m_telemetryClock.start();


connect(m_core,
        &CommunicationCore::ecuDiscovered,
        this,
        &Dispatcher::onEcuDiscovered);

connect(m_core,
        &CommunicationCore::ecuOnlineChanged,
        this,
        &Dispatcher::onEcuOnlineChanged);

connect(m_core,
        &CommunicationCore::ecuStatus,
        this,
        &Dispatcher::onEcuStatus);

connect(m_core,
        &CommunicationCore::ecuHeartbeat,
        this,
        &Dispatcher::onEcuHeartbeat);

connect(m_core,
        &CommunicationCore::telemetryReceived,
        this,
        &Dispatcher::onTelemetryReceived);

connect(m_core,
        &CommunicationCore::errorOccurred,
        this,
        &Dispatcher::communicationError);
    
    // Shared CAN FD bus - lives on the main/GUI thread.
    //m_bus = new CanFdEmulator(this);

    m_signalLoader = new SignalFileLoader(this);
    m_liveSource = new LiveSignalSource(this);
    m_rawSignalLoader = new RawSignalLoader(this);

    // CAN IDs here are placeholders for the skeleton.
    const ModuleSpec specs[4] = {
        { QStringLiteral("A"), 1 },
        { QStringLiteral("B"), 1 },
        { QStringLiteral("C"), 1 },
        { QStringLiteral("D"), 1 },
    };

    ModuleSession *sessions[4];
    //ModuleWorker *workers[4];
    for (int i = 0; i < 4; ++i) {
        sessions[i] = new ModuleSession(specs[i].ecuId, specs[i].moduleNumber, this);
        //workers[i] = new ModuleWorker(specs[i].ecuId, specs[i].moduleNumber);
       // wire(sessions[i], workers[i]);
    }

    m_ecuAModule1 = sessions[0];
    m_ecuAModule2 = sessions[1];
    m_ecuBModule1 = sessions[2];

    m_ecuBModule2 = sessions[3];

    /*m_ecuAModule1Worker = workers[0];
    m_ecuAModule2Worker = workers[1];
    m_ecuBModule1Worker = workers[2];
    m_ecuBModule2Worker = workers[3];*/

    /*
    // One thread per ECU: both of ECU A's module workers share ECU
    // A's thread, both of ECU B's share ECU B's thread. Started once,
    // alive for the app's whole lifetime.
    m_ecuAModule1Worker->moveToThread(&m_ecuAThread);
    m_ecuAModule2Worker->moveToThread(&m_ecuAThread);
    m_ecuBModule1Worker->moveToThread(&m_ecuBThread);
    m_ecuBModule2Worker->moveToThread(&m_ecuBThread);

    m_ecuAThread.start();
    m_ecuBThread.start();*/
}
/*
Dispatcher::~Dispatcher()
{
    //m_ecuAThread.quit();
    //m_ecuBThread.quit();
    //m_ecuAThread.wait();
   // m_ecuBThread.wait();
}*/

/*
void Dispatcher::wire(ModuleSession *session, ModuleWorker *worker)
{
    // Session -> Worker: crosses from the GUI thread to the worker's
    // ECU thread. Default AutoConnection resolves to Queued
    // automatically since sender/receiver live on different threads.


    connect(session, &ModuleSession::requestConnect, worker, &ModuleWorker::onRequestConnect);
    connect(session, &ModuleSession::requestDisconnect, worker, &ModuleWorker::onRequestDisconnect);
    connect(session, &ModuleSession::requestSetMode, worker, &ModuleWorker::onRequestSetMode);
    connect(session, &ModuleSession::requestSetPower, worker, &ModuleWorker::onRequestSetPower);

    // Worker -> Session: crosses back to the GUI thread, likewise
    // auto-queued.
    connect(worker, &ModuleWorker::connectedChanged, session, &ModuleSession::applyConnected);
    connect(worker, &ModuleWorker::modeChanged, session, &ModuleSession::applyMode);
    connect(worker, &ModuleWorker::powerStateChanged, session, &ModuleSession::applyPowerState);
    connect(worker, &ModuleWorker::ledStateChanged, session, &ModuleSession::applyLedState);
}*/

ModuleSession *Dispatcher::findSession(const QString &ecuId, int moduleNumber) const
{
    Q_UNUSED(moduleNumber);

    if (ecuId.compare(QStringLiteral("A"), Qt::CaseInsensitive) == 0)
        return m_ecuAModule1;

    if (ecuId.compare(QStringLiteral("B"), Qt::CaseInsensitive) == 0)
        return m_ecuAModule2;

    if (ecuId.compare(QStringLiteral("C"), Qt::CaseInsensitive) == 0)
        return m_ecuBModule1;

    if (ecuId.compare(QStringLiteral("D"), Qt::CaseInsensitive) == 0)
        return m_ecuBModule2;

    return nullptr;
}

void Dispatcher::connectModule(const QString &ecuId, int moduleNumber)
{
    if (ModuleSession *session = findSession(ecuId, moduleNumber))
        session->connectModule();
}

void Dispatcher::disconnectModule(const QString &ecuId, int moduleNumber)
{
    if (ModuleSession *session = findSession(ecuId, moduleNumber))
        session->disconnectModule();
}

void Dispatcher::setMode(const QString &ecuId, int moduleNumber, const QString &mode)
{
    if (ModuleSession *session = findSession(ecuId, moduleNumber))
        session->setMode(mode);

    if (mode == QStringLiteral("DataView"))
        m_selectedEcu = ecuId.toUpper();
}

void Dispatcher::setPower(const QString &ecuId, int moduleNumber, const QString &state)
{
    /*
    if (ModuleSession *session = findSession(ecuId, moduleNumber))
        session->setPower(state);
        */
    Q_UNUSED(moduleNumber);

    if (ecuId.isEmpty())
        return;

    const char ecu = ecuId.at(0).toUpper().toLatin1();

    if (state.compare("On", Qt::CaseInsensitive) == 0)
        m_core->powerOn(ecu);

    else if (state.compare("Off", Qt::CaseInsensitive) == 0)
        m_core->powerOff(ecu);
}

QString Dispatcher::firmwareKey(const QString &ecuId, int moduleNumber)
{
    return ecuId + QString::number(moduleNumber);
}

bool Dispatcher::selectFirmware(const QString &ecuId, int moduleNumber, const QUrl &fileUrl)
{
    ModuleSession *session = findSession(ecuId, moduleNumber);
    if (!session) {
        emit firmwareRejected(ecuId, moduleNumber, QStringLiteral("Bilinmeyen modül."));
        return false;
    }

    // Staging a firmware file for a module that isn't connected means
    // "queued for nothing is listening" - refuse rather than silently
    // accept, so this can't be mistaken for a real pending upload.
    if (!session->connected()) {
        emit firmwareRejected(ecuId, moduleNumber,
            QStringLiteral("Modül bağlı değil - önce \"Bağlan\" ile ECU'ya bağlanmalısınız."));
        return false;
    }

    const QString localPath = fileUrl.isLocalFile() ? fileUrl.toLocalFile() : fileUrl.toString();
    QFileInfo info(localPath);

    FirmwareInfo firmware;
    firmware.fileName = info.fileName();
    firmware.sizeBytes = info.size();

    m_firmwareByModule[firmwareKey(ecuId, moduleNumber)] = firmware;
    emit firmwareChanged(ecuId, moduleNumber);
    return true;
}

QString Dispatcher::firmwareFileName(const QString &ecuId, int moduleNumber) const
{
    return m_firmwareByModule.value(firmwareKey(ecuId, moduleNumber)).fileName;
}

qint64 Dispatcher::firmwareSizeBytes(const QString &ecuId, int moduleNumber) const
{
    return m_firmwareByModule.value(firmwareKey(ecuId, moduleNumber)).sizeBytes;
}

bool Dispatcher::hasFirmware(const QString &ecuId, int moduleNumber) const
{
    return m_firmwareByModule.contains(firmwareKey(ecuId, moduleNumber));
}
void Dispatcher::onEcuDiscovered(char ecu,const QString &channel,quint32 commandId)
{
    qInfo() << "ECU"
            << ecu
            << "discovered on"
            << channel
            << "command ID"
            << Qt::hex
            << commandId
            << Qt::dec;
}
void Dispatcher::onEcuOnlineChanged(char ecu,bool online)
{
    const QString ecuId = QString(QChar::fromLatin1(ecu).toUpper());

    for (int module = 1; module <= 2; ++module) {
        if (ModuleSession *session = findSession(ecuId, module)) {
            session->applyConnected(online);

            session->applyLedState(online? QStringLiteral("success"): QStringLiteral("inactive"));
        }
    }
}
void Dispatcher::onEcuStatus(char ecu,quint8 state,quint8 result)
{
    Q_UNUSED(result);

    QString stateText;

    if (state == 0)
        stateText = QStringLiteral("Off");
    else if (state == 1)
        stateText = QStringLiteral("On");
    else if (state == 2)
        stateText = QStringLiteral("Kill");
    else
        return;

    const QString ecuId = QString(QChar::fromLatin1(ecu).toUpper());

    for (int module = 1; module <= 2; ++module) {
        if (ModuleSession *session = findSession(ecuId, module))
            session->applyPowerState(stateText);
    }
}
void Dispatcher::onEcuHeartbeat(char ecu,quint8 counter,quint8 state)
{
    //in order to block unused parameter warnings
    Q_UNUSED(ecu);
    Q_UNUSED(counter);
    Q_UNUSED(state);
}
/*
void Dispatcher::onTelemetryReceived(EcuTelemetry telemetry)
{
    //mock for now
    qDebug() << "Telemetry received from ECU"
             << telemetry.ecu;
}
*/
void Dispatcher::onTelemetryReceived(EcuTelemetry telemetry)
{
    const QString incomingEcu = QString(QChar::fromLatin1(telemetry.ecu).toUpper());

    qInfo() << "incoming:" << incomingEcu
            << "selected:" << m_selectedEcu;

    if (m_selectedEcu.isEmpty())
        return;

    if (incomingEcu != m_selectedEcu)
        return;

    const qint64 nowMs = m_telemetryClock.elapsed();

    const char ecu = telemetry.ecu;

    if (m_lastTelemetryMs.contains(ecu)) {
        const qint64 deltaMs = nowMs - m_lastTelemetryMs.value(ecu);

        if (deltaMs > 0) {
            const double hz = 1000.0 / static_cast<double>(deltaMs); // f = 1/s || 1000/ms

            qInfo() << "SELECTED ECU"
                    << incomingEcu
                    << "HZ:"
                    << hz;

            m_liveSource->pushHz(hz);
        }
    }

    m_lastTelemetryMs[ecu] = nowMs;
}