QT       += core gui widgets qml quick quickcontrols2 charts

TARGET   = EcuControl
TEMPLATE = app

CONFIG   += c++17

# ── QXlsx (aybuke branch — Excel parametre okuma) ──────────────────────────
QXLSX_PARENTPATH = $$PWD/src/QXlsx/QXlsx
QXLSX_HEADERPATH = $$PWD/src/QXlsx/QXlsx/header/
QXLSX_SOURCEPATH = $$PWD/src/QXlsx/QXlsx/source/
include($$PWD/src/QXlsx/QXlsx/QXlsx.pri)

# ── Kaynak dosyalar ─────────────────────────────────────────────────────────
SOURCES += \
    src/main.cpp \
    src/core/dispatcher.cpp \
    src/core/modulesession.cpp \
    src/core/moduleworker.cpp \
    src/core/canfdemulator.cpp \
    src/core/ccpprotocol.cpp \
    src/core/signalfileloader.cpp \
    src/core/livesignalsource.cpp \
    src/core/rawsignalloader.cpp \
    src/core/datasimulator.cpp \
    src/core/parametermodel.cpp \
    src/core/rawdataparser.cpp \
    src/core/excelparser.cpp \
    src/core/packetbuilder.cpp \
    src/core/datacollector.cpp \
    src/core/idispatcher.cpp \
    src/core/mockdispatcher.cpp \
    src/core/RawDataReader.cpp

HEADERS += \
    src/core/dispatcher.h \
    src/core/modulesession.h \
    src/core/moduleworker.h \
    src/core/icanfdconnector.h \
    src/core/canfdemulator.h \
    src/core/ccpprotocol.h \
    src/core/signalfileloader.h \
    src/core/livesignalsource.h \
    src/core/rawsignalloader.h \
    src/core/datasimulator.h \
    src/core/parametermodel.h \
    src/core/parameter.h \
    src/core/rawdataparser.h \
    src/core/excelparser.h \
    src/core/packetbuilder.h \
    src/core/datacollector.h \
    src/core/idispatcher.h \
    src/core/mockdispatcher.h \
    src/core/RawDataReader.h \
    src/core/enums/CommandType.h \
    src/core/enums/ECUType.h \
    src/core/enums/ModeType.h

RESOURCES += qml.qrc
