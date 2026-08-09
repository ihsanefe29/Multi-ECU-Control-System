#ifndef ECUCONTROLLER_H
#define ECUCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QString>

class EcuWorker;

class EcuController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(int rpm READ rpm NOTIFY rpmChanged)
    Q_PROPERTY(bool powered READ powered NOTIFY poweredChanged)
    Q_PROPERTY(QString mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(double voltage READ voltage NOTIFY voltageChanged)

public:
    explicit EcuController(const QString &name, QObject *parent = nullptr);
    ~EcuController();

    QString name() const;
    QString status() const;
    int rpm() const;
    bool powered() const;
    QString mode() const;
    double voltage() const;

    Q_INVOKABLE void connectEcu();
    Q_INVOKABLE void disconnectEcu();
    Q_INVOKABLE void killEcu();
    Q_INVOKABLE void togglePower();
    Q_INVOKABLE void toggleMode();
    Q_INVOKABLE void resetEcu();
    Q_INVOKABLE void runSelfTest();

    void shutdownForFailSafe();   // fail-safe: her şeyi durdur, gücü kes

signals:
    void statusChanged();
    void rpmChanged();
    void poweredChanged();
    void modeChanged();
    void voltageChanged();

    // controller -> worker (thread sınırını geçer)
    void requestStart();
    void requestStop();

    // zengin olay sinyali: mesaj + ciddiyet + tür
    void eventLogged(const QString &message, int severity, int type);

    // self-test sonucu (dispatcher dinler)
    void selfTestResult(bool passed);

private slots:
    void onRpmProduced(int rpm);   // worker -> controller

private:
    void setStatus(const QString &newStatus);
    void setRpm(int newRpm);
    void setPowered(bool p);
    void setMode(const QString &m);
    void setVoltage(double v);
    void attemptConnection();      // tek bir bağlanma denemesi yürütür

    QString m_name;
    QString m_status;
    int m_rpm = 0;
    bool m_powered = false;
    QString m_mode = "Veri Görüntüleme";
    double m_voltage = 0.0;

    // threading
    QThread m_thread;
    EcuWorker *m_worker = nullptr;

    // yeniden bağlanma / retry
    QTimer m_reconnectTimer;       // kopunca sonraki denemeyi zamanlar
    QTimer m_connectingTimer;      // bir denemenin "sürmesini" simüle eder
    int m_reconnectAttempts = 0;
    bool m_userDisconnected = false;
    static const int kMaxAttempts = 3;

    // veri doğrulama
    static const int kRpmMin = 0;
    static const int kRpmMax = 8000;
    int m_lastValidRpm = 0;
};

#endif // ECUCONTROLLER_H