#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "parametermodel.h"
#include "datacollector.h"
#include "packetbuilder.h"

#ifdef USE_MOCK_DISPATCHER
#include "mockdispatcher.h"
#else
#include "tcpdispatcher.h"   // ileride yazılacak
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ParameterModel paramModel;
    PacketBuilder packetBuilder;

#ifdef USE_MOCK_DISPATCHER
    MockDispatcher dispatcher;
    qDebug() << "[main] Mock dispatcher kullanılıyor";
#else
    TcpDispatcher dispatcher;
    qDebug() << "[main] Gerçek (TCP) dispatcher kullanılıyor";
#endif

    DataCollector collector(&paramModel, &packetBuilder);

    QObject::connect(&collector, &DataCollector::packetReady,
                     &dispatcher, &decltype(dispatcher)::send);

    QObject::connect(&paramModel, &ParameterModel::valueChanged,
                     [](const QString &name, const QVariant &value) {
                         qDebug() << "[C++] Parametre değişti:" << name << "=" << value;
                     });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("paramModel", &paramModel);
    engine.rootContext()->setContextProperty("dataCollector", &collector);
    engine.rootContext()->setContextProperty("packetBuilder", &packetBuilder);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    collector.start(1000);

    return app.exec();
}