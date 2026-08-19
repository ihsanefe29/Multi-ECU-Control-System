#include "CommunicationCore.h"
#include <QIODevice>
#include <QCoreApplication>
#include <QDebug>
#include <QSet>
#include <QSocketNotifier>
#include <QTextStream>
#include <QTimer>

#include <cstdio>

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    qRegisterMetaType<CanFrame>("CanFrame");

    CommunicationCore core;

    bool heartbeatPrinting = false;
    QSet<char> heartbeatFilters;
    QSet<char> discoveredEcus;

    QObject::connect(&core, &CommunicationCore::channelDiscovered,
                     [](const QString &channel, bool fd) {
        qInfo() << "CAN interface discovered:" << channel << "CAN FD:" << fd;
    });

    QObject::connect(&core, &CommunicationCore::channelConnected,
                     [](const QString &channel) {
        qInfo() << "CAN interface connected:" << channel;
    });

    QObject::connect(&core, &CommunicationCore::ecuDiscovered,
                     [&](char ecu, const QString &channel, quint32 commandId) {
        ecu = QChar::fromLatin1(ecu).toUpper().toLatin1();
        discoveredEcus.insert(ecu);

        qInfo() << "ECU" << ecu << "discovered on" << channel
                << "command ID" << Qt::hex << commandId << Qt::dec;
    });

    QObject::connect(&core, &CommunicationCore::ecuDiscoveryCompleted,
                     &core, [&](int count) {
        qInfo() << "ECU discovery completed; found:" << count;

        for (char ecu : discoveredEcus)
            core.requestStatus(ecu);
    });

    QObject::connect(&core, &CommunicationCore::ecuOnlineChanged,
                     [](char ecu, bool online) {
        qInfo() << "ECU" << ecu
                << (online ? "ONLINE" : "OFFLINE (heartbeat timeout)");
    });

    QObject::connect(&core, &CommunicationCore::ecuHeartbeat,
                     [&](char ecu, quint8 counter, quint8 state) {
        ecu = QChar::fromLatin1(ecu).toUpper().toLatin1();

        if (heartbeatPrinting &&
            (heartbeatFilters.isEmpty() || heartbeatFilters.contains(ecu)) &&
            counter % 10 == 0) {

            qInfo() << "Heartbeat ECU" << ecu
                    << "counter:" << counter
                    << "power state:" << state;
        }
    });

    QObject::connect(&core, &CommunicationCore::ecuStatus,
                     [](char ecu, quint8 state, quint8 result) {
        const char *states[] = {"OFF", "ON", "KILL"};

        const QString stateText =
            state < 3 ? QString::fromLatin1(states[state])
                      : QStringLiteral("UNKNOWN");

        qInfo() << "ECU" << ecu
                << "status:" << stateText
                << "result:" << result;
    });

    QObject::connect(&core, &CommunicationCore::commandSent,
                     [](char ecu, quint8 command) {
        qInfo() << "TX command ECU" << ecu
                << "value" << Qt::hex << command << Qt::dec;
    });
/*
    QObject::connect(&core, &CommunicationCore::frameReceived,
                     [](const CanFrame &frame) {
        qInfo().noquote()
            << QStringLiteral("RX %1 ID=%2 %3 DATA=%4")
                   .arg(frame.channel)
                   .arg(frame.id, 3, 16, QChar('0'))
                   .arg(frame.isFd ? QStringLiteral("FD")
                                   : QStringLiteral("Classic"))
                   .arg(QString::fromLatin1(
                       frame.payload.toHex().toUpper()));
    });
    */

    QObject::connect(&core, &CommunicationCore::errorOccurred,
                     [](const QString &description) {
        qWarning().noquote() << description;
    });

    QObject::connect(&application, &QCoreApplication::aboutToQuit,
                     &core, &CommunicationCore::stop);

    QTextStream terminal(stdin, QIODevice::ReadOnly);
    QSocketNotifier inputNotifier(fileno(stdin),
                                  QSocketNotifier::Read,
                                  &application);

    QObject::connect(&inputNotifier, &QSocketNotifier::activated,
                     &application,
                     [&](QSocketDescriptor, QSocketNotifier::Type) {
        const QStringList fields =
            terminal.readLine().trimmed().split(' ', Qt::SkipEmptyParts);

        if (fields.isEmpty())
            return;

        // quit
        if (fields.size() == 1 &&
            fields[0].compare("quit", Qt::CaseInsensitive) == 0) {

            application.quit();
            return;
        }

        // stop
        if (fields.size() == 1 &&
            fields[0].compare("stop", Qt::CaseInsensitive) == 0) {

            heartbeatPrinting = false;
            heartbeatFilters.clear();

            qInfo() << "Heartbeat printing stopped";
            return;
        }

        // heartbeat, heartbeat A veya heartbeat A B C
        if (fields[0].compare("heartbeat", Qt::CaseInsensitive) == 0) {
            heartbeatPrinting = true;
            heartbeatFilters.clear();

            for (qsizetype i = 1; i < fields.size(); ++i) {
                if (fields[i].size() != 1)
                    continue;

                const char ecu =
                    fields[i].toUpper().at(0).toLatin1();

                if (discoveredEcus.contains(ecu))
                    heartbeatFilters.insert(ecu);
            }

            if (discoveredEcus.isEmpty()) {
                heartbeatPrinting = false;
                qWarning() << "No ECU discovered";
            } else if (heartbeatFilters.isEmpty()) {
                qInfo() << "Printing all discovered ECU heartbeats";
            } else {
                qInfo() << "Printing selected ECU heartbeats:"
                        << heartbeatFilters;
            }

            return;
        }

        if (fields.size() != 2 || fields[1].size() != 1) {
            qInfo() << "Usage: heartbeat [A B ...] | stop | "
                       "on A | off A | status A | quit";
            return;
        }

        const char ecu = fields[1].toUpper().at(0).toLatin1();

        if (!discoveredEcus.contains(ecu)) {
            qWarning() << "ECU" << ecu << "not discovered";
            return;
        }

        if (fields[0].compare("on", Qt::CaseInsensitive) == 0)
            core.powerOn(ecu);
        else if (fields[0].compare("off", Qt::CaseInsensitive) == 0)
            core.powerOff(ecu);
        else if (fields[0].compare("status", Qt::CaseInsensitive) == 0)
            core.requestStatus(ecu);
        else
            qInfo() << "Usage: heartbeat [A B ...] | stop | "
                       "on A | off A | status A | quit";
    });

    qInfo() << "Commands: heartbeat [A B ...] | stop | "
               "on A | off A | status A | quit";

    QTimer::singleShot(0, &core, &CommunicationCore::start);
    return application.exec();
}