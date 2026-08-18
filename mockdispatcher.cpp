#include "mockdispatcher.h"
#include <QDebug>

MockDispatcher::MockDispatcher(QObject *parent)
    : QObject(parent)
{
}

void MockDispatcher::send(const QByteArray &packet)
{
    qDebug() << "[MockDispatcher] Gönderildi (" << packet.size() << "byte):" << packet.toHex(' ');
}