#ifndef ECUCONTROLLER_H
#define ECUCONTROLLER_H

#include <QObject>

#include "../message/UIMessage.h"

class ECUController : public QObject
{
    Q_OBJECT

public:
    explicit ECUController(QObject *parent = nullptr);

signals:

    void safetyStateChanged(bool safe);

public slots:

    void handleMessage(const UIMessage &message);

private:

    bool isSafe() const;

    bool m_connected = false;
    bool m_powered = false;
    bool m_killed = false;
    bool m_switchOn = false;

    ModeType m_mode = ModeType::Normal;
};

#endif // ECUCONTROLLER_H
