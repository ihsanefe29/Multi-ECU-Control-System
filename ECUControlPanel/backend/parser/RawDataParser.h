#ifndef RAWDATAPARSER_H
#define RAWDATAPARSER_H

#include <QObject>
#include <QVariantMap>
#include <QVector>

#include "Parameter.h"


class RawDataParser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QVariantMap values
            READ values
                NOTIFY valuesChanged
        )


public:

    explicit RawDataParser(
        QObject *parent = nullptr
        );


    void setParameters(
        const QVector<Parameter> &parameters
        );


    void parse(
        const QByteArray &rawData,
        const QVector<Parameter> &parameters
        );


    QVariantMap values() const;


    Q_INVOKABLE QVariantMap parameterInfo(
        const QString &parameterName
        ) const;


signals:

    void valuesChanged();


private:

    QVariant parseValue(
        const QByteArray &data,
        const QString &dataType
        );


    QVariant applyConversion(
        const QVariant &rawValue,
        const QString &formula
        );


    QVector<Parameter> m_parameters;

    QVariantMap m_values;
};


#endif // RAWDATAPARSER_H