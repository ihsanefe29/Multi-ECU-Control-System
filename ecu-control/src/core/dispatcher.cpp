#include "dispatcher.h"
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

namespace {
struct ModuleSpec {
    QString ecuId;
    int moduleNumber;
    quint32 canId;
};
}

Dispatcher::Dispatcher(QObject *parent)
    : QObject(parent)
{
    // Shared CAN FD bus
    m_bus = new CanFdEmulator(this);

    m_signalLoader    = new SignalFileLoader(this);
    m_liveSource      = new LiveSignalSource(this);
    m_rawSignalLoader = new RawSignalLoader(this);
    m_dataSimulator   = new DataSimulator(this);   // aybuke branch
    m_parameterModel  = new ParameterModel(this);  // esra branch
    m_rawDataParser   = new RawDataParser(this);   // aybuke branch
    m_excelParser     = new ExcelParser(this);     // aybuke branch

    // DataSimulator → ParameterModel canlı bağlantısı
    // Her 500ms'de üretilen 16-byte paketi ayrıştırıp gauge'ları günceller.
    // Paket yapısı: Header(2) N1(2) EGT(2) FuelFlow(2) BleedValve(2) Vibration(4) Checksum(2)
    connect(m_dataSimulator, &DataSimulator::dataReady,
            this, [this](const QByteArray &pkt) {
        if (pkt.size() < 14) return;

        auto readInt16LE = [&](int offset) -> qint16 {
            return static_cast<qint16>(
                static_cast<quint8>(pkt[offset]) |
                (static_cast<quint8>(pkt[offset + 1]) << 8));
        };
        auto readFloatLE = [&](int offset) -> float {
            quint32 raw =
                static_cast<quint8>(pkt[offset]) |
                (static_cast<quint8>(pkt[offset+1]) <<  8) |
                (static_cast<quint8>(pkt[offset+2]) << 16) |
                (static_cast<quint8>(pkt[offset+3]) << 24);
            float f; memcpy(&f, &raw, sizeof(float));
            return f;
        };

        m_parameterModel->setValue("N1",         readInt16LE(2));
        m_parameterModel->setValue("EGT",         readInt16LE(4));
        m_parameterModel->setValue("Fuel Flow",   readInt16LE(6));
        m_parameterModel->setValue("Bleed Valve", readInt16LE(8));
        m_parameterModel->setValue("Vibration",   static_cast<double>(readFloatLE(10)));
    });

    const ModuleSpec specs[4] = {
        { QStringLiteral("A"), 1, 0x100 },
        { QStringLiteral("A"), 2, 0x101 },
        { QStringLiteral("B"), 1, 0x200 },
        { QStringLiteral("B"), 2, 0x201 },
    };

    ModuleSession *sessions[4];
    ModuleWorker  *workers[4];
    for (int i = 0; i < 4; ++i) {
        sessions[i] = new ModuleSession(specs[i].ecuId, specs[i].moduleNumber, this);
        workers[i]  = new ModuleWorker(specs[i].ecuId, specs[i].moduleNumber, specs[i].canId, m_bus);
        wire(sessions[i], workers[i]);
    }

    m_ecuAModule1 = sessions[0];
    m_ecuAModule2 = sessions[1];
    m_ecuBModule1 = sessions[2];
    m_ecuBModule2 = sessions[3];

    m_ecuAModule1Worker = workers[0];
    m_ecuAModule2Worker = workers[1];
    m_ecuBModule1Worker = workers[2];
    m_ecuBModule2Worker = workers[3];

    m_ecuAModule1Worker->moveToThread(&m_ecuAThread);
    m_ecuAModule2Worker->moveToThread(&m_ecuAThread);
    m_ecuBModule1Worker->moveToThread(&m_ecuBThread);
    m_ecuBModule2Worker->moveToThread(&m_ecuBThread);

    m_ecuAThread.start();
    m_ecuBThread.start();

    // ── Startup timer (EceGulYuksel branch) ────────────────────────────────
    // Her modülü sırayla devreye almak için timer'ı bağla.
    connect(&m_startupTimer, &QTimer::timeout, this, &Dispatcher::startNextModule);

    addLog(QStringLiteral("[BİLGİ] ") + QDateTime::currentDateTime().toString("hh:mm:ss")
           + QStringLiteral("  ECU Kontrol sistemi başlatıldı."));
}

Dispatcher::~Dispatcher()
{
    m_startupTimer.stop();
    m_ecuAThread.quit();
    m_ecuBThread.quit();
    m_ecuAThread.wait();
    m_ecuBThread.wait();
}

// ── Wire ──────────────────────────────────────────────────────────────────

