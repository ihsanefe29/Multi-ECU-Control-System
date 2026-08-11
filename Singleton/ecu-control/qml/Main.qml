import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import "components"

Window {
    id: window
    visible: true
    width: 1100
    height: 700
    color: "#14171a"
    title: qsTr("ECU Kontrol Arayüzü")

    // "control" = the 4-module grid, "dataView" = the standalone
    // signal chart screen, "firmwareUpload" = the (module-specific)
    // firmware staging screen. Mode combo selections navigate here
    // via EcuPanel's dataViewSelected/firmwareUploadSelected signals.
    property string activeScreen: "control"
    property string firmwareTargetEcuId: ""
    property int firmwareTargetModule: 0

    function openFirmwareUpload(ecuId, moduleNumber) {
        window.firmwareTargetEcuId = ecuId
        window.firmwareTargetModule = moduleNumber
        window.activeScreen = "firmwareUpload"
    }

    Item {
        anchors.fill: parent
        visible: window.activeScreen === "control"

        Column {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Text {
                text: qsTr("ECU Kontrol Paneli")
                color: "#e8ebee"
                font.pixelSize: 20
                font.bold: true
            }

            GridLayout {
                width: parent.width
                height: parent.height - 40
                columns: 2
                rowSpacing: 16
                columnSpacing: 16

                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: Dispatcher.ecuAModule1
                    onDataViewSelected: window.activeScreen = "dataView"
                    onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: Dispatcher.ecuAModule2
                    onDataViewSelected: window.activeScreen = "dataView"
                    onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: Dispatcher.ecuBModule1
                    onDataViewSelected: window.activeScreen = "dataView"
                    onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: Dispatcher.ecuBModule2
                    onDataViewSelected: window.activeScreen = "dataView"
                    onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                }
            }
        }
    }

    SignalChartScreen {
        anchors.fill: parent
        visible: window.activeScreen === "dataView"
        onBackRequested: window.activeScreen = "control"
    }

    FirmwareUploadScreen {
        anchors.fill: parent
        visible: window.activeScreen === "firmwareUpload"
        ecuId: window.firmwareTargetEcuId
        moduleNumber: window.firmwareTargetModule
        onBackRequested: window.activeScreen = "control"
    }
}
