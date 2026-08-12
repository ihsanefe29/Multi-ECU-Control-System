#pragma once

#include <QObject>
#include <QThread>
#include <QMap>
#include <QUrl>
#include "modulesession.h"
#include "moduleworker.h"
#include "canfdemulator.h"
#include "signalfileloader.h"
#include "livesignalsource.h"

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
//   - which firmware file (if any) is staged for each module
//   - the Signal Display screen's two data sources (SignalFileLoader,
//     LiveSignalSource) - these are mock/standalone sources (not
//     real per-module CCP measurement streams yet), but are exposed
//     through Dispatcher rather than as separate top-level QML
//     context properties, so everything QML touches goes through one
//     place, consistent with the rest of the architecture.
class Dispatcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ModuleSession* ecuAModule1 READ ecuAModule1 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuAModule2 READ ecuAModule2 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuBModule1 READ ecuBModule1 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuBModule2 READ ecuBModule2 CONSTANT)
    Q_PROPERTY(SignalFileLoader* signalLoader READ signalLoader CONSTANT)
    Q_PROPERTY(LiveSignalSource* liveSource READ liveSource CONSTANT)

public:
    explicit Dispatcher(QObject *parent = nullptr);
    ~Dispatcher() override;

    ModuleSession *ecuAModule1() const { return m_ecuAModule1; }
    ModuleSession *ecuAModule2() const { return m_ecuAModule2; }
    ModuleSession *ecuBModule1() const { return m_ecuBModule1; }
    ModuleSession *ecuBModule2() const { return m_ecuBModule2; }
    SignalFileLoader *signalLoader() const { return m_signalLoader; }
    LiveSignalSource *liveSource() const { return m_liveSource; }

    // ecuId: "A" or "B". moduleNumber: 1 or 2.
    Q_INVOKABLE void connectModule(const QString &ecuId, int moduleNumber);
    Q_INVOKABLE void disconnectModule(const QString &ecuId, int moduleNumber);
    Q_INVOKABLE void setMode(const QString &ecuId, int moduleNumber, const QString &mode);
    Q_INVOKABLE void setPower(const QString &ecuId, int moduleNumber, const QString &state);

    // Firmware staging - held here in the Dispatcher rather than on
    // ModuleSession/ModuleWorker, since (for now) this is just a
    // "which file is queued for this module" record, not something
    // that needs to cross into a worker thread yet. A real upload
    // step (actually sending the file over CCP DNLOAD) would move
    // this into ModuleWorker later.
    //
    // Requires the target module to be connected first - staging a
    // firmware file for a module you're not connected to is refused
    // (returns false, emits firmwareRejected) rather than silently
    // accepted, since "selected but nothing is actually listening"
    // is a state worth catching early.
    Q_INVOKABLE bool selectFirmware(const QString &ecuId, int moduleNumber, const QUrl &fileUrl);
    Q_INVOKABLE QString firmwareFileName(const QString &ecuId, int moduleNumber) const;
    Q_INVOKABLE qint64 firmwareSizeBytes(const QString &ecuId, int moduleNumber) const;
    Q_INVOKABLE bool hasFirmware(const QString &ecuId, int moduleNumber) const;

signals:
    void firmwareChanged(const QString &ecuId, int moduleNumber);
    void firmwareRejected(const QString &ecuId, int moduleNumber, const QString &reason);

private:
    ModuleSession *findSession(const QString &ecuId, int moduleNumber) const;
    void wire(ModuleSession *session, ModuleWorker *worker);
    static QString firmwareKey(const QString &ecuId, int moduleNumber);

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

    SignalFileLoader *m_signalLoader;
    LiveSignalSource *m_liveSource;

    struct FirmwareInfo {
        QString fileName;
        qint64 sizeBytes = 0;
    };
    QMap<QString, FirmwareInfo> m_firmwareByModule; // key: firmwareKey(ecuId, moduleNumber)
};
