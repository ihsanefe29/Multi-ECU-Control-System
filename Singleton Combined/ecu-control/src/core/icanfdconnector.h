#pragma once

#include <QObject>
#include <QByteArray>

// Abstract interface for a CAN FD transport.
//
// Design decision (per project discussion): ONE shared connector for
// all ECUs, rather than one connector per ECU. Real hardware usually
// exposes a single physical CAN FD interface for a given bus, so
// modeling it as one shared connector matches reality; ECUs/modules
// are told apart by CAN ID, not by having separate connector objects.
//
// Delivery is signal-based (frameReceived), not polling: an emulator
// or real driver calls back whenever a frame arrives, which keeps
// worker threads free instead of spinning a poll loop.
class ICanFdConnector : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    // Send is expected to be safe to call from any thread; real
    // implementations should marshal internally (e.g. via a queued
    // invokeMethod) if the underlying driver isn't thread-safe.
    virtual void sendFrame(quint32 canId, const QByteArray &data) = 0;

signals:
    void frameReceived(quint32 canId, QByteArray data);
};
