#ifndef RAWDATAREADER_H
#define RAWDATAREADER_H

#include <QObject>
#include <QByteArray>
#include <QString>

class RawDataReader : public QObject
{
    Q_OBJECT

public:
    explicit RawDataReader(QObject *parent = nullptr);

    QByteArray readFile(const QString &filePath);
};

#endif // RAWDATAREADER_H