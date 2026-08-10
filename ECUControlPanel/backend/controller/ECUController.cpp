#include "ECUController.h"

#include <QDebug>

ECUController::ECUController(QObject *parent)
    : QObject(parent)
{

}

bool ECUController::isSafe() const
{
    bool safe =
        m_connected
        && m_switchOn
        && !m_killed;

    qDebug() << "SAFE CHECK:"
             << "connected =" << m_connected
             << "switchOn =" << m_switchOn
             << "powered =" << m_powered
             << "killed =" << m_killed
             << "=> safe =" << safe;

    return safe;
}

void ECUController::handleMessage(const UIMessage &message)
{
    QString ecuName;

    switch (message.ecu)
    {
    case ECUType::A1:
        ecuName = "ECU A1";
        break;

    case ECUType::A2:
        ecuName = "ECU A2";
        break;

    case ECUType::B1:
        ecuName = "ECU B1";
        break;

    case ECUType::B2:
        ecuName = "ECU B2";
        break;
    }
    switch (message.command)
    {
    case CommandType::Connect:

        if (m_killed)
        {
            qDebug() << "Controller : Cannot connect. ECU is in KILL state.";
            break;
        }

        if (m_connected)
        {
            qDebug() << "Controller : ECU is already connected.";
            break;
        }

        m_connected = true;

        qDebug() << "Controller : ECU Connected.";

        break;

    case CommandType::Disconnect:

        if (!m_connected)
        {
            qDebug() << "Controller : Already disconnected.";
            break;
        }

        m_connected = false;
        m_powered = false;
        m_killed = false;
        m_switchOn = false;

        qDebug() << "Controller : ECU Disconnected.";

        break;

    case CommandType::Power:

        if (!m_connected)
        {
            qDebug() << "Controller : Cannot power ECU. Not connected.";
            break;
        }

        if (m_killed)
        {
            qDebug() << "Controller : Cannot power ECU. Emergency Stop is active.";
            break;
        }

        m_powered = !m_powered;

        qDebug() << "Controller : Power"
                 << (m_powered ? "Enabled." : "Disabled.");

        break;

    case CommandType::SwitchOn:

        if (!m_connected)
        {
            qDebug() << "Controller : Cannot switch ON. ECU is not connected.";
            break;
        }

        if (m_killed)
        {
            qDebug() << "Controller : Cannot switch ON. Emergency Stop is active.";
            break;
        }

        m_switchOn = true;

        qDebug() << "Controller : Switch ON.";

        break;

    case CommandType::SwitchOff:

        m_switchOn = false;
        m_powered = false;

        qDebug() << "Controller : Switch OFF.";

        break;

    case CommandType::Kill:

        m_killed = true;
        m_connected = false;
        m_powered = false;
        m_switchOn = false;

        qDebug() << "Controller : Emergency Stop Activated.";

        break;

    case CommandType::ChangeMode:
    {
        m_mode = message.mode;

        QString modeName;

        switch (m_mode)
        {
        case ModeType::Normal:
            modeName = "Normal";
            break;

        case ModeType::Test:
            modeName = "Test";
            break;

        case ModeType::Maintenance:
            modeName = "Maintenance";
            break;
        }

        qDebug() << "Controller :" << ecuName
                 << "Mode changed to"
                 << modeName;

        break;
    }

    }

    emit safetyStateChanged(isSafe());
}