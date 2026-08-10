#ifndef UIMESSAGE_H
#define UIMESSAGE_H

#include "../enums/ECUType.h"
#include "../enums/CommandType.h"
#include "../enums/ModeType.h"

struct UIMessage
{
    ECUType ecu;
    CommandType command;
    ModeType mode = ModeType::Normal;
};

#endif // UIMESSAGE_H