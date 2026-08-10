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

    // Zaman ilerliyor.
    m_tick++;


    // ---------------------------------------------------------
    // RPM
    // Address: 0
    // Width: 4
    // Type: Unsigned
    // ---------------------------------------------------------

    quint32 rpm =
        static_cast<quint32>(
            65000 +
            5000 * qSin(m_tick * 0.15)
            );

    writeUInt32(
        data,
        0,
        rpm
        );


    // ---------------------------------------------------------
    // Temperature
    // Address: 4
    // Width: 2
    // Type: Signed
    // ---------------------------------------------------------

    qint16 temperature =
        static_cast<qint16>(
            800 +
            100 * qSin(m_tick * 0.10)
            );

    writeInt16(
        data,
        4,
        temperature
        );


    // ---------------------------------------------------------
    // Voltage
    // Address: 6
    // Width: 2
    // Type: Unsigned
    // Conversion: x * 0.1
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Status
    // Address: 8
    // ---------------------------------------------------------

    data[8] = 1;


    // ---------------------------------------------------------
    // Counter
    // Address: 9
    // ---------------------------------------------------------

    data[9] =
        static_cast<char>(m_tick & 0xFF);


    // ---------------------------------------------------------
    // Current
    // Address: 10
    // Width: 4
    // Float
    // ---------------------------------------------------------

    float current =
        20.0f +
        5.0f * qSin(m_tick * 0.18);

    writeFloat(
        data,
        10,
        current
        );


    // ---------------------------------------------------------
    // Power
    // Address: 14
    // Width: 4
    // Float
    // ---------------------------------------------------------

    float power =
        300.0f +
        50.0f * qSin(m_tick * 0.15);

    writeFloat(
        data,
        14,
        power
        );


    // ---------------------------------------------------------
    // Bus Voltage
    // Address: 18
    // Width: 2
    // Conversion: x * 0.1
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Phase A Current
    // Address: 20
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Phase B Current
    // Address: 22
    // ---------------------------------------------------------

    qint16 phaseB =
        static_cast<qint16>(
            5500 +
            500 * qSin(m_tick * 0.11 + 1.0)
            );

    writeInt16(
        data,
        22,
        phaseB
        );


    // ---------------------------------------------------------
    // Phase C Current
    // Address: 24
    // ---------------------------------------------------------

    qint16 phaseC =
        static_cast<qint16>(
            6000 +
            500 * qSin(m_tick * 0.11 + 2.0)
            );

    writeInt16(
        data,
        24,
        phaseC
        );


    // ---------------------------------------------------------
    // Motor Temperature
    // Address: 26
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Inverter Temperature
    // Address: 28
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Battery Max Temp
    // Address: 30
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Ambient Temp
    // Address: 32
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // PCB Temp
    // Address: 34
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Target RPM
    // Address: 36
    // ---------------------------------------------------------

    quint32 targetRpm =
        rpm + 1000;

    writeUInt32(
        data,
        36,
        targetRpm
        );


    // ---------------------------------------------------------
    // Acceleration
    // Address: 40
    // ---------------------------------------------------------

    float acceleration =
        2.0f +
        1.0f * qSin(m_tick * 0.20);

    writeFloat(
        data,
        40,
        acceleration
        );


    // ---------------------------------------------------------
    // Torque Demand
    // Address: 44
    // ---------------------------------------------------------

    float torqueDemand =
        50.0f +
        10.0f * qSin(m_tick * 0.13);

    writeFloat(
        data,
        44,
        torqueDemand
        );


    // ---------------------------------------------------------
    // Actual Torque
    // Address: 48
    // ---------------------------------------------------------

    float actualTorque =
        48.0f +
        8.0f * qSin(m_tick * 0.13 + 0.5);

    writeFloat(
        data,
        48,
        actualTorque
        );


    // ---------------------------------------------------------
    // Encoder Position
    // Address: 52
    // ---------------------------------------------------------

    quint32 encoderPosition =
        static_cast<quint32>(
            m_tick * 100
            );

    writeUInt32(
        data,
        52,
        encoderPosition
        );


    // ---------------------------------------------------------
    // Fault Flags
    // Address: 56
    // ---------------------------------------------------------

    writeUInt32(
        data,
        56,
        0
        );


    // ---------------------------------------------------------
    // Warning Flags
    // Address: 60
    // ---------------------------------------------------------

    writeUInt32(
        data,
        60,
        0
        );


    // ---------------------------------------------------------
    // Error Counter
    // Address: 64
    // ---------------------------------------------------------

    writeUInt16(
        data,
        64,
        0
        );


    // ---------------------------------------------------------
    // System Uptime
    // Address: 66
    // ---------------------------------------------------------

    writeUInt32(
        data,
        66,
        static_cast<quint32>(m_tick / 2)
        );


    // ---------------------------------------------------------
    // Operating Mode
    // Address: 70
    // ---------------------------------------------------------

    data[70] = 1;


    // ---------------------------------------------------------
    // State Machine State
    // Address: 71
    // ---------------------------------------------------------

    data[71] = 2;


    // ---------------------------------------------------------
    // Heartbeat
    // Address: 72
    // ---------------------------------------------------------

    writeUInt16(
        data,
        72,
        static_cast<quint16>(m_tick)
        );


    // ---------------------------------------------------------
    // Firmware Version
    // Address: 74
    // ---------------------------------------------------------

    writeUInt32(
        data,
        74,
        10001
        );


    // ---------------------------------------------------------
    // Hardware Revision
    // Address: 78
    // ---------------------------------------------------------

    writeUInt16(
        data,
        78,
        101
        );


    // ---------------------------------------------------------
    // SOC
    // Address: 80
    // Conversion: x * 0.1
    // ---------------------------------------------------------

    quint16 soc =
        static_cast<quint16>(
            800 +
            20 * qSin(m_tick * 0.02)
            );

    writeUInt16(
        data,
        80,
        soc
        );


    // ---------------------------------------------------------
    // SOH
    // Address: 82
    // Conversion: x * 0.1
    // ---------------------------------------------------------

    quint16 soh =
        static_cast<quint16>(
            950 +
            5 * qSin(m_tick * 0.015)
            );

    writeUInt16(
        data,
        82,
        soh
        );


    // ---------------------------------------------------------
    // Cell Voltages
    // Conversion: x * 0.001
    // ---------------------------------------------------------

    quint16 cell1 =
        static_cast<quint16>(
            3500 +
            30 * qSin(m_tick * 0.08)
            );

    quint16 cell2 =
        static_cast<quint16>(
            3510 +
            30 * qSin(m_tick * 0.08 + 0.5)
            );

    quint16 cell3 =
        static_cast<quint16>(
            3490 +
            30 * qSin(m_tick * 0.08 + 1.0)
            );

    quint16 cell4 =
        static_cast<quint16>(
            3505 +
            30 * qSin(m_tick * 0.08 + 1.5)
            );

    writeUInt16(data, 84, cell1);
    writeUInt16(data, 86, cell2);
    writeUInt16(data, 88, cell3);
    writeUInt16(data, 90, cell4);


    // ---------------------------------------------------------
    // CRC / Checksum
    // ---------------------------------------------------------

    writeUInt32(
        data,
        92,
        0
        );


    // ---------------------------------------------------------
    // Reserved / Buffer
    // ---------------------------------------------------------

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