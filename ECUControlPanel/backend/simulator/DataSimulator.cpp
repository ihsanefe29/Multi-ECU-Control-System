#include "DataSimulator.h"

#include <QDebug>
#include <QtMath>
#include <cstring>


DataSimulator::DataSimulator(QObject *parent)
    : QObject(parent),
    m_tick(0)
{
    connect(
        &m_timer,
        &QTimer::timeout,
        this,
        [this]()
        {
            QByteArray data = generateData();

            emit dataReady(data);
        }
        );

    // 500 ms'de bir yeni ECU paketi oluştur.
    m_timer.start(500);
}


// =========================================================
// UINT16
// =========================================================

void DataSimulator::writeUInt16(
    QByteArray &data,
    int address,
    quint16 value)
{
    data[address] =
        static_cast<char>(value & 0xFF);

    data[address + 1] =
        static_cast<char>((value >> 8) & 0xFF);
}


// =========================================================
// INT16
// =========================================================

void DataSimulator::writeInt16(
    QByteArray &data,
    int address,
    qint16 value)
{
    quint16 raw =
        static_cast<quint16>(value);

    data[address] =
        static_cast<char>((raw >> 8) & 0xFF);

    data[address + 1] =
        static_cast<char>(raw & 0xFF);
}


// =========================================================
// UINT32
// =========================================================

void DataSimulator::writeUInt32(
    QByteArray &data,
    int address,
    quint32 value)
{
    data[address] =
        static_cast<char>((value >> 24) & 0xFF);

    data[address + 1] =
        static_cast<char>((value >> 16) & 0xFF);

    data[address + 2] =
        static_cast<char>((value >> 8) & 0xFF);

    data[address + 3] =
        static_cast<char>(value & 0xFF);
}


// =========================================================
// FLOAT
// =========================================================

void DataSimulator::writeFloat(
    QByteArray &data,
    int address,
    float value)
{
    quint32 rawValue = 0;

    std::memcpy(
        &rawValue,
        &value,
        sizeof(float)
        );

    data[address] =
        static_cast<char>((rawValue >> 24) & 0xFF);

    data[address + 1] =
        static_cast<char>((rawValue >> 16) & 0xFF);

    data[address + 2] =
        static_cast<char>((rawValue >> 8) & 0xFF);

    data[address + 3] =
        static_cast<char>(rawValue & 0xFF);
}


// =========================================================
// ECU DATA
// =========================================================

QByteArray DataSimulator::generateData()
{
    // =========================================================
    // ECU PACKET
    //
    // Byte 0-1  : Header
    // Byte 2-11 : Payload (10 byte)
    // Byte 12   : Checksum
    //
    // Payload:
    //
    // 0-1   : N1
    // 2-3   : EGT
    // 4-5   : Fuel Flow
    // 6     : Bleed Valve Position
    // 7     : RESERVED / UNUSED
    // 8-9   : Vibration
    // =========================================================

    QByteArray packet(13, '\0');

    m_tick++;


    // =========================================================
    // HEADER
    // =========================================================

    packet[0] =
        static_cast<char>(0xAA);

    packet[1] =
        static_cast<char>(0x55);


    // =========================================================
    // PAYLOAD BAŞLANGICI
    //
    // Excel adresleri payload'a göre tutuluyor.
    // Header payload adreslerine dahil değil.
    // =========================================================

    const int payloadStart = 2;


    // =========================================================
    // N1
    //
    // Excel:
    // RAM_ADDR   = 0
    // DATA_WIDTH = 16
    // BIT_OFFSET = 0
    // DATA_TYPE  = uint16
    //
    // Aralık: %0 - %105
    //
    // Fiziksel değer:
    // 0.0 - 105.0
    //
    // Raw değer:
    // fiziksel değer * 10
    //
    // Örnek:
    // 75.0 % → 750
    // =========================================================

    double n1 =
        60.0 +
        20.0 * qSin(m_tick * 0.10);

    quint16 n1Raw =
        static_cast<quint16>(
            n1 * 10.0
            );

    writeUInt16(
        packet,
        payloadStart + 0,
        n1Raw
        );


    // =========================================================
    // EGT
    //
    // Excel:
    // RAM_ADDR   = 2
    // DATA_WIDTH = 16
    // BIT_OFFSET = 0
    // DATA_TYPE  = uint16
    //
    // Aralık: 300 - 950 °C
    //
    // Raw değer doğrudan °C.
    // =========================================================

    quint16 egt =
        static_cast<quint16>(
            600.0 +
            150.0 * qSin(m_tick * 0.08)
            );

    writeUInt16(
        packet,
        payloadStart + 2,
        egt
        );


    // =========================================================
    // FUEL FLOW
    //
    // Excel:
    // RAM_ADDR   = 4
    // DATA_WIDTH = 16
    // BIT_OFFSET = 0
    // DATA_TYPE  = uint16
    //
    // Aralık: 200 - 2500 kg/h
    //
    // Raw değer doğrudan değer.
    // =========================================================

    quint16 fuelFlow =
        static_cast<quint16>(
            1200.0 +
            500.0 * qSin(m_tick * 0.06)
            );

    writeUInt16(
        packet,
        payloadStart + 4,
        fuelFlow
        );


    // =========================================================
    // BLEED VALVE POSITION
    //
    // Excel:
    // RAM_ADDR   = 6
    // DATA_WIDTH = 8
    // BIT_OFFSET = 0
    // DATA_TYPE  = uint8
    //
    // Enum:
    // 0 = Closed
    // 1 = Partial
    // 2 = Open
    //
    // Sadece 1 byte kullanılıyor.
    // =========================================================

    quint8 bleedValve =
        static_cast<quint8>(
            m_tick % 3
            );

    packet[payloadStart + 6] =
        static_cast<char>(
            bleedValve
            );


    // =========================================================
    // BYTE 7
    //
    // Excel'de kullanılmayan boş alan.
    //
    // Bilerek herhangi bir parametre yazmıyoruz.
    // QByteArray başlangıçta 0 ile doldurulduğu için:
    //
    // payload byte 7 = 0
    //
    // =========================================================

    packet[payloadStart + 7] =
        static_cast<char>(0x00);


    // =========================================================
    // VIBRATION
    //
    // Excel:
    // RAM_ADDR   = 8
    // DATA_WIDTH = 16
    // BIT_OFFSET = 0
    // DATA_TYPE  = uint16
    // Formula    = x * 0.01
    //
    // Aralık: 0 - 5.00
    //
    // Örnek:
    // 2.50 → raw 250
    // =========================================================

    double vibration =
        2.5 +
        1.0 * qSin(m_tick * 0.12);

    quint16 vibrationRaw =
        static_cast<quint16>(
            vibration * 100.0
            );

    writeUInt16(
        packet,
        payloadStart + 8,
        vibrationRaw
        );


    // =========================================================
    // CHECKSUM
    //
    // Şimdilik checksum algoritması bilinmediği için 0 bırakılıyor.
    //
    // Arkadaşından checksum algoritmasını öğrendiğimizde
    // burayı gerçek hesaplama ile değiştireceğiz.
    // =========================================================

    packet[12] =
        static_cast<char>(0x00);


    // =========================================================
    // DEBUG
    // =========================================================

    qDebug()
        << "DataSimulator:"
        << "13-byte packet"
        << "| N1:" << n1
        << "| EGT:" << egt
        << "| Fuel Flow:" << fuelFlow
        << "| Bleed Valve:" << bleedValve
        << "| Vibration:" << vibration;


    return packet;
}