#pragma once

#include <QByteArray>
#include <QMetaType>
#include <QString>

struct CanFrame
{
    QString channel;
    quint32 id = 0;
    bool isFd = false;
    QByteArray payload;
};

Q_DECLARE_METATYPE(CanFrame)
