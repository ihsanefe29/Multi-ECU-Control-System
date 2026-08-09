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
    // signal chart screen. Any module's mode combo being set to
    // "Veri Görüntüleme" navigates here (EcuPanel.dataViewSelected) -
    // there's only one shared chart screen, not one per module.
    property string activeScreen: "control"

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
                    session: dispatcher.ecuAModule1
                    onDataViewSelected: window.activeScreen = "dataView"
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: dispatcher.ecuAModule2
                    onDataViewSelected: window.activeScreen = "dataView"
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: dispatcher.ecuBModule1
                    onDataViewSelected: window.activeScreen = "dataView"
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: dispatcher.ecuBModule2
                    onDataViewSelected: window.activeScreen = "dataView"
                }
            }
        }
    }

    SignalChartScreen {
        anchors.fill: parent
        visible: window.activeScreen === "dataView"
        onBackRequested: window.activeScreen = "control"
    }
}
