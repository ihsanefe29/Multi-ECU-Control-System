#include "ExcelParser.h"

#include "xlsxdocument.h"

#include <QDebug>

ExcelParser::ExcelParser(QObject *parent)
    : QObject(parent)
{
}

bool ExcelParser::loadFile(const QString &filePath)
{
    m_parameters.clear();

    QXlsx::Document document(filePath);

    if (!document.load())
    {
        qDebug() << "ExcelParser: Excel file could not be opened!";
        return false;
    }

    qDebug() << "ExcelParser: Excel file opened successfully.";

    for (int row = 2; ; ++row)
    {
        QVariant addressValue =
            document.read(row, 1);

        QVariant offsetValue =
            document.read(row, 2);

        QVariant widthValue =
            document.read(row, 3);

        QVariant typeValue =
            document.read(row, 4);

        QVariant nameValue =
            document.read(row, 5);

        QVariant formulaValue =
            document.read(row, 6);

        // RAM Address hücresi boşsa
        // Excel'deki parametrelerin sonuna gelmişiz demektir.
        if (!addressValue.isValid() ||
            addressValue.toString().trimmed().isEmpty())
        {
            break;
        }

        Parameter parameter;

        parameter.ramAddress =
            addressValue.toInt();

        parameter.dataOffset =
            offsetValue.toInt();

        parameter.dataWidth =
            widthValue.toInt();

        parameter.dataType =
            typeValue.toString();

        parameter.parameterName =
            nameValue.toString();

        parameter.conversionFormula =
            formulaValue.toString();

        m_parameters.append(parameter);

        qDebug() << "Parameter:"
                 << parameter.parameterName
                 << "| Address:" << parameter.ramAddress
                 << "| Offset:" << parameter.dataOffset
                 << "| Width:" << parameter.dataWidth
                 << "| Type:" << parameter.dataType
                 << "| Formula:" << parameter.conversionFormula;
    }

    qDebug() << "Total parameters:"
             << m_parameters.size();

    return true;
}

QVector<Parameter> ExcelParser::parameters() const
{
    return m_parameters;
}