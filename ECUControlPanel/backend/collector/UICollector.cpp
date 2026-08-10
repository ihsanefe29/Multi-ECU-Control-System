#include "UICollector.h"

#include <QDebug>

UICollector::UICollector(QObject *parent)
    : QObject(parent)
{
}

ECUType UICollector::toECUType(int ecu)
{
    switch (ecu)
    {
    case 0: return ECUType::A1;
    case 1: return ECUType::A2;
    case 2: return ECUType::B1;
    case 3: return ECUType::B2;
    default: return ECUType::A1;
    }
}

void UICollector::connectRequest(int ecu)
{
    UIMessage message;

    message.ecu = toECUType(ecu);
    message.command = CommandType::Connect;
    message.mode = ModeType::Normal;

    qDebug() << "Collector : Connect request";

    emit messageCollected(message);
}

void UICollector::disconnectRequest(int ecu)
{
    UIMessage message;

    message.ecu = toECUType(ecu);
    message.command = CommandType::Disconnect;
    message.mode = ModeType::Normal;

    qDebug() << "Collector : Disconnect request";

    emit messageCollected(message);
}

void UICollector::powerRequest(int ecu)
{
    UIMessage message;

    message.ecu = toECUType(ecu);
    message.command = CommandType::Power;
    message.mode = ModeType::Normal;

    qDebug() << "Collector : Power request";

    emit messageCollected(message);
}

void UICollector::killRequest(int ecu)
{
    UIMessage message;

    message.ecu = toECUType(ecu);
    message.command = CommandType::Kill;
    message.mode = ModeType::Normal;

    qDebug() << "Collector : Kill request";

    emit messageCollected(message);
}

void UICollector::modeRequest(int ecu, int mode)
{
    UIMessage message;

    message.ecu = toECUType(ecu);
    message.command = CommandType::ChangeMode;

    switch (mode)
    {
    case 0:
        message.mode = ModeType::Normal;
        break;

    case 1:
        message.mode = ModeType::Test;
        break;

    case 2:
        message.mode = ModeType::Maintenance;
        break;

    default:
        message.mode = ModeType::Normal;
        break;
    }

    qDebug() << "Collector : Mode request";

    emit messageCollected(message);
}

void UICollector::switchOnRequest(int ecu)
{
    UIMessage message;

    message.ecu = toECUType(ecu);
    message.command = CommandType::SwitchOn;
    message.mode = ModeType::Normal;

    qDebug() << "Collector : Switch ON request";

    emit messageCollected(message);
}

void UICollector::switchOffRequest(int ecu)
{
    UIMessage message;

    message.ecu = toECUType(ecu);
    message.command = CommandType::SwitchOff;
    message.mode = ModeType::Normal;

    qDebug() << "Collector : Switch OFF request";

    emit messageCollected(message);
}