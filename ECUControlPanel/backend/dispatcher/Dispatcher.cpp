#include "Dispatcher.h"

#include <QDebug>

Dispatcher::Dispatcher(QObject *parent)
    : QObject(parent)
{

}

void Dispatcher::dispatch(const UIMessage &message)
{
    qDebug() << "Dispatcher forwarding message...";

    if (!m_manager)
    {
        qDebug() << "Dispatcher: Manager not connected!";
        return;
    }

    m_manager->handleMessage(message);
}

void Dispatcher::setManager(ECUManager *manager)
{
    m_manager = manager;
}