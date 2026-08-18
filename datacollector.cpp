#include "datacollector.h"
#include <QDebug>

DataCollector::DataCollector(ParameterModel *model, PacketBuilder *packetBuilder, QObject *parent)
    : QObject(parent), m_model(model), m_packetBuilder(packetBuilder)
{
    connect(&m_timer, &QTimer::timeout, this, &DataCollector::onTick);
}

void DataCollector::start(int intervalMs)
{
    m_timer.start(intervalMs);
    qDebug() << "[DataCollector] Başladı, interval:" << intervalMs << "ms";
}

void DataCollector::stop()
{
    m_timer.stop();
    qDebug() << "[DataCollector] Durduruldu";
}

void DataCollector::onTick()
{
    QVariantList orderedValues = m_model->getOrderedValues();
    QByteArray packet = m_packetBuilder->buildPacket(orderedValues);

    emit packetReady(packet);   // artık dispatcher'ı doğrudan çağırmıyor, sadece haber veriyor
}