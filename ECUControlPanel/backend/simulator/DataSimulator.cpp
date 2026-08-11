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
        static_cast<char>((value >> 8) & 0xFF);

    data[address + 1] =
        static_cast<char>(value & 0xFF);
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
// 100 BYTE ECU DATA
// =========================================================

QByteArray DataSimulator::generateData()
{
    QByteArray data(100, '\0');

    m_tick++;


    // =========================================================
    // RPM
    // Excel:
    // Address = 0
    // Offset  = 0
    // Width   = 2
    // Type    = Unsigned
    // Formula = x * 0.1
    // =========================================================

    quint16 rpm =
        static_cast<quint16>(
            50000 +
            10000 * qSin(m_tick * 0.15)
            );

    writeUInt16(
        data,
        0,
        rpm
        );


    // =========================================================
    // TEMPERATURE
    // Excel:
    // Address = 2
    // Offset  = 0
    // Width   = 2
    // Type    = Signed
    // =========================================================

    qint16 temperature =
        static_cast<qint16>(
            800 +
            100 * qSin(m_tick * 0.10)
            );

    writeInt16(
        data,
        2,
        temperature
        );


    // =========================================================
    // VOLTAGE
    // Excel:
    // Address = 4
    // Offset  = 2
    // Width   = 2
    // Type    = Unsigned
    // Formula = x * 0.01
    //
    // Gerçek veri adresi:
    // 4 + 2 = 6
    // =========================================================

    quint16 voltage =
        static_cast<quint16>(
            1500 +
            50 * qSin(m_tick * 0.12)
            );

    writeUInt16(
        data,
        6,
        voltage
        );


    // =========================================================
    // STATUS
    // Address = 8
    // Width = 1
    // =========================================================

    data[8] = 1;


    // =========================================================
    // COUNTER
    // Address = 9
    // Width = 1
    // =========================================================

    data[9] =
        static_cast<char>(
            m_tick & 0xFF
            );


    // =========================================================
    // CURRENT
    // Excel:
    // Address = 10
    // Offset  = 1
    // Width   = 2
    // Type    = Unsigned
    // Formula = x * 0.01 (A)
    //
    // Gerçek veri adresi:
    // 10 + 1 = 11
    // =========================================================

    double current =
        20.0 +
        5.0 * qSin(m_tick * 0.18);

    quint16 currentRaw =
        static_cast<quint16>(
            current * 100.0
            );

    writeUInt16(
        data,
        11,
        currentRaw
        );


    // =========================================================
    // POWER
    // Address = 14
    // Width = 4
    // Type = Float
    // Formula = x * 0.1
    // =========================================================

    float power =
        300.0f +
        50.0f * qSin(m_tick * 0.15);

    writeFloat(
        data,
        14,
        power
        );


    // =========================================================
    // BUS VOLTAGE
    // Address = 18
    // Width = 2
    // Type = Unsigned
    // Formula = x * 0.1
    // =========================================================

    quint16 busVoltage =
        static_cast<quint16>(
            4620 +
            100 * qSin(m_tick * 0.08)
            );

    writeUInt16(
        data,
        18,
        busVoltage
        );


    // =========================================================
    // PHASE A CURRENT
    // Address = 20
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 phaseA =
        static_cast<qint16>(
            5000 +
            500 * qSin(m_tick * 0.11)
            );

    writeInt16(
        data,
        20,
        phaseA
        );


    // =========================================================
    // PHASE B CURRENT
    // Address = 22
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 phaseB =
        static_cast<qint16>(
            5500 +
            500 * qSin(
                m_tick * 0.11 + 1.0
                )
            );

    writeInt16(
        data,
        22,
        phaseB
        );


    // =========================================================
    // PHASE C CURRENT
    // Address = 24
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 phaseC =
        static_cast<qint16>(
            6000 +
            500 * qSin(
                m_tick * 0.11 + 2.0
                )
            );

    writeInt16(
        data,
        24,
        phaseC
        );


    // =========================================================
    // MOTOR TEMP
    // Address = 26
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 motorTemp =
        static_cast<qint16>(
            650 +
            80 * qSin(m_tick * 0.07)
            );

    writeInt16(
        data,
        26,
        motorTemp
        );


    // =========================================================
    // INVERTER TEMP
    // Address = 28
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 inverterTemp =
        static_cast<qint16>(
            700 +
            70 * qSin(m_tick * 0.06)
            );

    writeInt16(
        data,
        28,
        inverterTemp
        );


    // =========================================================
    // BATTERY MAX TEMP
    // Address = 30
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 batteryTemp =
        static_cast<qint16>(
            750 +
            50 * qSin(m_tick * 0.05)
            );

    writeInt16(
        data,
        30,
        batteryTemp
        );


    // =========================================================
    // AMBIENT TEMP
    // Address = 32
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 ambientTemp =
        static_cast<qint16>(
            250 +
            30 * qSin(m_tick * 0.04)
            );

    writeInt16(
        data,
        32,
        ambientTemp
        );


    // =========================================================
    // PCB TEMP
    // Address = 34
    // Width = 2
    // Type = Signed
    // =========================================================

    qint16 pcbTemp =
        static_cast<qint16>(
            500 +
            60 * qSin(m_tick * 0.06)
            );

    writeInt16(
        data,
        34,
        pcbTemp
        );


    // =========================================================
    // TARGET RPM
    // Address = 36
    // Width = 4
    // Type = Unsigned
    // =========================================================

    quint32 targetRpm =
        static_cast<quint32>(
            rpm + 1000
            );

    writeUInt32(
        data,
        36,
        targetRpm
        );


    // =========================================================
    // ACCELERATION
    // Address = 40
    // Width = 4
    // Type = Float
    // Formula = x * 0.01
    // =========================================================

    float acceleration =
        2.0f +
        1.0f * qSin(
            m_tick * 0.20
            );

    writeFloat(
        data,
        40,
        acceleration
        );


    // =========================================================
    // TORQUE DEMAND
    // Address = 44
    // Width = 4
    // Type = Float
    // =========================================================

    float torqueDemand =
        50.0f +
        10.0f * qSin(
            m_tick * 0.13
            );

    writeFloat(
        data,
        44,
        torqueDemand
        );


    // =========================================================
    // ACTUAL TORQUE
    // Address = 48
    // Width = 4
    // Type = Float
    // =========================================================

    float actualTorque =
        48.0f +
        8.0f * qSin(
            m_tick * 0.13 + 0.5
            );

    writeFloat(
        data,
        48,
        actualTorque
        );


    // =========================================================
    // ENCODER POSITION
    // Address = 52
    // Width = 4
    // Type = Unsigned
    // =========================================================

    quint32 encoderPosition =
        static_cast<quint32>(
            m_tick * 100
            );

    writeUInt32(
        data,
        52,
        encoderPosition
        );


    // =========================================================
    // FAULT FLAGS
    // Address = 56
    // Width = 4
    // Bitmask
    // =========================================================

    writeUInt32(
        data,
        56,
        0
        );


    // =========================================================
    // WARNING FLAGS
    // Address = 60
    // Width = 4
    // Bitmask
    // =========================================================

    writeUInt32(
        data,
        60,
        0
        );


    // =========================================================
    // ERROR COUNTER
    // Address = 64
    // Width = 2
    // Type = Unsigned
    // =========================================================

    writeUInt16(
        data,
        64,
        0
        );


    // =========================================================
    // SYSTEM UPTIME
    // Address = 66
    // Width = 4
    // Type = Unsigned
    // Formula = x (ms)
    // =========================================================

    writeUInt32(
        data,
        66,
        static_cast<quint32>(
            m_tick / 2
            )
        );


    // =========================================================
    // OPERATING MODE
    // Address = 70
    // Width = 1
    // Enum
    // =========================================================

    data[70] = 1;


    // =========================================================
    // STATE MACHINE STATE
    // Address = 71
    // Width = 1
    // Enum
    // =========================================================

    data[71] = 2;


    // =========================================================
    // HEARTBEAT
    // Address = 72
    // Width = 2
    // Type = Unsigned
    // =========================================================

    writeUInt16(
        data,
        72,
        static_cast<quint16>(
            m_tick
            )
        );


    // =========================================================
    // FIRMWARE VERSION
    // Address = 74
    // Width = 4
    // Major.Minor.Patch
    // =========================================================

    writeUInt32(
        data,
        74,
        10001
        );


    // =========================================================
    // HARDWARE REVISION
    // Address = 78
    // Width = 2
    // Type = Unsigned
    // =========================================================

    writeUInt16(
        data,
        78,
        101
        );


    // =========================================================
    // STATE OF CHARGE
    // Address = 80
    // Width = 2
    // Type = Unsigned
    // Formula = x * 0.1
    // =========================================================

    quint16 soc =
        static_cast<quint16>(
            800 +
            20 * qSin(
                m_tick * 0.02
                )
            );

    writeUInt16(
        data,
        80,
        soc
        );


    // =========================================================
    // STATE OF HEALTH
    // Address = 82
    // Width = 2
    // Type = Unsigned
    // Formula = x * 0.1
    // =========================================================

    quint16 soh =
        static_cast<quint16>(
            950 +
            5 * qSin(
                m_tick * 0.015
                )
            );

    writeUInt16(
        data,
        82,
        soh
        );


    // =========================================================
    // CELL 1 VOLTAGE
    // Address = 84
    // Width = 2
    // Type = Unsigned
    // Formula = x * 0.001
    // =========================================================

    quint16 cell1 =
        static_cast<quint16>(
            3500 +
            30 * qSin(
                m_tick * 0.08
                )
            );

    writeUInt16(
        data,
        84,
        cell1
        );


    // =========================================================
    // CELL 2 VOLTAGE
    // Address = 86
    // Width = 2
    // Type = Unsigned
    // Formula = x * 0.001
    // =========================================================

    quint16 cell2 =
        static_cast<quint16>(
            3510 +
            30 * qSin(
                m_tick * 0.08 + 0.5
                )
            );

    writeUInt16(
        data,
        86,
        cell2
        );


    // =========================================================
    // CELL 3 VOLTAGE
    // Address = 88
    // Width = 2
    // Type = Unsigned
    // Formula = x * 0.001
    // =========================================================

    quint16 cell3 =
        static_cast<quint16>(
            3490 +
            30 * qSin(
                m_tick * 0.08 + 1.0
                )
            );

    writeUInt16(
        data,
        88,
        cell3
        );


    // =========================================================
    // CELL 4 VOLTAGE
    // Address = 90
    // Width = 2
    // Type = Unsigned
    // Formula = x * 0.001
    // =========================================================

    quint16 cell4 =
        static_cast<quint16>(
            3505 +
            30 * qSin(
                m_tick * 0.08 + 1.5
                )
            );

    writeUInt16(
        data,
        90,
        cell4
        );


    // =========================================================
    // CRC / CHECKSUM
    // Address = 92
    // Width = 4
    // =========================================================

    writeUInt32(
        data,
        92,
        0
        );


    // =========================================================
    // RESERVED / BUFFER
    // Address = 96
    // Width = 4
    // =========================================================

    writeUInt32(
        data,
        96,
        0
        );


    qDebug()
        << "DataSimulator:"
        << "100-byte packet"
        << "| RPM:" << rpm
        << "| Temperature:" << temperature
        << "| Voltage:" << voltage
        << "| Current:" << current;


    return data;
}