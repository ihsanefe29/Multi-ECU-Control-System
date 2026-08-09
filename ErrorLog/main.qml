import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    width: 1000
    height: 720
    visible: true
    title: "ECU Control Panel"
    color: "#0E1626"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // --- SOL TARAF: 2x2 ECU IZGARASI ---
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            rows: 2
            columnSpacing: 12
            rowSpacing: 12

            Repeater {
                model: dispatcher.ecus

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#1C2B59"
                    radius: 8

                    property var ecu: modelData
                    property int ecuIndex: index

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Rectangle {
                                width: 16
                                height: 16
                                radius: 8
                                color: ecu.status === "Arızalı"           ? "#DC2626"
                                                                     : ecu.status === "Test başarısız"     ? "#F59E0B"
                                                                     : ecu.status === "Test ediliyor"      ? "#3B82F6"
                                                                     : ecu.status === "Kill"              ? "#EF4444"
                                                                     : ecu.status === "Bağlı"             ? "#22C55E"
                                                                     : ecu.status === "Bağlanıyor"        ? "#3B82F6"
                                                                     : ecu.status === "Yeniden bağlanıyor" ? "#3B82F6"
                                                                     : ecu.powered                        ? "#F59E0B"
                                                                     :                                      "#6B7280"
                                border.width: 1
                                border.color: "#0E1626"
                            }

                            Text {
                                text: ecu.name
                                color: "white"
                                font.pixelSize: 20
                                font.bold: true
                                Layout.fillWidth: true
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            color: "#3A4A7A"
                        }

                        Text {
                            text: "Durum: " + ecu.status
                            color: "#B8C4E0"
                            font.pixelSize: 13
                        }

                        Text {
                            text: ecu.mode === "Yazılım Yükleme"
                                  ? "Yazılım yükleme modu aktif"
                                  : "RPM: " + ecu.rpm
                            color: "#B8C4E0"
                            font.pixelSize: 13
                        }

                        // Sadece arızalıyken görünen kurtarma butonu
                        Button {
                            text: "⟳ Reset (Arıza Temizle)"
                            Layout.fillWidth: true
                            visible: ecu.status === "Arızalı"
                            highlighted: true
                            onClicked: dispatcher.sendCommand(ecuIndex, "reset")
                        }

                        Button {
                            text: ecu.powered ? "Gücü Kes" : "Güç Ver"
                            Layout.fillWidth: true
                            enabled: ecu.status !== "Arızalı"
                            highlighted: ecu.powered
                            onClicked: dispatcher.sendCommand(ecuIndex, "power")
                        }

                        Button {
                            text: "Mod: " + ecu.mode
                            Layout.fillWidth: true
                            enabled: ecu.powered && ecu.status !== "Arızalı"
                            onClicked: dispatcher.sendCommand(ecuIndex, "mode")
                        }

                        Button {
                            text: ecu.status === "Bağlı" ? "Bağlantıyı Kes" : "Bağlan"
                            Layout.fillWidth: true
                            enabled: ecu.powered && ecu.status !== "Arızalı"
                            onClicked: {
                                if (ecu.status === "Bağlı")
                                    dispatcher.sendCommand(ecuIndex, "disconnect")
                                else
                                    dispatcher.sendCommand(ecuIndex, "connect")
                            }
                        }

                        ModeSwitch {
                            Layout.fillWidth: true
                            enabled: ecu.powered && ecu.status !== "Arızalı"
                            currentMode: ecu.status === "Bağlı" ? "ON"
                                       : ecu.status === "Kill"  ? "KILL"
                                       : "OFF"
                            onModeRequested: {
                                if (mode === "ON")
                                    dispatcher.sendCommand(ecuIndex, "connect")
                                else if (mode === "OFF")
                                    dispatcher.sendCommand(ecuIndex, "disconnect")
                                else if (mode === "KILL")
                                    dispatcher.sendCommand(ecuIndex, "kill")
                            }
                        }
                    }
                }
            }
        }

        // --- SAĞ TARAF: OLAY AKIŞI PANELİ ---
        Rectangle {
            Layout.preferredWidth: 280
            Layout.fillHeight: true
            color: "#141F3D"
            radius: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Text {
                    text: "Olay Akışı"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                Button {
                    text: dispatcher.failSafeActive ? "⚠ FAIL-SAFE AKTİF — Temizle" : "⛔ ACİL DURDUR"
                    Layout.fillWidth: true
                    highlighted: true
                    onClicked: {
                        if (dispatcher.failSafeActive)
                            dispatcher.clearFailSafe()
                        else
                            dispatcher.emergencyStop()
                    }
                }
                Button {
                                    text: dispatcher.startupInProgress ? "⏳ Başlatılıyor..." : "▶ Sıralı Başlat"
                                    Layout.fillWidth: true
                                    enabled: !dispatcher.startupInProgress && !dispatcher.failSafeActive
                                    onClicked: dispatcher.startupSequence()
                                }

                Text {
                                            text: ecu.voltage > 0 ? "Voltaj: " + ecu.voltage.toFixed(1) + " V" : ""
                                            visible: ecu.voltage > 0
                                            color: "#B8C4E0"
                                            font.pixelSize: 13
                                        }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#3A4A7A"
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: dispatcher.logMessages
                    spacing: 4

                    delegate: Text {
                        width: ListView.view.width
                        text: modelData
                        color: modelData.indexOf("[KRİTİK]") === 0 ? "#DC2626"
                             : modelData.indexOf("[HATA]") === 0   ? "#EF4444"
                             : modelData.indexOf("[UYARI]") === 0  ? "#F59E0B"
                             :                                        "#B8C4E0"
                        font.pixelSize: 12
                        font.family: "Consolas"
                        font.bold: modelData.indexOf("[HATA]") === 0
                                 || modelData.indexOf("[KRİTİK]") === 0
                        wrapMode: Text.Wrap
                    }

                    onCountChanged: positionViewAtEnd()
                }
            }
        }
    }

    // Fail-safe aktifken ekranın üstünde kırmızı uyarı bandı
    Rectangle {
        visible: dispatcher.failSafeActive
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 44
        color: "#DC2626"

        Text {
            anchors.centerIn: parent
            text: "⚠  SİSTEM FAIL-SAFE MODUNDA — TÜM ECU'LAR DURDURULDU  ⚠"
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }
    }
}