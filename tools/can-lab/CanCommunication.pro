QT += core serialbus

CONFIG += c++17

TEMPLATE = app
TARGET = QtCanBusService

SOURCES += \
    src/main.cpp \
    src/CanChannelWorker.cpp \
    src/CommunicationCore.cpp

HEADERS += \
    src/CanFrame.h \
    src/CanChannelWorker.h \
    src/CommunicationCore.h \
    src/EcuTelemetry.h