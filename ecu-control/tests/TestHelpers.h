#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <QObject>
#include <QVariant>

class TestHelpers : public QObject
{
    Q_OBJECT
public:
    explicit TestHelpers(QObject *parent = nullptr);

    Q_INVOKABLE QObject *findChild(QObject *root, const QString &propertyName, const QVariant &propertyValue);
    Q_INVOKABLE QObject *findByText(QObject *root, const QString &text);
    Q_INVOKABLE QObject *findComboBox(QObject *root);
    Q_INVOKABLE QObject *findBySignal(QObject *root, const QString &signalName);
};

#endif // TESTHELPERS_H