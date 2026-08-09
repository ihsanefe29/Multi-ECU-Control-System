#ifndef ECUWORKER_H
#define ECUWORKER_H

#include <QObject>
#include <QString>

class QTimer;

// Gerçek işi yapan nesne. Kendi thread'inde yaşar.
class EcuWorker : public QObject
{
    Q_OBJECT
public:
    explicit EcuWorker(const QString &name, QObject *parent = nullptr);

public slots:
    void startProducing();   // ana thread'den sinyalle çağrılır
    void stopProducing();

signals:
    void rpmProduced(int rpm);   // worker → controller

private:
    QString m_name;
    QTimer *m_timer = nullptr;
};

#endif // ECUWORKER_H