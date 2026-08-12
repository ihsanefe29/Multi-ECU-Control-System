#pragma once

#include "icanfdconnector.h"

// A software-only stand-in for a real CAN FD interface, so the rest
// of the app (Dispatcher, ModuleSession, QML UI) can be developed and
// tested without physical hardware.
//
// It answers every sent frame with a plausible CCP response after a
// short simulated bus delay, delivered via the frameReceived signal
// (QTimer::singleShot -> emit), never via polling.
class CanFdEmulator : public ICanFdConnector
{
    Q_OBJECT

public:
    explicit CanFdEmulator(QObject *parent = nullptr);

    void sendFrame(quint32 canId, const QByteArray &data) override;

private:
    QByteArray buildFakeResponse(const QByteArray &request) const;
};
