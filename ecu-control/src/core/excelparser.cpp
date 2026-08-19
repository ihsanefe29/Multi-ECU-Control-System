#include "excelparser.h"
#include "xlsxdocument.h"
#include <QDebug>

ExcelParser::ExcelParser(QObject *parent)
    : QObject(parent)
{}

bool ExcelParser::loadFile(const QString &filePath)
{
    m_parameters.clear();

    QXlsx::Document document(filePath);

    if (!document.load()) {
        qDebug() << "ExcelParser: dosya açılamadı:" << filePath;
        return false;
    }

    qDebug() << "ExcelParser: dosya açıldı:" << filePath;

    // Satır 1 başlık — satır 2'den itibaren veri
    for (int row = 2; ; ++row) {
        QVariant addressVal = document.read(row, 1);

        // RAM Address boşsa son satıra geldik
        if (!addressVal.isValid() ||
            addressVal.toString().trimmed().isEmpty())
            break;

        Parameter p;
        p.ramAddress        = addressVal.toInt();
        p.dataWidth         = document.read(row, 2).toInt();
        p.bitOffset         = document.read(row, 3).toInt();
        p.dataType          = document.read(row, 4).toString();
        p.parameterName     = document.read(row, 5).toString();
        p.conversionFormula = document.read(row, 6).toString();

        m_parameters.append(p);

        qDebug() << "  " << p.parameterName
                 << "addr=" << p.ramAddress
                 << "width=" << p.dataWidth
                 << "type=" << p.dataType
                 << "formula=" << p.conversionFormula;
    }

    qDebug() << "ExcelParser: toplam" << m_parameters.size() << "parametre";
    return true;
}

QVector<Parameter> ExcelParser::parameters() const
{
    return m_parameters;
}
