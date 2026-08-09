#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QObject>
#include <QList>
#include <QVariant>
#include <QStringList>
#include <QDateTime>
#include <QTimer>

class EcuController;

class Dispatcher : public QObject
{
    Q_OBJECT
    // ECU listesini QML'e açıyoruz ki arayüz kutuları buradan üretsin
    Q_PROPERTY(QVariantList ecus READ ecus CONSTANT)
    Q_PROPERTY(QStringList logMessages READ logMessages NOTIFY logMessagesChanged)
    Q_PROPERTY(bool failSafeActive READ failSafeActive NOTIFY failSafeChanged)
    Q_PROPERTY(bool startupInProgress READ startupInProgress NOTIFY startupChanged)
    bool startupInProgress() const;
public:
    explicit Dispatcher(QObject *parent = nullptr);

    QVariantList ecus() const;
    QStringList logMessages() const;

    // QML'in çağıracağı TEK kapı: "şu ECU'ya, şu komutu gönder"
    Q_INVOKABLE void sendCommand(int ecuIndex, const QString &command);
    bool failSafeActive() const;
    Q_INVOKABLE void triggerFailSafe(const QString &reason);  // fail-safe'e geç
    Q_INVOKABLE void clearFailSafe();                          // operatör kurtarması
    Q_INVOKABLE void emergencyStop();                          // acil durdur (manuel)
    Q_INVOKABLE void startupSequence();   // sıralı başlatmayı başlatır

signals:
    void logMessagesChanged();
    void failSafeChanged();
    void startupChanged();

private:
    QList<EcuController*> m_ecuList;   // santralın kişi rehberi
    QStringList m_logMessages;
    void addLog(const QString &line);
    bool m_failSafeActive = false;
    void checkSystemHealth();     // kaç ECU arızalı diye bakar
    QList<qint64> m_faultTimestamps;   // arızaların zaman damgaları (ms)
    void recordFault();                // yeni bir arıza kaydeder ve eşik kontrolü yapar
    bool m_startupInProgress = false;
    int m_startupIndex = -1;          // şu an hangi ECU'yu başlatıyoruz
    QTimer m_startupTimer;            // ECU'lar arası bekleme
    void startNextEcu();              // sıradaki ECU'yu başlat
};

#endif // DISPATCHER_H