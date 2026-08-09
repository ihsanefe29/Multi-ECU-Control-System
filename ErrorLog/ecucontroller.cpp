#include "ecuevent.h"
#include "ecucontroller.h"
#include "ecuworker.h"
#include <QRandomGenerator>

EcuController::EcuController(const QString &name, QObject *parent)
    : QObject(parent), m_name(name), m_status("Bağlı değil")
{
    m_worker = new EcuWorker(name);        // parent yok, thread'e taşınacak
    m_worker->moveToThread(&m_thread);     // worker artık ayrı thread'de

    // thread bitince worker'ı temizle
    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);

    // controller → worker (farklı thread → otomatik queued/thread-güvenli)
    connect(this, &EcuController::requestStart, m_worker, &EcuWorker::startProducing);
    connect(this, &EcuController::requestStop,  m_worker, &EcuWorker::stopProducing);

    // worker → controller (farklı thread → otomatik queued)
    connect(m_worker, &EcuWorker::rpmProduced, this, &EcuController::onRpmProduced);

    m_thread.start();      // thread uygulama boyunca ayakta kalır

    // Denemeler arası bekleme (kopma ile sonraki deneme arası)
    m_reconnectTimer.setSingleShot(true);
    m_reconnectTimer.setInterval(1500);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this]() {
        attemptConnection();     // yeni bir deneme başlat
    });

    // Bir denemenin "sürmesini" simüle eden timer (bağlanma gecikmesi)
    m_connectingTimer.setSingleShot(true);
    m_connectingTimer.setInterval(1500);   // her deneme ~1.5 sn sürer
    connect(&m_connectingTimer, &QTimer::timeout, this, [this]() {
        // Deneme bitti: rastgele başarılı/başarısız (%60 başarı)
        bool success = QRandomGenerator::global()->bounded(100) < 60;

        if (success) {
            setStatus("Bağlı");
            m_reconnectAttempts = 0;
            emit eventLogged(m_name + " yeniden bağlandı",
                             (int)EventSeverity::Info, (int)EventType::Command);
            emit requestStart();
        } else {
            emit eventLogged(m_name + " deneme " + QString::number(m_reconnectAttempts)
                             + " başarısız",
                             (int)EventSeverity::Warning, (int)EventType::ConnectionLost);

            if (m_reconnectAttempts >= kMaxAttempts) {
                // Pes et: arızalı duruma geç
                setStatus("Arızalı");
                emit eventLogged(m_name + " " + QString::number(kMaxAttempts)
                                     + " denemede bağlanamadı, arızalı olarak işaretlendi",
                                 (int)EventSeverity::Critical, (int)EventType::FailSafe);
            } else {
                // Sonraki denemeyi zamanla
                m_reconnectTimer.start();
            }
        }
    });
}

EcuController::~EcuController()
{
    m_thread.quit();       // thread'in olay döngüsünü durdur
    m_thread.wait();       // tamamen bitmesini bekle
}

QString EcuController::name() const   { return m_name; }
QString EcuController::status() const { return m_status; }
int EcuController::rpm() const        { return m_rpm; }
bool EcuController::powered() const { return m_powered; }
QString EcuController::mode() const { return m_mode; }

void EcuController::connectEcu()
{
    if (!m_powered)
        return;
    m_userDisconnected = false;
    setStatus("Bağlı");
    m_reconnectAttempts = 0;      // başarılı bağlantı, sayaç sıfırla
    emit requestStart();
}

void EcuController::attemptConnection()
{
    if (m_userDisconnected)
        return;

    m_reconnectAttempts++;
    setStatus("Bağlanıyor");
    emit eventLogged(m_name + " bağlanmayı deniyor... (deneme "
                         + QString::number(m_reconnectAttempts) + "/"
                         + QString::number(kMaxAttempts) + ")",
                     (int)EventSeverity::Warning, (int)EventType::ConnectionLost);
    m_connectingTimer.start();   // deneme "sürecini" başlat
}

void EcuController::disconnectEcu()
{
    m_userDisconnected = true;
    m_reconnectTimer.stop();
    m_connectingTimer.stop();     // süren deneme varsa iptal et
    setStatus("Bağlı değil");
    emit requestStop();
    setRpm(0);
}

void EcuController::killEcu()
{
    setStatus("Kill");
    emit requestStop();
    setRpm(0);
}

void EcuController::shutdownForFailSafe()
{
    // Tüm otomatik davranışları durdur
    m_reconnectTimer.stop();
    m_connectingTimer.stop();
    m_reconnectAttempts = 0;
    m_userDisconnected = true;   // otomatik yeniden bağlanma olmasın

    // Veri akışını durdur
    emit requestStop();
    setRpm(0);

    // Gücü de kes: en pasif, en güvenli hal
    setPowered(false);
    setStatus("Bağlı değil");
}

