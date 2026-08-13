#pragma once

#include <QtGlobal>
#include <QVector>
#include <QMetaType>


// One raw telemetry item coming from an ECU.
//
// The CommunicationCore does NOT convert this value.
// It only extracts the RAM address and the raw 32-bit value
// from the CAN / CAN FD payload.
//
// Example:
//
// RAM_ADDR = 0x00000008
// DATA     = 0x00003039
//
// Dispatcher / RawSignalLoader will later decide that
// address 0x08 means something like "Vibration" and
// apply the conversion formula.
struct TelemetryValue
{
    quint32 ramAddress = 0;
    quint32 rawValue = 0;
};


// One telemetry update received from one ECU.
//
// A CAN FD frame may contain multiple RAM-address / value pairs,
// therefore values is a QVector rather than one single value.
//
// Example:
//
// ECU A
//
// values[0]:
//   ramAddress = 0x00000000
//   rawValue   = 5000
//
// values[1]:
//   ramAddress = 0x00000002
//   rawValue   = 700
//
// values[2]:
//   ramAddress = 0x00000008
//   rawValue   = 12345
//
struct EcuTelemetry
{
    char ecu = '\0';

    QVector<TelemetryValue> values;
};


// Needed when these structs are passed through Qt signals / slots,
// especially queued connections between threads.
Q_DECLARE_METATYPE(TelemetryValue)
Q_DECLARE_METATYPE(EcuTelemetry)