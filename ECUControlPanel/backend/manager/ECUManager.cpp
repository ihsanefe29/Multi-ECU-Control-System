#include "ECUManager.h"

#include <QDebug>

ECUManager::ECUManager(QObject *parent)
    : QObject(parent)
{
    connect(
        &m_controller,
        &ECUController::safetyStateChanged,
        this,
        &ECUManager::safetyStateChanged
        );
}

void ECUManager::handleMessage(const UIMessage &message)
{
    qDebug() << "ECUManager forwarding message...";

    m_controller.handleMessage(message);
}