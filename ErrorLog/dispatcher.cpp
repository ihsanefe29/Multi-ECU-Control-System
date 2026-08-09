#include "dispatcher.h"
#include "ecucontroller.h"
#include <QDebug>
#include <QDateTime>
#include "ecuevent.h"

// Fail-safe tetikleme eşikleri (kolay ayarlanabilir)
static const int  kWindow1Ms   = 3000;  // 3 saniye
static const int  kWindow1Count = 2;     // içinde 2 arıza
static const int  kWindow2Ms   = 6000;  // 6 saniye
static const int  kWindow2Count = 3;     // içinde 3 arıza


Dispatcher::Dispatcher(QObject *parent)
    : QObject(parent)
{
    m_ecuList << new EcuController("ECU A", this)
    << new EcuController("ECU B", this)
    << new EcuController("ECU C", this)
    << new EcuController("ECU D", this);

    for (EcuController *ecu : m_ecuList) {
        connect(ecu, &EcuController::eventLogged, this,
                [this](const QString &msg, int severity, int type) {
                    QString prefix;
                    switch ((EventSeverity)severity) {
                    case EventSeverity::Info:     prefix = "[BİLGİ] "; break;
                    case EventSeverity::Warning:  prefix = "[UYARI] "; break;
                    case EventSeverity::Error:    prefix = "[HATA] ";  break;
                    case EventSeverity::Critical: prefix = "[KRİTİK] "; break;
                    }
            QString logLine = prefix
                              + QDateTime::currentDateTime().toString("hh:mm:ss")
                              + "  " + msg;
            qDebug() << logLine;
            addLog(logLine);

            // Sadece bir ECU "Arızalı" durumuna düştüğünde arıza say
            if (type == (int)EventType::FailSafe)
                recordFault();

            // Sıralı başlatmada ECU'lar arası bekleme
            m_startupTimer.setSingleShot(true);
            m_startupTimer.setInterval(800);   // her ECU arası 0.8 sn
            connect(&m_startupTimer, &QTimer::timeout, this, [this]() {
                startNextEcu();
            });

            // Her ECU'nun self-test sonucunu dinle
            for (EcuController *ecu : m_ecuList) {
                connect(ecu, &EcuController::selfTestResult, this, [this, ecu](bool passed) {
                    if (passed) {
                        ecu->connectEcu();   // test geçti, bağlan
                    }
                    // test kaldıysa bağlama, zaten ECU "Test başarısız" durumunda
                    // Sonraki ECU'ya geç (kısa bekleme sonrası)
                    m_startupTimer.start();
                });
            }
        });
    }
}

