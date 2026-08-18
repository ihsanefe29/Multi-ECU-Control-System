#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include "parametermodel.h"
#include "packetbuilder.h"

class DataCollector : public QObject
{
    Q_OBJECT

public:
    explicit DataCollector(ParameterModel *model, PacketBuilder *packetBuilder, QObject *parent = nullptr);

    Q_INVOKABLE void start(int intervalMs);
    Q_INVOKABLE void stop();

signals:
    void packetReady(const QByteArray &packet);   // yeni eklenen sinyal

private slots:
    void onTick();

private:
    ParameterModel *m_model;
    PacketBuilder *m_packetBuilder;
    QTimer m_timer;
};

#endif // DATACOLLECTOR_H