#pragma once

#include <QtGlobal>

enum class EcuCommand : quint8
{
    PowerOn       = 0x01,
    PowerOff      = 0x02,
    RequestStatus = 0x05,
};

enum class DiscoveryMessage : quint8
{
    Request  = 0xD1,
    Response = 0xD2
};

namespace CanProtocol
{
    constexpr quint32 DiscoveryRequestId       = 0x7A0;
    constexpr quint32 DiscoveryResponseFirstId = 0x7A1;
    constexpr quint32 DiscoveryResponseLastId  = 0x7AF;

    constexpr qint64 HeartbeatTimeoutMs = 500;
}