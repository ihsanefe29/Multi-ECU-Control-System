QT += quick
QT += charts
include(QXlsx/QXlsx/QXlsx.pri)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        backend/collector/UICollector.cpp \
        backend/controller/ECUController.cpp \
        backend/dispatcher/Dispatcher.cpp \
        backend/manager/ECUManager.cpp \
        backend/parser/ExcelParser.cpp \
        backend/parser/RawDataParser.cpp \
        backend/parser/RawDataReader.cpp \
        backend/simulator/DataSimulator.cpp \
        main.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    ECUControlPanel_tr_TR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    backend/collector/UICollector.h \
    backend/controller/ECUController.h \
    backend/dispatcher/Dispatcher.h \
    backend/enums/CommandType.h \
    backend/enums/ECUType.h \
    backend/enums/ModeType.h \
    backend/manager/ECUManager.h \
    backend/message/UIMessage.h \
    backend/parser/ExcelParser.h \
    backend/parser/Parameter.h \
    backend/parser/RawDataParser.h \
    backend/parser/RawDataReader.h \
    backend/simulator/DataSimulator.h

DISTFILES += \
    docs/ECU_State_Machine.md \
    data/ECU_Data.xlsx \
    data/raw_data.txt
