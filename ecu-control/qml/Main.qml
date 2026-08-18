import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
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

    function openParameters() {
        window.activeScreen = "parameters"
    }

    Item {
        anchors.fill: parent
        visible: window.activeScreen === "control"

        Column {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            // ── Başlık + Acil Kontroller (EceGulYuksel branch) ──────────────
            Row {
                width: parent.width
                spacing: 12

                Text {
                    text: qsTr("ECU Kontrol Paneli")
                    color: "#e8ebee"
                    font.pixelSize: 20
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item { width: parent.width - 420; height: 1 }

                Button {
                    text: Dispatcher.startupInProgress
                          ? qsTr("⏳ Başlatılıyor…")
                          : qsTr("▶ Sıralı Başlat")
                    enabled: !Dispatcher.startupInProgress && !Dispatcher.failSafeActive
                    onClicked: Dispatcher.startupSequence()
                }

                Button {
                    text: Dispatcher.failSafeActive
                          ? qsTr("⚠ FAİL-SAFE — Temizle")
                          : qsTr("⛔ Acil Durdur")
                    highlighted: true
                    palette.button: Dispatcher.failSafeActive ? "#dc2626" : "#b91c1c"
                    onClicked: {
                        if (Dispatcher.failSafeActive)
                            Dispatcher.clearFailSafe()
                        else
                            Dispatcher.emergencyStop()
                    }
                }
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
                    onParameterPageSelected: window.openParameters()
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: Dispatcher.ecuAModule2
                    onDataViewSelected: window.activeScreen = "dataView"
                    onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                    onParameterPageSelected: window.openParameters()
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: Dispatcher.ecuBModule1
                    onDataViewSelected: window.activeScreen = "dataView"
                    onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                    onParameterPageSelected: window.openParameters()
                }
                EcuPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    session: Dispatcher.ecuBModule2
                    onDataViewSelected: window.activeScreen = "dataView"
                    onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                    onParameterPageSelected: window.openParameters()
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

    // Parametre kontrol ekranı (esra_parameter-page branch)
    ParameterPage {
        anchors.fill: parent
        visible: window.activeScreen === "parameters"
        onBackRequested: window.activeScreen = "control"
    }
    // ── Fail-Safe kırmızı uyarı bandı (EceGulYuksel branch) ───────────────
    Rectangle {
        visible: Dispatcher.failSafeActive
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 44
        color: "#dc2626"
        z: 100

        Text {
            anchors.centerIn: parent
            text: qsTr("⚠  SİSTEM FAIL-SAFE MODUNDA — TÜM MODÜLLER DURDURULDU  ⚠")
            color: "white"
            font.pixelSize: 15
            font.bold: true
        }
    }

    // ── Olay Akışı (Log) paneli — sadece kontrol ekranında görünür ─────────
    Rectangle {
        visible: window.activeScreen === "control"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.topMargin: 16
        anchors.rightMargin: 16
        anchors.bottomMargin: 16
        width: 260
        color: "#141F3D"
        radius: 8

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            Text {
                text: qsTr("Olay Akışı")
                color: "white"
                font.pixelSize: 14
                font.bold: true
            }

            Rectangle { width: parent.width; height: 1; color: "#3A4A7A" }

            ListView {
                id: logView
                width: parent.width
                height: parent.height - 40
                clip: true
                model: Dispatcher.logMessages
                spacing: 3

                delegate: Text {
                    width: logView.width
                    text: modelData
                    color: modelData.indexOf("[KRİTİK]") === 0 ? "#dc2626"
                         : modelData.indexOf("[HATA]")   === 0 ? "#ef4444"
                         : modelData.indexOf("[UYARI]")  === 0 ? "#f59e0b"
                         : modelData.indexOf("[BİLGİ]")  === 0 ? "#2fbf71"
                         :                                        "#b8c4e0"
                    font.pixelSize: 11
                    font.family: "Courier New"
                    font.bold: modelData.indexOf("[KRİTİK]") === 0
                    wrapMode: Text.Wrap
                }

                onCountChanged: positionViewAtEnd()
            }
        }
    }
}
