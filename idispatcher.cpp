#ifndef IDISPATCHER_H
#define IDISPATCHER_H

#include <QByteArray>

class IDispatcher
{
public:
    virtual ~IDispatcher() = default;
    virtual void send(const QByteArray &packet) = 0;
};

#endif // IDISPATCHER_H
