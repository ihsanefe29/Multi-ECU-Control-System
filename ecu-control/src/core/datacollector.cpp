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
    qDebug() << "[DataCollector] Ba┼şlad─▒, interval:" << intervalMs << "ms";
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

    emit packetReady(packet);   // art─▒k dispatcher'─▒ do─şrudan ├ğa─ş─▒rm─▒yor, sadece haber veriyor
}
