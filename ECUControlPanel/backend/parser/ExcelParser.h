#ifndef EXCELPARSER_H
#define EXCELPARSER_H

#include <QObject>
#include <QVector>

#include "Parameter.h"

class ExcelParser : public QObject
{
    Q_OBJECT

public:
    explicit ExcelParser(QObject *parent = nullptr);

    bool loadFile(const QString &filePath);

    QVector<Parameter> parameters() const;

private:
    QVector<Parameter> m_parameters;
};

#endif // EXCELPARSER_H