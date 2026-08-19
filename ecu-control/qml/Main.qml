import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import "components"

Window {
    id: window
    visible: true
    minimumWidth: 800
    minimumHeight: 560
    width: 1280
    height: 800
    color: "#0f1623"
    title: qsTr("ECU Kontrol Arayüzü")

    property string activeScreen: "control"
    property string firmwareTargetEcuId: ""
    property int firmwareTargetModule: 0

    // Her modülün aksent rengi
    readonly property var accentColors: ["#3b82f6", "#a855f7", "#14b8a6", "#f97316"]

    function openFirmwareUpload(ecuId, moduleNumber) {
        window.firmwareTargetEcuId = ecuId
        window.firmwareTargetModule = moduleNumber
        window.activeScreen = "firmwareUpload"
    }
    function openParameters() {
        window.activeScreen = "parameters"
    }

    // ── Kontrol Ekranı ────────────────────────────────────────────────────────
    Item {
        id: controlScreen
        anchors.fill: parent
        anchors.margins: 16
        visible: window.activeScreen === "control"

        ColumnLayout {
            anchors.fill: parent
            spacing: 12

            // ── Header ────────────────────────────────────────────────────────
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // Dikey mavi çizgi + başlık
                Rectangle {
                    width: 4; height: 34; radius: 2
                    color: "#3b82f6"
                }
                Text {
                    text: "ECU Kontrol Paneli"
                    color: "#e2e8f0"
                    font.pixelSize: Math.max(16, window.height * 0.028)
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                }

                Item { Layout.fillWidth: true }

                // Sıralı Başlat
                Button {
                    id: startBtn
                    text: Dispatcher.startupInProgress ? "⟳  Başlatılıyor..." : "▶  Sıralı Başlat"
                    enabled: !Dispatcher.startupInProgress && !Dispatcher.failSafeActive
                    implicitWidth: 170; implicitHeight: 36
                    onClicked: Dispatcher.startupSequence()

                    background: Rectangle {
                        radius: 8
                        color: parent.enabled
                               ? (parent.pressed  ? "#15803d"
                               :  parent.hovered  ? "#22c55e" : "#16a34a")
                               : "#1e2d1e"
                        Behavior on color { ColorAnimation { duration: 140 } }
                    }
                    contentItem: Text {
                        text: parent.text; color: "white"
                        font.pixelSize: 13; font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:   Text.AlignVCenter
                    }
                }

                // Acil Durdur
                Button {
                    id: emergencyBtn
                    text: Dispatcher.failSafeActive ? "✕  Fail-Safe Temizle" : "⛔  Acil Durdur"
                    implicitWidth: 185; implicitHeight: 36
                    onClicked: {
                        if (Dispatcher.failSafeActive) Dispatcher.clearFailSafe()
                        else Dispatcher.emergencyStop()
                    }
                    background: Rectangle {
                        radius: 8
                        color: parent.pressed ? "#7f1d1d"
                             : parent.hovered ? "#ef4444" : "#dc2626"
                        Behavior on color { ColorAnimation { duration: 140 } }
                    }
                    contentItem: Text {
                        text: parent.text; color: "white"
                        font.pixelSize: 13; font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:   Text.AlignVCenter
                    }
                }
            }

            // ── Gövde: 2×2 grid + log paneli ─────────────────────────────────
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12

                // 2×2 ECU kartları
                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: 2
                    rowSpacing: 12
                    columnSpacing: 12

                    EcuPanel {
                        Layout.fillWidth: true; Layout.fillHeight: true
                        session: Dispatcher.ecuAModule1
                        accentColor: window.accentColors[0]
                        onDataViewSelected:       window.activeScreen = "dataView"
                        onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                        onParameterPageSelected:  window.openParameters()
                    }
                    EcuPanel {
                        Layout.fillWidth: true; Layout.fillHeight: true
                        session: Dispatcher.ecuAModule2
                        accentColor: window.accentColors[1]
                        onDataViewSelected:       window.activeScreen = "dataView"
                        onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                        onParameterPageSelected:  window.openParameters()
                    }
                    EcuPanel {
                        Layout.fillWidth: true; Layout.fillHeight: true
                        session: Dispatcher.ecuBModule1
                        accentColor: window.accentColors[2]
                        onDataViewSelected:       window.activeScreen = "dataView"
                        onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                        onParameterPageSelected:  window.openParameters()
                    }
                    EcuPanel {
                        Layout.fillWidth: true; Layout.fillHeight: true
                        session: Dispatcher.ecuBModule2
                        accentColor: window.accentColors[3]
                        onDataViewSelected:       window.activeScreen = "dataView"
                        onFirmwareUploadSelected: window.openFirmwareUpload(session.ecuId, session.moduleNumber)
                        onParameterPageSelected:  window.openParameters()
                    }
                }

                // ── Log Paneli ────────────────────────────────────────────────
                Rectangle {
                    Layout.preferredWidth: Math.max(200, window.width * 0.21)
                    Layout.fillHeight: true
                    color: "#0a1628"
                    radius: 12
                    border.color: "#1e3a5f"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        // Log başlığı
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6
                            Rectangle { width: 4; height: 18; radius: 2; color: "#3b82f6" }
                            Text {
                                text: "Olay Akışı"
                                color: "#e2e8f0"
                                font.pixelSize: 14; font.bold: true
                            }
                            Item { Layout.fillWidth: true }
                            Text {
                                text: Dispatcher.logMessages.length + " kayıt"
                                color: "#475569"; font.pixelSize: 10
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: "#1e3a5f" }

                        ListView {
                            id: logView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            model: Dispatcher.logMessages
                            spacing: 1

                            delegate: Rectangle {
                                width: logView.width
                                height: logText.implicitHeight + 6
                                color: index % 2 === 0 ? "transparent" : "#ffffff05"
                                radius: 3

                                Text {
                                    id: logText
                                    anchors {
                                        left: parent.left; right: parent.right
                                        leftMargin: 4; rightMargin: 4
                                        verticalCenter: parent.verticalCenter
                                    }
                                    text: modelData
                                    color: modelData.indexOf("[KRİTİK]") === 0 ? "#f87171"
                                         : modelData.indexOf("[HATA]")   === 0 ? "#fca5a5"
                                         : modelData.indexOf("[UYARI]")  === 0 ? "#fbbf24"
                                         : modelData.indexOf("[BİLGİ]")  === 0 ? "#4ade80"
                                         :                                        "#94a3b8"
                                    font.pixelSize: 10
                                    font.family: "Courier New"
                                    wrapMode: Text.Wrap
                                }
                            }
                            onCountChanged: positionViewAtEnd()
                        }
                    }
                }
            }
        }
    }

    // ── Fail-Safe Uyarı Bandı ────────────────────────────────────────────────
    Rectangle {
        visible: Dispatcher.failSafeActive
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 44
        color: "#7f1d1d"
        z: 100

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#ef4444"; border.width: 2
        }

        Row {
            anchors.centerIn: parent
            spacing: 12
            Text { text: "⚠";  color: "#fca5a5"; font.pixelSize: 20; anchors.verticalCenter: parent.verticalCenter }
            Text {
                text: "SİSTEM FAIL-SAFE MODUNDA  —  TÜM MODÜLLER DURDURULDU"
                color: "white"; font.pixelSize: 14; font.bold: true; font.letterSpacing: 1.2
                anchors.verticalCenter: parent.verticalCenter
            }
            Text { text: "⚠"; color: "#fca5a5"; font.pixelSize: 20; anchors.verticalCenter: parent.verticalCenter }
        }
    }

    // ── Diğer Ekranlar ────────────────────────────────────────────────────────
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
    ParameterPage {
        anchors.fill: parent
        visible: window.activeScreen === "parameters"
        onBackRequested: window.activeScreen = "control"
    }
}
