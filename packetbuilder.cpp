#include "packetbuilder.h"
#include <QDataStream>
#include <QDebug>

PacketBuilder::PacketBuilder(QObject *parent)
    : QObject(parent)
{
}

void PacketBuilder::appendInt16(QByteArray &packet, qint16 value)
{
    QDataStream stream(&packet, QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << value;
}

void PacketBuilder::appendFloat(QByteArray &packet, float value)
{
    QDataStream stream(&packet, QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision); // varsayılan DoublePrecision, float'ı bile 8 byte yazar
    stream << value;
}

void PacketBuilder::setEnumOptions(int paramIndex, const QStringList &options)
{
    m_enumOptions[paramIndex] = options;
}

void PacketBuilder::setStepSize(int paramIndex, double stepSize)
{
    m_stepSizes[paramIndex] = stepSize;
}

QByteArray PacketBuilder::buildPacket(const QVariantList &orderedValues)
{
    QByteArray packet;
    packet.append(static_cast<char>(0xAA));
    packet.append(static_cast<char>(0x55));

    for (int i = 0; i < orderedValues.size(); ++i) {
        const QVariant &val = orderedValues[i];
        double stepSize = m_stepSizes.value(i, 1.0);  // tanımlı değilse varsayılan: int davranışı

        if (stepSize < 1.0) {
            // Ondalıklı adım -> her zaman float (4 byte) olarak kodla.
            appendFloat(packet, val.toFloat());
        }
        else if (val.type() == QVariant::String) {
            // Enum parametresi -> 2 byte
            qint16 enumId = 0;
            if (m_enumOptions.contains(i)) {
                int idx = m_enumOptions[i].indexOf(val.toString());
                if (idx >= 0) {
                    enumId = static_cast<qint16>(idx);
                } else {
                    qWarning() << "Bilinmeyen enum değeri:" << val.toString() << "index:" << i;
                }
            }
            appendInt16(packet, enumId);
        }
        else {
            // Normal int parametre -> 2 byte
            appendInt16(packet, static_cast<qint16>(val.toInt()));
        }
    }

    // Checksum artık 2 byte: paketteki tüm byte'ların 16-bit XOR'u.
    // Header (0xAA, 0x55) dahil, checksum alanı hariç tüm byte'lar üzerinden hesaplanır.
    quint16 checksum = 0;
    for (int i = 0; i < packet.size(); i += 2) {
        quint16 word;
        if (i + 1 < packet.size()) {
            word = static_cast<quint8>(packet[i]) | (static_cast<quint8>(packet[i + 1]) << 8);
        } else {
            // tek sayıda byte kaldıysa (bu paket düzeninde olmaz ama savunma amaçlı)
            word = static_cast<quint8>(packet[i]);
        }
        checksum ^= word;
    }

    appendInt16(packet, static_cast<qint16>(checksum));
    return packet;
}