#ifndef PACKETBUILDER_H
#define PACKETBUILDER_H

#include <QObject>
#include <QByteArray>
#include <QVariantList>
#include <QMap>
#include <QStringList>

class PacketBuilder : public QObject
{
    Q_OBJECT
public:
    explicit PacketBuilder(QObject *parent = nullptr);

    QByteArray buildPacket(const QVariantList &orderedValues);

    Q_INVOKABLE void setEnumOptions(int paramIndex, const QStringList &options);
    Q_INVOKABLE void setStepSize(int paramIndex, double stepSize);

private:
    void appendInt16(QByteArray &packet, qint16 value);
    void appendFloat(QByteArray &packet, float value);

    QMap<int, QStringList> m_enumOptions;
    QMap<int, double> m_stepSizes;   // index -> stepSize (tanımsızsa varsayılan 1.0 = int davranışı)
};

#endif // PACKETBUILDER_H