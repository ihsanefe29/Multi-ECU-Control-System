#ifndef MOCKDISPATCHER_H
#define MOCKDISPATCHER_H

#include "idispatcher.h"
#include <QObject>

class MockDispatcher : public QObject, public IDispatcher
{
    Q_OBJECT

public:
    explicit MockDispatcher(QObject *parent = nullptr);

public slots:
    void send(const QByteArray &packet) override;   // artık slot
};

#endif // MOCKDISPATCHER_H