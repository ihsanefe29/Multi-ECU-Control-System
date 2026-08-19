#include "datasimulator.h"
#include <QDebug>
#include <QtMath>
#include <cstring>
#include <QDataStream>

// Paket yapısı — Esra'nın PacketBuilder'ıyla tam uyumlu (16 byte):
//
//  Byte  0-1  : Header 0xAA 0x55
//  Byte  2-3  : N1          (int16 LE, stepSize=1  → raw = fiziksel değer * 1)
//  Byte  4-5  : EGT         (int16 LE, stepSize=1  → raw = °C)
//  Byte  6-7  : Fuel Flow   (int16 LE, stepSize=1  → raw = kg/h)
//  Byte  8-9  : Bleed Valve (int16 LE, enum: 0=Choose/1=Closed/2=Partial/3=Open)
//  Byte 10-13 : Vibration   (float  LE, stepSize=0.1 → appendFloat)
//  Byte 14-15 : Checksum    (uint16 LE, 16-bit XOR of all preceding bytes)

DataSimulator::DataSimulator(QObject *parent)
    : QObject(parent), m_tick(0)
{
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        emit dataReady(generateData());
    });
    m_timer.start(500);
}

// ── Yardımcı: int16 little-endian ─────────────────────────────────────────
static void appendInt16LE(QByteArray &pkt, qint16 value)
{
    QDataStream s(&pkt, QIODevice::Append);
    s.setByteOrder(QDataStream::LittleEndian);
    s << value;
}

// ── Yardımcı: float little-endian (single precision) ──────────────────────
static void appendFloatLE(QByteArray &pkt, float value)
{
    QDataStream s(&pkt, QIODevice::Append);
    s.setByteOrder(QDataStream::LittleEndian);
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);
    s << value;
}

// ── Paket üretici ─────────────────────────────────────────────────────────
QByteArray DataSimulator::generateData()
{
    m_tick++;

    QByteArray packet;
    packet.reserve(16);

    // ── Header ────────────────────────────────────────────────────────────
    packet.append(static_cast<char>(0xAA));
    packet.append(static_cast<char>(0x55));

    // ── N1 (%0-105, stepSize=1, int16) ────────────────────────────────────
    double n1 = 60.0 + 20.0 * qSin(m_tick * 0.10);
    appendInt16LE(packet, static_cast<qint16>(qRound(n1)));

    // ── EGT (300-950 °C, stepSize=1, int16) ───────────────────────────────
    double egt = 600.0 + 150.0 * qSin(m_tick * 0.08);
    appendInt16LE(packet, static_cast<qint16>(qRound(egt)));

    // ── Fuel Flow (200-2500 kg/h, stepSize=1, int16) ──────────────────────
    double fuelFlow = 1200.0 + 500.0 * qSin(m_tick * 0.06);
    appendInt16LE(packet, static_cast<qint16>(qRound(fuelFlow)));

    // ── Bleed Valve (enum int16: 1=Closed, 2=Partial, 3=Open) ─────────────
    // PacketBuilder enum listesi: ["Choose..", "Closed", "Partial", "Open"]
    // → index 0=Choose, 1=Closed, 2=Partial, 3=Open
    qint16 bleedValve = static_cast<qint16>((m_tick % 3) + 1); // 1-2-3 döngüsü
    appendInt16LE(packet, bleedValve);

    // ── Vibration (0.0-5.0, stepSize=0.1 → float 4 byte) ─────────────────
    float vibration = static_cast<float>(2.5 + 1.0 * qSin(m_tick * 0.12));
    appendFloatLE(packet, vibration);

    // ── Checksum (16-bit XOR, PacketBuilder ile aynı algoritma) ───────────
    quint16 checksum = 0;
    for (int i = 0; i < packet.size(); i += 2) {
        quint16 word;
        if (i + 1 < packet.size())
            word = static_cast<quint8>(packet[i]) | (static_cast<quint8>(packet[i+1]) << 8);
        else
            word = static_cast<quint8>(packet[i]);
        checksum ^= word;
    }
    appendInt16LE(packet, static_cast<qint16>(checksum));

    // Toplam: 2 + 2 + 2 + 2 + 2 + 4 + 2 = 16 byte
    Q_ASSERT(packet.size() == 16);

    qDebug() << "DataSimulator 16-byte:"
             << "N1=" << n1
             << "EGT=" << qRound(egt)
             << "FuelFlow=" << qRound(fuelFlow)
             << "BleedValve=" << bleedValve
             << "Vibration=" << vibration;

    return packet;
}
