#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QLocale>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QTimer>

#include "backend/collector/UICollector.h"
#include "backend/dispatcher/Dispatcher.h"
#include "backend/manager/ECUManager.h"
#include "backend/parser/ExcelParser.h"
#include "backend/parser/RawDataParser.h"
#include "backend/parser/RawDataReader.h"
#include "backend/simulator/DataSimulator.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    QTranslator translator;

    const QStringList uiLanguages = QLocale::system().uiLanguages();

    for (const QString &locale : uiLanguages)
    {
        const QString baseName =
            "ECUControlPanel_" + QLocale(locale).name();

        if (translator.load(":/i18n/" + baseName))
        {
            app.installTranslator(&translator);
            break;
        }
    }

    UICollector collector;
    Dispatcher dispatcher;
    ECUManager manager;
    ExcelParser excelParser;
    RawDataParser rawDataParser;
    RawDataReader rawDataReader;
    DataSimulator dataSimulator;

    // Excel dosyasının proje içerisindeki konumu
    QString excelPath =
        QCoreApplication::applicationDirPath()
        + "/../../../data/ECU_Data.xlsx";

    qDebug() << "Excel path:" << excelPath;
    qDebug() << "Excel exists:" << QFileInfo::exists(excelPath);

    QString rawDataPath =
        QCoreApplication::applicationDirPath()
        + "/../../../data/raw_data.txt";

    qDebug() << "Raw data path:" << rawDataPath;
    qDebug() << "Raw data exists:" << QFileInfo::exists(rawDataPath);

    // Excel dosyasını oku
    excelParser.loadFile(excelPath);

    QVector<Parameter> parameters =
        excelParser.parameters();

    // Parser'a Excel'den gelen parametre tanımlarını ver
    rawDataParser.setParameters(parameters);

    QObject::connect(
        &collector,
        &UICollector::messageCollected,
        &dispatcher,
        &Dispatcher::dispatch
        );

    dispatcher.setManager(&manager);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(
        "uiCollector",
        &collector
        );

    engine.rootContext()->setContextProperty(
        "ecuManager",
        &manager
        );

    engine.rootContext()->setContextProperty(
        "rawDataParser",
        &rawDataParser
        );

    const QUrl url(
        QStringLiteral("qrc:/qml/main.qml")
        );

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && url == objUrl)
            {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection
        );

    QObject::connect(
        &dataSimulator,
        &DataSimulator::dataReady,
        [&](const QByteArray &rawData)
        {
            // Paket:
            // [0-1]  Header
            // [2-11] Payload
            // [12]   Checksum

            if (rawData.size() < 3)
            {
                qDebug()
                << "Main: Invalid packet size:"
                << rawData.size();

                return;
            }


            // ----------------------------------------------------
            // HEADER KONTROLÜ
            // ----------------------------------------------------

            const quint8 header1 =
                static_cast<quint8>(
                    rawData.at(0)
                    );

            const quint8 header2 =
                static_cast<quint8>(
                    rawData.at(1)
                    );


            if (header1 != 0xAA ||
                header2 != 0x55)
            {
                qDebug()
                << "Main: Invalid header.";

                return;
            }


            // ----------------------------------------------------
            // CHECKSUM
            //
            // Şimdilik checksum algoritmasını bilmiyoruz.
            // Bu nedenle sadece son byte'ı paketten ayırıyoruz.
            // ----------------------------------------------------

            const quint8 checksum =
                static_cast<quint8>(
                    rawData.at(
                        rawData.size() - 1
                        )
                    );

            Q_UNUSED(checksum);


            // ----------------------------------------------------
            // PAYLOAD
            //
            // Header: 2 byte
            // Checksum: 1 byte
            //
            // Geri kalan bölüm Parser'a gider.
            // ----------------------------------------------------

            const QByteArray payload =
                rawData.mid(
                    2,
                    rawData.size() - 3
                    );


            qDebug()
                << "Main: Packet size:"
                << rawData.size()
                << "| Payload size:"
                << payload.size();


            // ----------------------------------------------------
            // RAW DATA PARSER
            // ----------------------------------------------------

            rawDataParser.parse(
                payload,
                parameters
                );
        }
        );

    engine.load(url);

    return app.exec();
}