void Dispatcher::wire(ModuleSession *session, ModuleWorker *worker)
{
    connect(session, &ModuleSession::requestConnect,    worker, &ModuleWorker::onRequestConnect);
    connect(session, &ModuleSession::requestDisconnect, worker, &ModuleWorker::onRequestDisconnect);
    connect(session, &ModuleSession::requestSetMode,    worker, &ModuleWorker::onRequestSetMode);
    connect(session, &ModuleSession::requestSetPower,   worker, &ModuleWorker::onRequestSetPower);

    connect(worker, &ModuleWorker::connectedChanged,   session, &ModuleSession::applyConnected);
    connect(worker, &ModuleWorker::modeChanged,        session, &ModuleSession::applyMode);
    connect(worker, &ModuleWorker::powerStateChanged,  session, &ModuleSession::applyPowerState);
    connect(worker, &ModuleWorker::ledStateChanged,    session, &ModuleSession::applyLedState);
}

// ── Session lookup ────────────────────────────────────────────────────────

ModuleSession *Dispatcher::findSession(const QString &ecuId, int moduleNumber) const
{
    if (ecuId == QStringLiteral("A"))
        return moduleNumber == 1 ? m_ecuAModule1 : m_ecuAModule2;
    if (ecuId == QStringLiteral("B"))
        return moduleNumber == 1 ? m_ecuBModule1 : m_ecuBModule2;
    return nullptr;
}

// ── Module commands ───────────────────────────────────────────────────────

