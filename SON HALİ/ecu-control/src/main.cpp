#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "core/dispatcher.h"
#include "core/signalfileloader.h"
#include "core/livesignalsource.h"

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

    Dispatcher dispatcher;
    SignalFileLoader signalLoader;
    LiveSignalSource liveSource;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("dispatcher", &dispatcher);
    engine.rootContext()->setContextProperty("signalLoader", &signalLoader);
    engine.rootContext()->setContextProperty("liveSource", &liveSource);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
