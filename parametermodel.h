#ifndef PARAMETERMODEL_H
#define PARAMETERMODEL_H

#include <QObject>
#include <QVariantMap>
#include <QMap>
#include <QStringList>
#include <QVariantList>

class ParameterModel : public QObject
{
    Q_OBJECT

public:
    explicit ParameterModel(QObject *parent = nullptr);

    Q_INVOKABLE void setValue(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant getValue(const QString &name) const;
    Q_INVOKABLE QVariantMap getAllValues() const;

    // Yeni: parametre sırasını (ID'ye göre) kaydet
    Q_INVOKABLE void setParameterOrder(const QStringList &orderedNames);

    // Yeni: C++ tarafında kullanılacak, sıralı değer listesi döner
    QVariantList getOrderedValues() const;
    QStringList parameterOrder() const;

signals:
    void valueChanged(const QString &name, const QVariant &value);

private:
    QMap<QString, QVariant> m_values;
    QStringList m_order;   // parametre adlarının ID sırasına göre listesi
};

#endif // PARAMETERMODEL_H