#include "mockdispatcher.h"
#include <QDebug>

MockDispatcher::MockDispatcher(QObject *parent)
    : QObject(parent)
{
}

void MockDispatcher::send(const QByteArray &packet)
{
    qDebug() << "[MockDispatcher] G├Ânderildi (" << packet.size() << "byte):" << packet.toHex(' ');
}