void Dispatcher::startupSequence()
{
    if (m_failSafeActive) {
        addLog("[UYARI] " + QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  Fail-safe aktifken sıralı başlatma yapılamaz");
        return;
    }
    if (m_startupInProgress)
        return;

    m_startupInProgress = true;
    emit startupChanged();
    addLog("[BİLGİ] " + QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  Sıralı başlatma başladı");

    m_startupIndex = -1;
    startNextEcu();
}

void Dispatcher::startNextEcu()
{
    m_startupIndex++;

    // Tüm ECU'lar bitti mi?
    if (m_startupIndex >= m_ecuList.size()) {
        m_startupInProgress = false;
        emit startupChanged();
        addLog("[BİLGİ] " + QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  Sıralı başlatma tamamlandı");
        return;
    }

    EcuController *ecu = m_ecuList.at(m_startupIndex);

    addLog("[BİLGİ] " + QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  " + ecu->name() + " devreye alınıyor ("
           + QString::number(m_startupIndex + 1) + "/"
           + QString::number(m_ecuList.size()) + ")");

    // Önce güç ver, sonra self-test yap
    if (!ecu->powered())
        ecu->togglePower();
    ecu->runSelfTest();   // sonucu selfTestResult sinyaliyle gelecek
}

QVariantList Dispatcher::ecus() const
{
    // QList<EcuController*> -> QVariantList (QML'in okuyabileceği biçim)
    QVariantList list;
    for (EcuController *ecu : m_ecuList)
        list << QVariant::fromValue(ecu);
    return list;
}

QStringList Dispatcher::logMessages() const
{
    return m_logMessages;
}

void Dispatcher::addLog(const QString &line)
{
    m_logMessages.append(line);
    // Panel çok şişmesin: son 50 kaydı tut
    while (m_logMessages.size() > 50)
        m_logMessages.removeFirst();
    emit logMessagesChanged();
}

void Dispatcher::sendCommand(int ecuIndex, const QString &command)
{
    // FAIL-SAFE KİLİDİ: aktifken hiçbir ECU komutu işlenmez
    if (m_failSafeActive) {
        addLog("[UYARI] " + QDateTime::currentDateTime().toString("hh:mm:ss")
               + "  Fail-safe aktif, komut reddedildi: " + command);
        return;
    }

    // Güvenlik: geçersiz index gelirse hiçbir şey yapma
    if (ecuIndex < 0 || ecuIndex >= m_ecuList.size()) {
        qDebug() << "Dispatcher: geçersiz ECU index:" << ecuIndex;
        return;
    }
    EcuController *ecu = m_ecuList.at(ecuIndex);

    // AKIŞ LOGU: her komut tek yerden geçiyor, hepsini buradan görüyoruz
    QString logLine = QDateTime::currentDateTime().toString("hh:mm:ss")
                      + "  " + ecu->name() + "  ->  " + command;
    qDebug() << logLine;
    addLog(logLine);          // panele de ekle

    // Komutu doğru ECU'ya dağıt
    if (command == "power")        ecu->togglePower();
    else if (command == "connect") ecu->connectEcu();
    else if (command == "disconnect") ecu->disconnectEcu();
    else if (command == "kill")    ecu->killEcu();
    else if (command == "mode")    ecu->toggleMode();
    else if (command == "reset")   ecu->resetEcu();
    else qDebug() << "Dispatcher: bilinmeyen komut:" << command;
}

bool Dispatcher::failSafeActive() const
{
    return m_failSafeActive;
}

void Dispatcher::checkSystemHealth()
{
    if (m_failSafeActive)
        return;   // zaten fail-safe'teyiz, tekrar tetikleme

    // Kaç ECU arızalı?
    int faultyCount = 0;
    for (EcuController *ecu : m_ecuList) {
        if (ecu->status() == "Arızalı")
            faultyCount++;
    }

    // Birden fazla ECU aynı anda arızalıysa: sistemik sorun, fail-safe'e geç
    if (faultyCount >= 2) {
        triggerFailSafe(QString::number(faultyCount)
                        + " ECU aynı anda arızalı, sistemik sorun");
    }
}

void Dispatcher::triggerFailSafe(const QString &reason)
{
    if (m_failSafeActive)
        return;

    m_failSafeActive = true;
    emit failSafeChanged();

    addLog("[KRİTİK] " + QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  FAIL-SAFE ETKİN: " + reason
           + " — tüm ECU'lar durduruldu, güç kesildi");

    // Her ECU'yu tamamen sustur: timer'lar dur, veri dur, güç kes
    for (EcuController *ecu : m_ecuList)
        ecu->shutdownForFailSafe();
}

void Dispatcher::clearFailSafe()
{
    if (!m_failSafeActive)
        return;

    m_failSafeActive = false;
    emit failSafeChanged();

    addLog("[BİLGİ] " + QDateTime::currentDateTime().toString("hh:mm:ss")
           + "  Fail-safe temizlendi, sistem operatör kontrolüne döndü");

    // Tüm ECU'ları nötr/güvenli başlangıca çek
    for (EcuController *ecu : m_ecuList)
        ecu->resetEcu();
}

void Dispatcher::emergencyStop()
{
    triggerFailSafe("Operatör acil durdurma tetikledi");
}

void Dispatcher::recordFault()
{
    if (m_failSafeActive)
        return;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    m_faultTimestamps.append(now);

    // Eski kayıtları temizle (en geniş pencereden daha eskiler gereksiz)
    while (!m_faultTimestamps.isEmpty()
           && now - m_faultTimestamps.first() > kWindow2Ms) {
        m_faultTimestamps.removeFirst();
    }

    // Son kWindow1Ms içinde kaç arıza var?
    int countInWindow1 = 0;
    int countInWindow2 = m_faultTimestamps.size();  // hepsi zaten <= kWindow2Ms
    for (qint64 t : m_faultTimestamps) {
        if (now - t <= kWindow1Ms)
            countInWindow1++;
    }

    // Eşik kontrolü: iki kuraldan biri sağlanırsa fail-safe
    if (countInWindow1 >= kWindow1Count) {
        triggerFailSafe(QString::number(countInWindow1) + " ECU "
                        + QString::number(kWindow1Ms / 1000)
                        + " sn içinde arızalandı (ardarda arıza)");
    }
    else if (countInWindow2 >= kWindow2Count) {
        triggerFailSafe(QString::number(countInWindow2) + " ECU "
                        + QString::number(kWindow2Ms / 1000)
                        + " sn içinde arızalandı (yaygın arıza)");
    }
}

bool Dispatcher::startupInProgress() const { return m_startupInProgress; }