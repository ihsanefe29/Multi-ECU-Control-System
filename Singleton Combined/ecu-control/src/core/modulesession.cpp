#include "modulesession.h"

ModuleSession::ModuleSession(QString ecuId, int moduleNumber, QObject *parent)
    : QObject(parent)
    , m_ecuId(std::move(ecuId))
    , m_moduleNumber(moduleNumber)
{
}

void ModuleSession::applyConnected(bool connected)
{
    if (m_connected == connected)
        return;
    m_connected = connected;
    emit connectedChanged();
}

void ModuleSession::applyMode(const QString &mode)
{
    if (m_mode == mode)
        return;
    m_mode = mode;
    emit modeChanged();
}

void ModuleSession::applyPowerState(const QString &state)
{
    if (m_powerState == state)
        return;
    m_powerState = state;
    emit powerStateChanged();
}

void ModuleSession::applyLedState(const QString &state)
{
    if (m_ledState == state)
        return;
    m_ledState = state;
    emit ledStateChanged();
}
