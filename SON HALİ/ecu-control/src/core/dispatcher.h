#pragma once

#include <QObject>
#include <QThread>
#include "modulesession.h"
#include "moduleworker.h"
#include "canfdemulator.h"

// The single object QML talks to. QML calls Q_INVOKABLE methods here
// with only semantic intent (which ECU, which module, what action) -
// it never touches CCP frames, CAN IDs, or threads directly.
//
// Dispatcher owns:
//   - 4 ModuleSession objects (GUI thread, QML-bound) - the only
//     things QML ever sees
//   - 4 ModuleWorker objects (2 on ECU A's thread, 2 on ECU B's
//     thread) - do the actual CCP/CAN FD work
//   - the wiring between each Session/Worker pair
//   - one QThread per ECU, alive for the whole app lifetime
//   - the single shared CanFdEmulator
class Dispatcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ModuleSession* ecuAModule1 READ ecuAModule1 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuAModule2 READ ecuAModule2 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuBModule1 READ ecuBModule1 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuBModule2 READ ecuBModule2 CONSTANT)

public:
    explicit Dispatcher(QObject *parent = nullptr);
    ~Dispatcher() override;

    ModuleSession *ecuAModule1() const { return m_ecuAModule1; }
    ModuleSession *ecuAModule2() const { return m_ecuAModule2; }
    ModuleSession *ecuBModule1() const { return m_ecuBModule1; }
    ModuleSession *ecuBModule2() const { return m_ecuBModule2; }

    // ecuId: "A" or "B". moduleNumber: 1 or 2.
    Q_INVOKABLE void connectModule(const QString &ecuId, int moduleNumber);
    Q_INVOKABLE void disconnectModule(const QString &ecuId, int moduleNumber);
    Q_INVOKABLE void setMode(const QString &ecuId, int moduleNumber, const QString &mode);
    Q_INVOKABLE void setPower(const QString &ecuId, int moduleNumber, const QString &state);

private:
    ModuleSession *findSession(const QString &ecuId, int moduleNumber) const;
    void wire(ModuleSession *session, ModuleWorker *worker);

    QThread m_ecuAThread;
    QThread m_ecuBThread;
    CanFdEmulator *m_bus;

    ModuleSession *m_ecuAModule1;
    ModuleSession *m_ecuAModule2;
    ModuleSession *m_ecuBModule1;
    ModuleSession *m_ecuBModule2;

    ModuleWorker *m_ecuAModule1Worker;
    ModuleWorker *m_ecuAModule2Worker;
    ModuleWorker *m_ecuBModule1Worker;
    ModuleWorker *m_ecuBModule2Worker;
};
