#pragma once

#include <QObject>
#include <QByteArray>
#include <QTimer>

// Sahte ECU CAN paketi üretici (aybuke branch).
// 500ms'de bir 13-byte'lık paket yayınlar:
//   Byte 0-1  : Header  (0xAA 0x55)
//   Byte 2-3  : N1      (uint16, x*0.1 → %)
//   Byte 4-5  : EGT     (uint16, °C)
//   Byte 6-7  : FuelFlow(uint16, kg/h)
//   Byte 8    : BleedValve (uint8 enum: 0=Closed,1=Partial,2=Open)
//   Byte 9    : RESERVED
//   Byte 10-11: Vibration (uint16, x*0.01)
//   Byte 12   : Checksum (şimdilik 0)
class DataSimulator : public QObject
{
    Q_OBJECT

public:
    explicit DataSimulator(QObject *parent = nullptr);

signals:
    void dataReady(const QByteArray &data);

private:
    QByteArray generateData();

    void writeUInt16(QByteArray &data, int address, quint16 value);
    void writeInt16 (QByteArray &data, int address, qint16  value);
    void writeUInt32(QByteArray &data, int address, quint32 value);
    void writeFloat (QByteArray &data, int address, float   value);

    QTimer m_timer;
    int    m_tick = 0;
};
