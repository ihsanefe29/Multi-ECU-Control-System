#include "TestHelpers.h"
#include <QQmlEngine>
#include <QJSEngine>
#include <QMetaProperty>
#include <QMetaMethod>
#include <functional>

TestHelpers::TestHelpers(QObject *parent)
    : QObject(parent)
{
}

static QObject *findChildRecursive(QObject *root, const std::function<bool(QObject *)> &matcher)
{
    if (!root)
        return nullptr;

    if (matcher(root))
        return root;

    const auto children = root->children();
    for (QObject *child : children) {
        QObject *found = findChildRecursive(child, matcher);
        if (found)
            return found;
    }

    return nullptr;
}

QObject *TestHelpers::findChild(QObject *root, const QString &propertyName, const QVariant &propertyValue)
{
    return findChildRecursive(root, [&](QObject *object) {
        return object->property(propertyName.toUtf8().constData()) == propertyValue;
    });
}

QObject *TestHelpers::findByText(QObject *root, const QString &text)
{
    return findChildRecursive(root, [&](QObject *object) {
        return object->property("text") == text;
    });
}

QObject *TestHelpers::findComboBox(QObject *root)
{
    return findChildRecursive(root, [&](QObject *object) {
        return object->metaObject()->className() == QByteArrayLiteral("QQuickComboBox")
            || object->metaObject()->className() == QByteArrayLiteral("ComboBox");
    });
}

QObject *TestHelpers::findBySignal(QObject *root, const QString &signalName)
{
    return findChildRecursive(root, [&](QObject *object) {
        const QMetaObject *meta = object->metaObject();
        for (int i = 0; i < meta->methodCount(); ++i) {
            QMetaMethod method = meta->method(i);
            if (method.methodType() == QMetaMethod::Signal && method.name() == signalName)
                return true;
        }
        return false;
    });
}

static QObject *testHelpersSingletonProvider(QQmlEngine *engine, QJSEngine *)
{
    return new TestHelpers(engine);
}

static const bool testHelpersRegistered = [] {
    qmlRegisterSingletonType<TestHelpers>("TestHelpers", 1, 0, "TestHelpers", testHelpersSingletonProvider);
    return true;
}();