#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QObject>

#include "../message/UIMessage.h"
#include "../manager/ECUManager.h"

class Dispatcher : public QObject
{
    Q_OBJECT

public:
    explicit Dispatcher(QObject *parent = nullptr);

    void setManager(ECUManager *manager);

public slots:

    void dispatch(const UIMessage &message);

private:

    ECUManager *m_manager = nullptr;
};

#endif // DISPATCHER_H