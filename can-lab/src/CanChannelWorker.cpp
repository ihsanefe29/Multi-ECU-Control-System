#include "CanChannelWorker.h"
#include <QVariant>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>

#include <utility>

CanChannelWorker::CanChannelWorker(QString plugin, QString channel, bool supportsCanFd)
    : m_plugin(std::move(plugin)),
      m_channel(std::move(channel)),
      m_supportsCanFd(supportsCanFd)
{
}

void CanChannelWorker::initialize()
{
    QString error;
    m_device = QCanBus::instance()->createDevice(m_plugin, m_channel, &error);
    if (!m_device) {
        emit errorOccurred(m_channel, error);
        emit finished();
        return;
    }

    m_device->setParent(this); //parent object is this and on delete delete device to
    // Do not force CAN FD on Classical-CAN-only USB adapters. SocketCAN
    // already owns bitrate/FD configuration; this only enables FD frames
    // when the discovered interface reports that it supports them.
    if (m_supportsCanFd)
        m_device->setConfigurationParameter(QCanBusDevice::CanFdKey, QVariant(true)); //QVariant used so that int bool etc can be send via one parameter

    connect(m_device, &QCanBusDevice::framesReceived,
            this, &CanChannelWorker::readFrames);


    connect(m_device, &QCanBusDevice::errorOccurred,
            this, [this](QCanBusDevice::CanBusError) {
                emit errorOccurred(m_channel, m_device->errorString());
            });
    
            
    //could be updated to:
    /*
    connect(m_device, &QCanBusDevice::errorOccurred,
        this, [this](QCanBusDevice::CanBusError error) { 
            
            // 1. Hatayı analiz et
            if (error == QCanBusDevice::ConnectionError) {
                qDebug() << "cable disconected";
                // emit corresponding 
            } 
            else if (error == QCanBusDevice::WriteError) {
                //emit corresponding 
            }

            
            emit errorOccurred(m_channel, m_device->errorString());
        });
        */

    if (!m_device->connectDevice()) {
        emit errorOccurred(m_channel, m_device->errorString());
        emit finished();
        return;
    }

    emit connected(m_channel);
}

void CanChannelWorker::sendFrame(quint32 id, bool isFd, QByteArray payload)
{
    const qsizetype limit = isFd ? 64:8;

    if (!m_device || m_device->state() != QCanBusDevice::ConnectedState) {
        emit errorOccurred(m_channel, QStringLiteral("CAN device is not connected"));
        return;
    }
    if (payload.size() > limit) {
        emit errorOccurred(m_channel, QStringLiteral("payload exceeds %1 bytes").arg(limit));
        return;
    }
    if (isFd && !m_supportsCanFd) {
        emit errorOccurred(m_channel, QStringLiteral("interface does not support CAN FD"));
        return;
    }

    QCanBusFrame frame(id, payload);
    frame.setFlexibleDataRateFormat(isFd);
    frame.setBitrateSwitch(isFd);
    if (!m_device->writeFrame(frame))
        emit errorOccurred(m_channel, m_device->errorString());
}

void CanChannelWorker::readFrames()
{
    while (m_device && m_device->framesAvailable() > 0) {
        const QCanBusFrame raw = m_device->readFrame();
        if (raw.frameType() != QCanBusFrame::DataFrame)
            continue;
        
        const CanFrame newFrame = {m_channel, raw.frameId(), raw.hasFlexibleDataRateFormat(), raw.payload()};
        emit frameReceived(newFrame);
    }
}

void CanChannelWorker::shutdown()
{
    if (m_device)
        m_device->disconnectDevice();
    emit finished();
}
