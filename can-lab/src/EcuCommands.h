#pragma once

#include <QtGlobal>

enum class EcuCommand : quint8
{
    PowerOn       = 0x01,
    PowerOff      = 0x02,
    RequestStatus = 0x03
};

inline bool isValidCommand(EcuCommand command)
{
    switch (command) {
    case EcuCommand::PowerOn:
    case EcuCommand::PowerOff:
    case EcuCommand::RequestStatus:
        return true;
    }

    return false;
}