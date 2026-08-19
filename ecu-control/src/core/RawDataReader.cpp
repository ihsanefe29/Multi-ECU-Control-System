#include "RawDataReader.h"

#include <QFile>
#include <QDebug>

RawDataReader::RawDataReader(QObject *parent)
    : QObject(parent)
{
}

QByteArray RawDataReader::readFile(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "RawDataReader: File could not be opened!";
        return QByteArray();
    }

    QByteArray fileData = file.readAll();

    file.close();

    // Dosyadaki boşlukları ve satır sonlarını kaldır.
    fileData.replace(" ", "");
    fileData.replace("\n", "");
    fileData.replace("\r", "");

    QByteArray rawData;

    // Her iki hexadecimal karakter bir byte oluşturur.
    for (int i = 0; i + 1 < fileData.size(); i += 2)
    {
        bool ok = false;

        int value =
            QString(fileData.mid(i, 2)).toInt(&ok, 16);

        if (!ok)
        {
            qDebug() << "RawDataReader: Invalid byte:"
                     << fileData.mid(i, 2);

            continue;
        }

        rawData.append(static_cast<char>(value));
    }

    qDebug() << "RawDataReader: Raw data size:"
             << rawData.size()
             << "bytes";

    return rawData;
}