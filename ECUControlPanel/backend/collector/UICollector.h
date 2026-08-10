#ifndef UICOLLECTOR_H
#define UICOLLECTOR_H

#include <QObject>

#include "../message/UIMessage.h"

class UICollector : public QObject
{
    Q_OBJECT

public:
    explicit UICollector(QObject *parent = nullptr);

    Q_INVOKABLE void connectRequest(int ecu);
    Q_INVOKABLE void disconnectRequest(int ecu);
    Q_INVOKABLE void powerRequest(int ecu);
    Q_INVOKABLE void killRequest(int ecu);
    Q_INVOKABLE void modeRequest(int ecu, int mode);
    Q_INVOKABLE void switchOnRequest(int ecu);
    Q_INVOKABLE void switchOffRequest(int ecu);

signals:
    void messageCollected(const UIMessage &message);

private:
    ECUType toECUType(int ecu);
};

#endif // UICOLLECTOR_H