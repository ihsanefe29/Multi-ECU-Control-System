#include "ParameterList.h"

void ParameterList::addParameter(const Parameter &parameter)
{
    m_parameters.append(parameter);
}

QVector<Parameter> ParameterList::parameters() const
{
    return m_parameters;
}