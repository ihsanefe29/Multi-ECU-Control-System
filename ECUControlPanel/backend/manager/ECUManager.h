#ifndef ECUMANAGER_H
#define ECUMANAGER_H

#include <QObject>

#include "../controller/ECUController.h"
#include "../message/UIMessage.h"

class ECUManager : public QObject
{
    Q_OBJECT

public:
    explicit ECUManager(QObject *parent = nullptr);

signals:
    void safetyStateChanged(bool safe);

public slots:
    void handleMessage(const UIMessage &message);

private:
    ECUController m_controller;
};

#endif // ECUMANAGER_H
