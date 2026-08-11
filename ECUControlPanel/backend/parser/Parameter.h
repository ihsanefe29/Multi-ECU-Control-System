#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>

struct Parameter
{
    int ramAddress;
    int dataOffset;
    int dataWidth;

    QString dataType;
    QString parameterName;
    QString conversionFormula;
};

#endif // PARAMETER_H