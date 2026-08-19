#include "ccpprotocol.h"

namespace {
constexpr int kCroLength = 8;

QByteArray padTo(QByteArray frame, int length = kCroLength)
{
    while (frame.size() < length)
        frame.append(char(0));
    return frame;
}
}

namespace Ccp {

QByteArray buildConnect(quint8 counter, quint16 stationAddress)
{
    QByteArray frame;
    frame.append(char(static_cast<quint8>(Command::Connect)));
    frame.append(char(counter));
    frame.append(char(stationAddress & 0xFF));
    frame.append(char((stationAddress >> 8) & 0xFF));
    return padTo(frame);
}

QByteArray buildGetSeed(quint8 counter)
{
    QByteArray frame;
    frame.append(char(static_cast<quint8>(Command::GetSeed)));
    frame.append(char(counter));
    return padTo(frame);
}

QByteArray buildUnlock(quint8 counter, const QByteArray &key)
{
    QByteArray frame;
    frame.append(char(static_cast<quint8>(Command::Unlock)));
    frame.append(char(counter));
    // Classic CCP UNLOCK carries up to 6 key bytes in one CRO; longer
    // keys would need multiple UNLOCK exchanges - not handled here.
    frame.append(key.left(6));
    return padTo(frame);
}

QByteArray buildSetMta(quint8 counter, quint32 address)
{
    QByteArray frame;
    frame.append(char(static_cast<quint8>(Command::SetMta)));
    frame.append(char(counter));
    frame.append(char(0)); // MTA number (0 = default)
    frame.append(char(0)); // address extension
    frame.append(char(address & 0xFF));
    frame.append(char((address >> 8) & 0xFF));
    frame.append(char((address >> 16) & 0xFF));
    frame.append(char((address >> 24) & 0xFF));
    return padTo(frame);
}

QByteArray buildDnload(quint8 counter, const QByteArray &payload)
{
    QByteArray frame;
    frame.append(char(static_cast<quint8>(Command::Dnload)));
    frame.append(char(counter));
    frame.append(char(payload.size())); // byte count for this chunk
    frame.append(payload.left(5));      // classic CRO fits 5 data bytes here
    return padTo(frame);
}

QByteArray buildDisconnect(quint8 counter, quint16 stationAddress)
{
    QByteArray frame;
    frame.append(char(static_cast<quint8>(Command::Disconnect)));
    frame.append(char(counter));
    frame.append(char(0)); // temporary/permanent disconnect flag
    frame.append(char(0));
    frame.append(char(stationAddress & 0xFF));
    frame.append(char((stationAddress >> 8) & 0xFF));
    return padTo(frame);
}

ParsedResponse parseResponse(const QByteArray &frame)
{
    ParsedResponse result;
    if (frame.size() < 2)
        return result;

    result.valid = true;
    result.status = static_cast<ReturnStatus>(static_cast<quint8>(frame.at(0)));
    result.counter = static_cast<quint8>(frame.at(1));
    result.payload = frame.mid(2);
    return result;
}

} // namespace Ccp
