#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QMap>
#include <QStringList>

// N1, EGT, FuelFlow, BleedValve, Vibration gibi ECU parametrelerini
// tutar ve QML'e açar (esra_parameter-page branch).
class ParameterModel : public QObject
{
    Q_OBJECT

public:
    explicit ParameterModel(QObject *parent = nullptr);

    Q_INVOKABLE void setValue(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant getValue(const QString &name) const;
    Q_INVOKABLE QVariantMap getAllValues() const;

    // Parametre sırasını (ID'ye göre) kaydet
    Q_INVOKABLE void setParameterOrder(const QStringList &orderedNames);

    // Sıralı değer listesi (C++ tarafı için)
    QVariantList getOrderedValues() const;
    QStringList  parameterOrder() const;

signals:
    void valueChanged(const QString &name, const QVariant &value);

private:
    QMap<QString, QVariant> m_values;
    QStringList m_order;
};
