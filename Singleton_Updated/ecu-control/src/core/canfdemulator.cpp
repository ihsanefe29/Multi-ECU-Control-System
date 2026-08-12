#include "canfdemulator.h"
#include "ccpprotocol.h"

#include <QTimer>
#include <QRandomGenerator>

CanFdEmulator::CanFdEmulator(QObject *parent)
    : ICanFdConnector(parent)
{
}

void CanFdEmulator::sendFrame(quint32 canId, const QByteArray &data)
{
    const QByteArray response = buildFakeResponse(data);

    // Simulated bus/ECU processing delay. Delivered asynchronously via
    // the frameReceived signal when the timer fires - this is the
    // "signal, not polling" delivery model discussed for the real
    // driver too.
    QTimer::singleShot(15, this, [this, canId, response]() {
        emit frameReceived(canId, response);
    });
}

QByteArray CanFdEmulator::buildFakeResponse(const QByteArray &request) const
{
    if (request.isEmpty())
        return QByteArray();

    const auto command = static_cast<Ccp::Command>(static_cast<quint8>(request.at(0)));
    const quint8 counter = request.size() > 1 ? static_cast<quint8>(request.at(1)) : 0;

    QByteArray response;
    response.append(char(static_cast<quint8>(Ccp::ReturnStatus::Acknowledge)));
    response.append(char(counter));

    switch (command) {
    case Ccp::Command::GetSeed: {
        // Fake 4-byte seed so the UNLOCK flow has something to react to.
        for (int i = 0; i < 4; ++i)
            response.append(char(QRandomGenerator::global()->bounded(256)));
        break;
    }
    default:
        // CONNECT / UNLOCK / SET_MTA / DNLOAD / DISCONNECT: plain
        // acknowledge is enough for the skeleton.
        break;
    }

    while (response.size() < 8)
        response.append(char(0));

    return response;
}
