#pragma once

#include <QByteArray>
#include <QtGlobal>

// Minimal CCP (CAN Calibration Protocol, ASAM MCD-1 CCP 2.1) command
// layer.
//
// NOTE ON CAN FD: CCP's frame layout was defined for classic CAN
// (fixed 8-byte payload). This project deliberately runs CCP over
// CAN FD anyway (a project-specific choice, not the standard
// pairing - the standard FD-aware successor to CCP is XCP). Frames
// are still built as classic 8-byte CCP command/response bytes; CAN
// FD's larger payload capacity is simply unused for CCP traffic. If
// this project later needs XCP for CAN FD, it belongs in its own
// separate protocol layer alongside this one, not as a replacement.
//
// This is a skeleton: enough command structure to build/parse the
// core CONNECT / GET_SEED / UNLOCK / DISCONNECT sequence and stand in
// for DNLOAD (flash) and SET_MTA. It is not a complete CCP 2.1
// implementation.
namespace Ccp {

// Command Receive Object (CRO) command codes, as sent by the master
// (this application) to the ECU (slave).
enum class Command : quint8 {
    Connect     = 0x01,
    SetMta      = 0x02,
    Dnload      = 0x03, // used for firmware/calibration download
    Upload      = 0x04,
    GetSeed     = 0x12,
    Unlock      = 0x17,
    Disconnect  = 0x07,
};

// Command Return Message (CRM) status byte, as sent back by the ECU.
enum class ReturnStatus : quint8 {
    Acknowledge = 0xFF,
    Error       = 0xFE,
};

// CRO frames are built as: [command][counter][...command-specific...],
// zero-padded to 8 bytes to match classic CCP framing (see note above
// about running this over CAN FD).
QByteArray buildConnect(quint8 counter, quint16 stationAddress);
QByteArray buildGetSeed(quint8 counter);
QByteArray buildUnlock(quint8 counter, const QByteArray &key);
QByteArray buildSetMta(quint8 counter, quint32 address);
QByteArray buildDnload(quint8 counter, const QByteArray &payload);
QByteArray buildDisconnect(quint8 counter, quint16 stationAddress);

struct ParsedResponse {
    bool valid = false;
    ReturnStatus status = ReturnStatus::Error;
    quint8 counter = 0;
    QByteArray payload; // remaining bytes after status+counter
};

ParsedResponse parseResponse(const QByteArray &frame);

} // namespace Ccp