void Dispatcher::connectModule(const QString &ecuId, int moduleNumber)
{
    if (m_failSafeActive) {
        addLog(QStringLiteral("[UYARI] Fail-safe aktif, bağlantı reddedildi: ECU ")
               + ecuId + " Modül " + QString::number(moduleNumber));
        return;
    }
    if (ModuleSession *s = findSession(ecuId, moduleNumber)) {
        addLog(QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  ECU " + ecuId + " Modül " + QString::number(moduleNumber) + "  ->  bağlan");
        s->connectModule();
    }
}

void Dispatcher::disconnectModule(const QString &ecuId, int moduleNumber)
{
    if (ModuleSession *s = findSession(ecuId, moduleNumber)) {
        addLog(QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  ECU " + ecuId + " Modül " + QString::number(moduleNumber) + "  ->  bağlantıyı kes");
        s->disconnectModule();
    }
}

void Dispatcher::setMode(const QString &ecuId, int moduleNumber, const QString &mode)
{
    if (ModuleSession *s = findSession(ecuId, moduleNumber)) {
        addLog(QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  ECU " + ecuId + " Modül " + QString::number(moduleNumber) + "  ->  mod: " + mode);
        s->setMode(mode);
    }
}

void Dispatcher::setPower(const QString &ecuId, int moduleNumber, const QString &state)
{
    if (m_failSafeActive) {
        addLog(QStringLiteral("[UYARI] Fail-safe aktif, güç komutu reddedildi."));
        return;
    }
    if (ModuleSession *s = findSession(ecuId, moduleNumber)) {
        addLog(QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  ECU " + ecuId + " Modül " + QString::number(moduleNumber) + "  ->  güç: " + state);
        s->setPower(state);
    }
}

// ── Firmware staging ──────────────────────────────────────────────────────

QString Dispatcher::firmwareKey(const QString &ecuId, int moduleNumber)
{
    return ecuId + QString::number(moduleNumber);
}

bool Dispatcher::selectFirmware(const QString &ecuId, int moduleNumber, const QUrl &fileUrl)
{
    ModuleSession *session = findSession(ecuId, moduleNumber);
    if (!session) {
        emit firmwareRejected(ecuId, moduleNumber, QStringLiteral("Bilinmeyen modül."));
        return false;
    }
    if (!session->connected()) {
        emit firmwareRejected(ecuId, moduleNumber,
            QStringLiteral("Modül bağlı değil - önce \"Bağlan\" ile ECU'ya bağlanmalısınız."));
        return false;
    }

    const QString localPath = fileUrl.isLocalFile() ? fileUrl.toLocalFile() : fileUrl.toString();
    QFileInfo info(localPath);

    FirmwareInfo firmware;
    firmware.fileName  = info.fileName();
    firmware.sizeBytes = info.size();

    m_firmwareByModule[firmwareKey(ecuId, moduleNumber)] = firmware;
    emit firmwareChanged(ecuId, moduleNumber);

    addLog(QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  ECU " + ecuId + " Modül " + QString::number(moduleNumber)
           + "  firmware seçildi: " + info.fileName());
    return true;
}

QString Dispatcher::firmwareFileName(const QString &ecuId, int moduleNumber) const
{
    return m_firmwareByModule.value(firmwareKey(ecuId, moduleNumber)).fileName;
}

qint64 Dispatcher::firmwareSizeBytes(const QString &ecuId, int moduleNumber) const
{
    return m_firmwareByModule.value(firmwareKey(ecuId, moduleNumber)).sizeBytes;
}

bool Dispatcher::hasFirmware(const QString &ecuId, int moduleNumber) const
{
    return m_firmwareByModule.contains(firmwareKey(ecuId, moduleNumber));
}

// ── Log (EceGulYuksel branch) ─────────────────────────────────────────────

void Dispatcher::addLog(const QString &line)
{
    m_logMessages.append(line);
    while (m_logMessages.size() > kMaxLogLines)
        m_logMessages.removeFirst();
    emit logMessagesChanged();
}

bool Dispatcher::loadExcelParameters(const QUrl &fileUrl)
{
    const QString localPath = fileUrl.isLocalFile()
                              ? fileUrl.toLocalFile()
                              : fileUrl.toString();

    if (!m_excelParser->loadFile(localPath)) {
        addLog(QStringLiteral("[HATA] ") +
               QDateTime::currentDateTime().toString("hh:mm:ss") +
               "  Excel dosyası açılamadı: " + localPath);
        return false;
    }

    addLog(QDateTime::currentDateTime().toString("hh:mm:ss") +
           "  Excel yüklendi: " +
           QString::number(m_excelParser->parameters().size()) +
           " parametre.");
    return true;
}

// ── Fail-Safe (EceGulYuksel branch) ──────────────────────────────────────

void Dispatcher::triggerFailSafe(const QString &reason)
{
    if (m_failSafeActive)
        return;

    m_failSafeActive = true;
    m_startupTimer.stop();
    m_startupInProgress = false;
    emit failSafeChanged();
    emit startupChanged();

    addLog(QStringLiteral("[KRİTİK] ")
           + QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  FAIL-SAFE ETKİN: " + reason
           + " — tüm modüller durduruldu");

    // Tüm modülleri güvenli duruma çek
    const QList<ModuleSession*> all = { m_ecuAModule1, m_ecuAModule2,
                                        m_ecuBModule1, m_ecuBModule2 };
    for (ModuleSession *s : all)
        s->setPower(QStringLiteral("Off"));
}

void Dispatcher::clearFailSafe()
{
    if (!m_failSafeActive)
        return;

    m_failSafeActive = false;
    emit failSafeChanged();

    addLog(QStringLiteral("[BİLGİ] ")
           + QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  Fail-safe temizlendi, sistem operatör kontrolüne döndü.");
}

void Dispatcher::emergencyStop()
{
    triggerFailSafe(QStringLiteral("Operatör acil durdurma tetikledi"));
}

// ── Sıralı başlatma (EceGulYuksel branch) ────────────────────────────────

void Dispatcher::startupSequence()
{
    if (m_startupInProgress || m_failSafeActive)
        return;

    m_startupInProgress = true;
    m_startupIndex = 0;
    emit startupChanged();

    addLog(QStringLiteral("[BİLGİ] ")
           + QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  Sıralı başlatma başladı (4 modül)...");

    startNextModule();
}

void Dispatcher::startNextModule()
{
    const QList<ModuleSession*> all = { m_ecuAModule1, m_ecuAModule2,
                                        m_ecuBModule1, m_ecuBModule2 };

    if (m_startupIndex >= all.size()) {
        m_startupInProgress = false;
        m_startupTimer.stop();
        emit startupChanged();
        addLog(QStringLiteral("[BİLGİ] ")
               + QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  Sıralı başlatma tamamlandı.");
        return;
    }

    ModuleSession *s = all.at(m_startupIndex);
    addLog(QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  ECU " + s->ecuId() + " Modül " + QString::number(s->moduleNumber())
           + " devreye alınıyor (" + QString::number(m_startupIndex + 1) + "/4)...");

    s->setPower(QStringLiteral("On"));
    s->connectModule();

    m_startupIndex++;
    m_startupTimer.setSingleShot(true);
    m_startupTimer.start(800);  // modüller arası 800ms bekleme
}

void Dispatcher::checkModuleHealth()
{
    int faultyCount = 0;
    const QList<ModuleSession*> all = { m_ecuAModule1, m_ecuAModule2,
                                        m_ecuBModule1, m_ecuBModule2 };
    for (ModuleSession *s : all) {
        if (s->ledState() == QStringLiteral("fail"))
            faultyCount++;
    }
    if (faultyCount >= 2)
        triggerFailSafe(QString::number(faultyCount) + " modül aynı anda hata durumunda");
}
