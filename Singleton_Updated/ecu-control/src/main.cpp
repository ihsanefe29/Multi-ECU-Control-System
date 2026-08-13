#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include "core/dispatcher.h"
#include "CommunicationCore.h"
#include "CanFrame.h"
#include "EcuTelemetry.h"

int main(int argc, char *argv[])
{
    // Windows/MinGW defaults to ANGLE (Direct3D-backed OpenGL ES),
    // and Qt Charts' QML ChartView is known to render blank under
    // ANGLE on some setups - it needs real desktop OpenGL. Must be
    // set before the QApplication is constructed.
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QQuickStyle::setStyle("Default");

    // QApplication (not QGuiApplication): Qt.labs.platform's
    // FileDialog needs a QtWidgets-based fallback on platforms
    // without a native/portal file dialog (e.g. plain X11 Linux
    // without a desktop portal). QApplication is a strict superset,
    // so this doesn't change anything on platforms with a real
    // native dialog (e.g. Windows).
    QApplication app(argc, argv);

        
    qRegisterMetaType<CanFrame>("CanFrame");
    qRegisterMetaType<TelemetryValue>("TelemetryValue");
    qRegisterMetaType<EcuTelemetry>("EcuTelemetry");

    // signalLoader and liveSource live inside Dispatcher (as
    // Dispatcher.signalLoader / Dispatcher.liveSource) rather than as
    // separate top-level objects, so every QML-visible object is
    // reachable through the one Dispatcher entry point.
    CommunicationCore core;
    Dispatcher dispatcher(&core);

    QQmlApplicationEngine engine;

    // Registered as a QML singleton (qmlRegisterSingletonInstance,
    // Qt 5.14+) instead of engine.rootContext()->setContextProperty().
    // A context property is a loosely-typed, string-keyed global that
    // isn't checked at compile time and doesn't reliably reach
    // dynamically-created QML contexts (e.g. components instantiated
    // by a Loader with their own context). A QML singleton is a real,
    // versioned, importable type - QML files bring it in explicitly
    // with `import EcuControl 1.0` and reference it as `Dispatcher`,
    // the same way any other QML type is used.
    qmlRegisterSingletonInstance<Dispatcher>("EcuControl", 1, 0, "Dispatcher", &dispatcher);

    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    

    QObject::connect(
        &app,
        &QCoreApplication::aboutToQuit,
        &core,
        &CommunicationCore::stop
    );


    // Start CommunicationCore after Qt event loop initialization.
    QTimer::singleShot(
        0,
        &core,
        &CommunicationCore::start
    );


    return app.exec();
}
