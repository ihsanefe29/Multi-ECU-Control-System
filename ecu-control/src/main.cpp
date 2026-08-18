#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDebug>
#include "core/dispatcher.h"

int main(int argc, char *argv[])
{
    // Qt 5.15 + MinGW: AA_UseDesktopOpenGL bazı sistemlerde pencereyi
    // tamamen engeller (ANGLE/D3D backend yoksa). ShareOpenGLContexts
    // yeterli ve daha güvenli.
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    // "Fusion" Qt 5'te tüm platformlarda güvenilir çalışır.
    // "Default" Qt 6'da kaldırıldı; Qt 5.15'te de bazen sorun çıkarır.
    QQuickStyle::setStyle("Fusion");

    // QApplication (not QGuiApplication): Qt.labs.platform's
    // FileDialog needs a QtWidgets-based fallback on platforms
    // without a native/portal file dialog (e.g. plain X11 Linux
    // without a desktop portal). QApplication is a strict superset,
    // so this doesn't change anything on platforms with a real
    // native dialog (e.g. Windows).
    QApplication app(argc, argv);

    // signalLoader and liveSource live inside Dispatcher (as
    // Dispatcher.signalLoader / Dispatcher.liveSource) rather than as
    // separate top-level objects, so every QML-visible object is
    // reachable through the one Dispatcher entry point.
    Dispatcher dispatcher;

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

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "[EcuControl] QML motoru hicbir nesne yukleyemedi."
                    << "QML import hatasi veya kaynak dosyasi eksik olabilir.";
        return -1;
    }

    return app.exec();
}
