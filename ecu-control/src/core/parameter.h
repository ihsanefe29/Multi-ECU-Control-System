#pragma once
#include <QString>
#include <QtGlobal>

// RAM adres tablosundan okunan tek bir ECU parametresi.
// ExcelParser ve RawDataParser bu struct'ı paylaşır.
enum class ParameterDataType
{
    Boolean,
    Int8, Int16, Int32, Int64,
    UInt8, UInt16, UInt32, UInt64,
    Single, // float 32-bit
    Double  // double 64-bit
};

struct Parameter
{
    int ramAddress       = 0;
    int dataWidth        = 0;   // bit cinsinden
    int bitOffset        = 0;   // byte içindeki bit kayması

    QString dataType;           // Orijinal metin (örn: "uint16", "float", "double", "boolean")
    ParameterDataType typeEnum = ParameterDataType::UInt32;

    QString parameterName;
    QString conversionFormula;  // "x * 0.1", "x / 10", "Enum", "Hex", vb.
    double  resolution   = 1.0; // Çarpan (örn: 0.01)
    QString unit;               // Birim (örn: "°C", "RPM", "V")
    double  minValue     = 0.0;
    double  maxValue     = 0.0;

    static ParameterDataType parseDataType(const QString &str)
    {
        const QString s = str.trimmed().toLower();
        if (s == "bool" || s == "boolean") return ParameterDataType::Boolean;
        if (s == "int8")                   return ParameterDataType::Int8;
        if (s == "int16")                  return ParameterDataType::Int16;
        if (s == "int32" || s == "int")    return ParameterDataType::Int32;
        if (s == "int64")                  return ParameterDataType::Int64;
        if (s == "uint8")                  return ParameterDataType::UInt8;
        if (s == "uint16")                 return ParameterDataType::UInt16;
        if (s == "uint32" || s == "uint")  return ParameterDataType::UInt32;
        if (s == "uint64")                 return ParameterDataType::UInt64;
        if (s == "single" || s == "float") return ParameterDataType::Single;
        if (s == "double")                 return ParameterDataType::Double;
        return ParameterDataType::UInt32;
    }
};
