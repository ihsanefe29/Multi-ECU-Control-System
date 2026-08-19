#pragma once
#include <QObject>
#include <QVector>
#include "parameter.h"

// Excel (.xlsx) parametre dosyasını okur.
// (aybuke branch — ExcelParser)
//
// Excel sütun düzeni (satır 1 başlık, satır 2'den veri):
//   Sütun 1: RAM Address
//   Sütun 2: Data Width (bit)
//   Sütun 3: Bit Offset
//   Sütun 4: Data Type  (uint8/uint16/.../float)
//   Sütun 5: Parameter Name
//   Sütun 6: Conversion Formula  (x * 0.1, Enum, Hex, vb.)
//
// QXlsx kütüphanesine bağımlıdır (src/QXlsx/).
class ExcelParser : public QObject
{
    Q_OBJECT

public:
    explicit ExcelParser(QObject *parent = nullptr);

    // Dosyayı yükle. Başarısızsa false döner.
    bool loadFile(const QString &filePath);

    QVector<Parameter> parameters() const;

private:
    QVector<Parameter> m_parameters;
};
