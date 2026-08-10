#ifndef PARAMETERLIST_H
#define PARAMETERLIST_H

#include <QVector>

#include "Parameter.h"

class ParameterList
{
public:

    void addParameter(const Parameter &parameter);

    QVector<Parameter> parameters() const;

private:

    QVector<Parameter> m_parameters;
};

#endif