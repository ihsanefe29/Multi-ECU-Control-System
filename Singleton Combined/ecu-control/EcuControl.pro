QT       += core gui widgets qml quick quickcontrols2 charts

TARGET   = EcuControl
TEMPLATE = app

CONFIG   += c++17

SOURCES += \
    src/main.cpp \
    src/core/dispatcher.cpp \
    src/core/modulesession.cpp \
    src/core/moduleworker.cpp \
    src/core/canfdemulator.cpp \
    src/core/ccpprotocol.cpp \
    src/core/signalfileloader.cpp \
    src/core/livesignalsource.cpp \
    src/core/rawsignalloader.cpp

HEADERS += \
    src/core/dispatcher.h \
    src/core/modulesession.h \
    src/core/moduleworker.h \
    src/core/icanfdconnector.h \
    src/core/canfdemulator.h \
    src/core/ccpprotocol.h \
    src/core/signalfileloader.h \
    src/core/livesignalsource.h \
    src/core/rawsignalloader.h

RESOURCES += qml.qrc
