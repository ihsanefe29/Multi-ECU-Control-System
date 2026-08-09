#include "ecuworker.h"
#include <QTimer>
#include <QRandomGenerator>
#include <QThread>
#include <QDebug>

EcuWorker::EcuWorker(const QString &name, QObject *parent)
    : QObject(parent), m_name(name)
{
}

void EcuWorker::startProducing()
{
    // Timer'ı burada (worker thread'inde) oluşturuyoruz ki doğru thread'e ait olsun.
    if (!m_timer) {
        m_timer = new QTimer(this);
        m_timer->setInterval(500);
        connect(m_timer, &QTimer::timeout, this, [this]() {
            int rpm;
            // ~%4 ihtimalle bozuk (aralık dışı) veri üret: gürültü/hata simülasyonu
            if (QRandomGenerator::global()->bounded(100) < 4) {
                // ya çok yüksek ya negatif, mantıksız bir değer
                if (QRandomGenerator::global()->bounded(2) == 0)
                    rpm = 50000 + QRandomGenerator::global()->bounded(15000); // aşırı yüksek
                else
                    rpm = -QRandomGenerator::global()->bounded(2000);          // negatif
            } else {
                rpm = 800 + QRandomGenerator::global()->bounded(4200);        // normal
            }
            emit rpmProduced(rpm);
        });
    }
    // Kanıt: hangi thread'de çalıştığını konsola yaz
    qDebug() << m_name << "worker thread:" << QThread::currentThreadId();
    m_timer->start();
}

void EcuWorker::stopProducing()
{
    if (m_timer)
        m_timer->stop();
    emit rpmProduced(0);
}