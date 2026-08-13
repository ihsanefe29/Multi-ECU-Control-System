QT += core gui widgets qml quick quickcontrols2 charts serialbus

CONFIG += c++17

TARGET = EcuControl
TEMPLATE = app

INCLUDEPATH += ../../can-lab/src

SOURCES += \
    src/main.cpp \
    src/core/dispatcher.cpp \
    src/core/modulesession.cpp \
    src/core/signalfileloader.cpp \
    src/core/livesignalsource.cpp \
    src/core/rawsignalloader.cpp \
    ../../can-lab/src/CommunicationCore.cpp \
    ../../can-lab/src/CanChannelWorker.cpp

HEADERS += \
    src/core/dispatcher.h \
    src/core/modulesession.h \
    src/core/signalfileloader.h \
    src/core/livesignalsource.h \
    src/core/rawsignalloader.h \
    ../../can-lab/src/CommunicationCore.h \
    ../../can-lab/src/CanChannelWorker.h \
    ../../can-lab/src/CanFrame.h \
    ../../can-lab/src/EcuTelemetry.h

RESOURCES += qml.qrc