void EcuController::resetEcu()
{
    // Arızayı temizle: timer'ları durdur, sayaçları sıfırla, bekleme durumuna dön
    m_reconnectTimer.stop();
    m_connectingTimer.stop();
    m_reconnectAttempts = 0;
    m_userDisconnected = false;
    setRpm(0);
    setStatus("Bağlı değil");   // güç hâlâ var, sadece bağlantı yok
    emit eventLogged(m_name + " sıfırlandı, arıza temizlendi",
                     (int)EventSeverity::Info, (int)EventType::Command);
}

void EcuController::togglePower()
{
    if (m_powered) {
        setPowered(false);
        disconnectEcu();     // güç kesilince bağlantı da düşer
        setStatus("Bağlı değil");
    } else {
        setPowered(true);
    }
}

void EcuController::toggleMode()
{
    if (m_mode == "Veri Görüntüleme")
        setMode("Yazılım Yükleme");
    else
        setMode("Veri Görüntüleme");
}

void EcuController::setMode(const QString &m)
{
    if (m_mode == m)
        return;
    m_mode = m;
    emit modeChanged();
}

void EcuController::setPowered(bool p)
{
    if (m_powered == p)
        return;
    m_powered = p;
    emit poweredChanged();
}

void EcuController::onRpmProduced(int newRpm)
{
    if (m_status == "Bağlı" && m_mode == "Veri Görüntüleme") {

        if (QRandomGenerator::global()->bounded(100) < 3) {
            setStatus("Yeniden bağlanıyor");
            emit requestStop();
            setRpm(0);
            emit eventLogged(m_name + " bağlantısı koptu",
                             (int)EventSeverity::Error, (int)EventType::ConnectionLost);

            m_reconnectAttempts = 0;         // sayaç sıfırla, yeni bir tur başlıyor
            if (!m_userDisconnected) {
                m_reconnectTimer.start();    // 1.5 sn sonra ilk denemeyi yap
            }
            return;
        }

        // VERİ DOĞRULAMA: gelen değer geçerli aralıkta mı?
        if (newRpm < kRpmMin || newRpm > kRpmMax) {
            // Bozuk veri: reddet, son geçerli değeri koru, uyar
            emit eventLogged(m_name + " geçersiz veri reddedildi (rpm="
                                 + QString::number(newRpm) + "), son geçerli değer korunuyor",
                             (int)EventSeverity::Warning, (int)EventType::InvalidData);
            setRpm(m_lastValidRpm);   // ekranda son sağlam değer kalsın
        } else {
            m_lastValidRpm = newRpm;  // bunu geçerli kabul et, sakla
            setRpm(newRpm);
        }
    } else {
        setRpm(0);
    }
}

void EcuController::setStatus(const QString &newStatus)
{
    if (m_status == newStatus)
        return;
    m_status = newStatus;
    emit statusChanged();
}

void EcuController::setRpm(int newRpm)
{
    if (m_rpm == newRpm)
        return;
    m_rpm = newRpm;
    emit rpmChanged();
}

double EcuController::voltage() const { return m_voltage; }

void EcuController::setVoltage(double v)
{
    if (qFuzzyCompare(m_voltage, v))
        return;
    m_voltage = v;
    emit voltageChanged();
}

void EcuController::runSelfTest()
{
    setStatus("Test ediliyor");
    emit eventLogged(m_name + " ön kontrol başladı (voltaj/hat)",
                     (int)EventSeverity::Info, (int)EventType::Command);

    // Voltaj ölç (simülasyon): normal 24V sistem, 22-26 arası kabul
    // Ara sıra bozuk (düşük/yüksek) voltaj üret ki test bazen kalsın
    double v;
    if (QRandomGenerator::global()->bounded(100) < 80) {
        // %80 sağlıklı: 23.0 - 25.0 V arası
        v = 23.0 + QRandomGenerator::global()->generateDouble() * 2.0;
    } else {
        // %20 sorunlu: ya çok düşük ya çok yüksek
        if (QRandomGenerator::global()->bounded(2) == 0)
            v = 18.0 + QRandomGenerator::global()->generateDouble() * 3.0; // düşük
        else
            v = 27.0 + QRandomGenerator::global()->generateDouble() * 3.0; // yüksek
    }
    setVoltage(v);

    // Kabul aralığı: 22.0 - 26.0 V
    bool passed = (v >= 22.0 && v <= 26.0);

    if (passed) {
        emit eventLogged(m_name + QString(" ön kontrol GEÇTİ (%.1f V)")
                                      .arg(v),
                         (int)EventSeverity::Info, (int)EventType::Command);
    } else {
        setStatus("Test başarısız");
        emit eventLogged(m_name + QString(" ön kontrol KALDI (%.1f V, aralık dışı)")
                                      .arg(v),
                         (int)EventSeverity::Warning, (int)EventType::InvalidData);
    }

    emit selfTestResult(passed);
}

