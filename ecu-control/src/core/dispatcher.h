#pragma once

#include <QObject>
#include <QThread>
#include <QMap>
#include <QUrl>
#include <QStringList>
#include <QTimer>
#include "modulesession.h"
#include "moduleworker.h"
#include "canfdemulator.h"
#include "signalfileloader.h"
#include "livesignalsource.h"
#include "rawsignalloader.h"
#include "datasimulator.h"
#include "parametermodel.h"
#include "rawdataparser.h"
#include "excelparser.h"

// The single object QML talks to. QML calls Q_INVOKABLE methods here
// with only semantic intent (which ECU, which module, what action) -
// it never touches CCP frames, CAN IDs, or threads directly.
//
// Dispatcher owns:
//   - 4 ModuleSession objects (GUI thread, QML-bound)
//   - 4 ModuleWorker objects (2 on ECU A's thread, 2 on ECU B's thread)
//   - the wiring between each Session/Worker pair
//   - one QThread per ECU, alive for the whole app lifetime
//   - the single shared CanFdEmulator
//   - firmware staging per module
//   - SignalFileLoader, LiveSignalSource, RawSignalLoader (data display)
//   - DataSimulator  — sahte ECU paketi üretici (aybuke branch)
//   - ParameterModel — N1/EGT/FuelFlow/BleedValve/Vibration yönetimi (esra branch)
//   - Log sistemi + Fail-Safe + Sıralı başlatma (EceGulYuksel branch)
class Dispatcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ModuleSession* ecuAModule1 READ ecuAModule1 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuAModule2 READ ecuAModule2 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuBModule1 READ ecuBModule1 CONSTANT)
    Q_PROPERTY(ModuleSession* ecuBModule2 READ ecuBModule2 CONSTANT)
    Q_PROPERTY(SignalFileLoader* signalLoader READ signalLoader CONSTANT)
    Q_PROPERTY(LiveSignalSource* liveSource READ liveSource CONSTANT)
    Q_PROPERTY(RawSignalLoader* rawSignalLoader READ rawSignalLoader CONSTANT)
    Q_PROPERTY(DataSimulator*   dataSimulator   READ dataSimulator   CONSTANT)
    Q_PROPERTY(ParameterModel*  parameterModel  READ parameterModel  CONSTANT)
    Q_PROPERTY(RawDataParser*   rawDataParser   READ rawDataParser   CONSTANT)
    Q_PROPERTY(ExcelParser*     excelParser     READ excelParser     CONSTANT)

    // ── Log / Fail-Safe (EceGulYuksel branch) ──────────────────────────────
    Q_PROPERTY(QStringList logMessages READ logMessages NOTIFY logMessagesChanged)
    Q_PROPERTY(bool failSafeActive READ failSafeActive NOTIFY failSafeChanged)
    Q_PROPERTY(bool startupInProgress READ startupInProgress NOTIFY startupChanged)

public:
    explicit Dispatcher(QObject *parent = nullptr);
    ~Dispatcher() override;

    ModuleSession *ecuAModule1() const { return m_ecuAModule1; }
    ModuleSession *ecuAModule2() const { return m_ecuAModule2; }
    ModuleSession *ecuBModule1() const { return m_ecuBModule1; }
    ModuleSession *ecuBModule2() const { return m_ecuBModule2; }
    SignalFileLoader *signalLoader() const { return m_signalLoader; }
    LiveSignalSource *liveSource() const { return m_liveSource; }
    RawSignalLoader *rawSignalLoader() const { return m_rawSignalLoader; }
    DataSimulator   *dataSimulator() const { return m_dataSimulator; }
    ParameterModel  *parameterModel() const { return m_parameterModel; }
    RawDataParser   *rawDataParser()  const { return m_rawDataParser; }
    ExcelParser     *excelParser()    const { return m_excelParser; }

    QStringList logMessages() const { return m_logMessages; }
    bool failSafeActive() const     { return m_failSafeActive; }
    bool startupInProgress() const  { return m_startupInProgress; }

    // ecuId: "A" or "B". moduleNumber: 1 or 2.
    Q_INVOKABLE void connectModule(const QString &ecuId, int moduleNumber);
    Q_INVOKABLE void disconnectModule(const QString &ecuId, int moduleNumber);
    Q_INVOKABLE void setMode(const QString &ecuId, int moduleNumber, const QString &mode);
    Q_INVOKABLE void setPower(const QString &ecuId, int moduleNumber, const QString &state);

    // Firmware staging
    Q_INVOKABLE bool selectFirmware(const QString &ecuId, int moduleNumber, const QUrl &fileUrl);
    Q_INVOKABLE QString firmwareFileName(const QString &ecuId, int moduleNumber) const;
    Q_INVOKABLE qint64 firmwareSizeBytes(const QString &ecuId, int moduleNumber) const;
    Q_INVOKABLE bool hasFirmware(const QString &ecuId, int moduleNumber) const;

    // ── Log / Fail-Safe / Startup (EceGulYuksel branch) ───────────────────
    Q_INVOKABLE void triggerFailSafe(const QString &reason);
    Q_INVOKABLE void clearFailSafe();
    Q_INVOKABLE void emergencyStop();
    Q_INVOKABLE void startupSequence();
    // Excel parametre dosyasını yükle ve RawDataParser'a ilet
    Q_INVOKABLE bool loadExcelParameters(const QUrl &fileUrl);

    // QML'den log ekleme
    Q_INVOKABLE void addLog(const QString &line);

signals:
    void firmwareChanged(const QString &ecuId, int moduleNumber);
    void firmwareRejected(const QString &ecuId, int moduleNumber, const QString &reason);

    // Log / Fail-Safe sinyalleri
    void logMessagesChanged();
    void failSafeChanged();
    void startupChanged();

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
    RawSignalLoader  *m_rawSignalLoader;
    DataSimulator    *m_dataSimulator;
    ParameterModel   *m_parameterModel;
    RawDataParser    *m_rawDataParser;
    ExcelParser      *m_excelParser;

    struct FirmwareInfo {
        QString fileName;
        qint64 sizeBytes = 0;
    };
    QMap<QString, FirmwareInfo> m_firmwareByModule;

    // ── Log / Fail-Safe (EceGulYuksel branch) ──────────────────────────────
    QStringList m_logMessages;
    bool m_failSafeActive    = false;
    bool m_startupInProgress = false;
    int  m_startupIndex      = -1;
    QTimer m_startupTimer;

    static constexpr int kMaxLogLines = 100;

    void startNextModule();
    void checkModuleHealth();
};
