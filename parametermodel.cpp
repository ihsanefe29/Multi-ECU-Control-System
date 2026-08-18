#include "parametermodel.h"

ParameterModel::ParameterModel(QObject *parent)
    : QObject(parent)
{
}

void ParameterModel::setValue(const QString &name, const QVariant &value)
{
    QVariant normalized = value;

    // Eğer değer aslında tam sayıysa (ondalık kısmı 0 ise), int'e zorla
    if (value.type() == QVariant::Double) {
        double d = value.toDouble();
        if (d == static_cast<qint64>(d)) {
            normalized = static_cast<int>(d);
        }
    }

    if (m_values.value(name) != normalized) {
        m_values[name] = normalized;
        emit valueChanged(name, normalized);
    }
}

QVariant ParameterModel::getValue(const QString &name) const
{
    return m_values.value(name);
}

QVariantMap ParameterModel::getAllValues() const
{
    QVariantMap result;
    for (auto it = m_values.constBegin(); it != m_values.constEnd(); ++it) {
        result.insert(it.key(), it.value());
    }
    return result;
}

void ParameterModel::setParameterOrder(const QStringList &orderedNames)
{
    m_order = orderedNames;
}

QStringList ParameterModel::parameterOrder() const
{
    return m_order;
}

QVariantList ParameterModel::getOrderedValues() const
{
    QVariantList result;
    for (const QString &name : m_order) {
        // Henüz değer girilmemiş parametre için 0 (varsayılan) koy
        result.append(m_values.value(name, 0));
    }
    return result;
}