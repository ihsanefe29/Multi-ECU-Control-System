#ifndef DATASIMULATOR_H
#define DATASIMULATOR_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

class DataSimulator : public QObject
{
    Q_OBJECT

public:

    explicit DataSimulator(QObject *parent = nullptr);

signals:

    void dataReady(const QByteArray &data);

private:

    QByteArray generateData();

    void writeUInt16(
        QByteArray &data,
        int address,
        quint16 value
        );

    void writeInt16(
        QByteArray &data,
        int address,
        qint16 value
        );

    void writeUInt32(
        QByteArray &data,
        int address,
        quint32 value
        );

    void writeFloat(
        QByteArray &data,
        int address,
        float value
        );

private:

    QTimer m_timer;

    int m_tick;
};

#endif // DATASIMULATOR_H