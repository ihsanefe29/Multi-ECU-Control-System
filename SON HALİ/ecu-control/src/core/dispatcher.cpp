#include "dispatcher.h"

Dispatcher::Dispatcher(QObject *parent)
    : QObject(parent)
{
    // Shared CAN FD bus - lives on the main/GUI thread.
    m_bus = new CanFdEmulator(this);

    // GUI-thread models - what QML actually binds to.
    m_ecuAModule1 = new ModuleSession(QStringLiteral("A"), 1, this);
    m_ecuAModule2 = new ModuleSession(QStringLiteral("A"), 2, this);
    m_ecuBModule1 = new ModuleSession(QStringLiteral("B"), 1, this);
    m_ecuBModule2 = new ModuleSession(QStringLiteral("B"), 2, this);

    // Worker-thread objects that do the real CCP/CAN FD work. CAN IDs
    // here are placeholders for the skeleton.
    m_ecuAModule1Worker = new ModuleWorker(QStringLiteral("A"), 1, 0x100, m_bus);
    m_ecuAModule2Worker = new ModuleWorker(QStringLiteral("A"), 2, 0x101, m_bus);
    m_ecuBModule1Worker = new ModuleWorker(QStringLiteral("B"), 1, 0x200, m_bus);
    m_ecuBModule2Worker = new ModuleWorker(QStringLiteral("B"), 2, 0x201, m_bus);

    // One thread per ECU: both of ECU A's module workers share ECU
    // A's thread, both of ECU B's share ECU B's thread. Started once,
    // alive for the app's whole lifetime.
    m_ecuAModule1Worker->moveToThread(&m_ecuAThread);
    m_ecuAModule2Worker->moveToThread(&m_ecuAThread);
    m_ecuBModule1Worker->moveToThread(&m_ecuBThread);
    m_ecuBModule2Worker->moveToThread(&m_ecuBThread);

    wire(m_ecuAModule1, m_ecuAModule1Worker);
    wire(m_ecuAModule2, m_ecuAModule2Worker);
    wire(m_ecuBModule1, m_ecuBModule1Worker);
    wire(m_ecuBModule2, m_ecuBModule2Worker);

    m_ecuAThread.start();
    m_ecuBThread.start();
}

Dispatcher::~Dispatcher()
{
    m_ecuAThread.quit();
    m_ecuBThread.quit();
    m_ecuAThread.wait();
    m_ecuBThread.wait();
}

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
}

ModuleSession *Dispatcher::findSession(const QString &ecuId, int moduleNumber) const
{
    if (ecuId == QStringLiteral("A"))
        return moduleNumber == 1 ? m_ecuAModule1 : m_ecuAModule2;
    if (ecuId == QStringLiteral("B"))
        return moduleNumber == 1 ? m_ecuBModule1 : m_ecuBModule2;
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
}

void Dispatcher::setPower(const QString &ecuId, int moduleNumber, const QString &state)
{
    if (ModuleSession *session = findSession(ecuId, moduleNumber))
        session->setPower(state);
}
