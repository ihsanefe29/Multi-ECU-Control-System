#pragma once

#include "CanFrame.h"

#include <QObject>

class QCanBusDevice;

class CanChannelWorker final : public QObject
{
    Q_OBJECT

public:
    explicit CanChannelWorker(QString plugin, QString channel, bool supportsCanFd);

public slots:
    void initialize();
    void sendFrame(quint32 id, bool isFd, QByteArray payload);
    void shutdown();

signals:
    void connected(QString channel);
    void frameReceived(CanFrame frame);
    void errorOccurred(QString channel, QString description);
    void finished();

private slots:
    void readFrames();

private:
    QString m_plugin;
    QString m_channel;
    bool m_supportsCanFd = false;
    QCanBusDevice *m_device = nullptr;
};
