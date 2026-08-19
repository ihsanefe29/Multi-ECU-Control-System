import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

Rectangle {
    id: root
    property var    session
    property string accentColor: "#3b82f6"
    property string displayTitle: session
        ? ("ECU " + session.ecuId + " — Modül " + session.moduleNumber) : ""

    signal dataViewSelected()
    signal firmwareUploadSelected()
    signal parameterPageSelected()

    radius: 12
    color:  "#0d1829"
    border.width: 1
    border.color: accentColor

    // Hafif iç parıltı
    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        radius: parent.radius - 1
        color: "transparent"
        border.width: 1
        border.color: Qt.rgba(
            parseInt(root.accentColor.slice(1,3),16)/255,
            parseInt(root.accentColor.slice(3,5),16)/255,
            parseInt(root.accentColor.slice(5,7),16)/255,
            0.18)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // ── Başlık satırı ─────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            // Renkli badge
            Rectangle {
                radius: 6
                color: root.accentColor
                implicitWidth:  badgeText.implicitWidth + 18
                implicitHeight: 28
                Text {
                    id: badgeText
                    anchors.centerIn: parent
                    text: root.displayTitle
                    color: "white"
                    font.pixelSize: 13; font.bold: true
                }
            }

            Item { Layout.fillWidth: true }

            // LED göstergesi
            LedIndicator {
                Layout.alignment: Qt.AlignVCenter
                state: session ? session.ledState : "inactive"
            }

            // Bağlan / Bağlantıyı Kes
            Button {
                id: connectBtn
                text: session && session.connected ? "Bağlantıyı Kes" : "Bağlan"
                implicitHeight: 28
                onClicked: {
                    if (!session) return
                    if (session.connected)
                        Dispatcher.disconnectModule(session.ecuId, session.moduleNumber)
                    else
                        Dispatcher.connectModule(session.ecuId, session.moduleNumber)
                }
                background: Rectangle {
                    radius: 6
                    color: connectBtn.pressed  ? Qt.darker (root.accentColor, 1.4)
                         : connectBtn.hovered  ? Qt.lighter(root.accentColor, 1.25)
                         : Qt.rgba(
                               parseInt(root.accentColor.slice(1,3),16)/255,
                               parseInt(root.accentColor.slice(3,5),16)/255,
                               parseInt(root.accentColor.slice(5,7),16)/255, 0.25)
                    border.color: root.accentColor; border.width: 1
                    Behavior on color { ColorAnimation { duration: 120 } }
                }
                contentItem: Text {
                    text: connectBtn.text; color: "white"
                    font.pixelSize: 11; font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:   Text.AlignVCenter
                }
            }
        }

        // Ayırıcı çizgi
        Rectangle {
            Layout.fillWidth: true; height: 1
            color: root.accentColor; opacity: 0.25
        }

        // ── Gövde ─────────────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            // Sol: mod seçimi + gauge + durum
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 6

                // Mod seçici
                RowLayout {
                    Layout.fillWidth: true; spacing: 6
                    Text {
                        text: "Mod:"
                        color: "#64748b"; font.pixelSize: 12
                        Layout.alignment: Qt.AlignVCenter
                    }
                    ComboBox {
                        id: modeCombo
                        Layout.fillWidth: true
                        implicitHeight: 30
                        model: ["Veri Görüntüleme", "Yazılım Yükleme", "Parametre Kontrolü"]
                        onActivated: {
                            if (!session) return
                            if (index === 0) {
                                Dispatcher.setMode(session.ecuId, session.moduleNumber, "DataView")
                                root.dataViewSelected()
                            } else if (index === 1) {
                                Dispatcher.setMode(session.ecuId, session.moduleNumber, "FirmwareUpload")
                                root.firmwareUploadSelected()
                            } else {
                                root.parameterPageSelected()
                            }
                        }
                        background: Rectangle {
                            color: "#0f1a2e"; radius: 6
                            border.color: "#334155"; border.width: 1
                        }
                        contentItem: Text {
                            leftPadding: 8
                            text: modeCombo.displayText
                            color: "#cbd5e1"; font.pixelSize: 12
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                // Seçilebilir metrik gauge
                RpmGauge {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    accentColor: root.accentColor
                }

                // Durum satırı
                Text {
                    text: "Bağlantı: %1  |  Güç: %2"
                        .arg(session && session.connected ? "Bağlı ✓" : "Bağlı değil")
                        .arg(session ? session.powerState : "—")
                    color: "#475569"; font.pixelSize: 11
                    Layout.fillWidth: true; wrapMode: Text.WordWrap
                }

                // Firmware etiketi
                Text {
                    id: firmwareLabel
                    property string fileName: ""
                    text: fileName.length > 0
                          ? "Firmware: " + fileName
                          : "Firmware: seçilmedi"
                    color: "#475569"; font.pixelSize: 11
                    Layout.fillWidth: true; elide: Text.ElideRight

                    function refresh() {
                        if (session)
                            fileName = Dispatcher.firmwareFileName(session.ecuId, session.moduleNumber)
                    }
                    Component.onCompleted: refresh()
                    Connections {
                        target: Dispatcher
                        function onFirmwareChanged(ecuId, moduleNumber) {
                            if (session && ecuId === session.ecuId
                                        && moduleNumber === session.moduleNumber)
                                firmwareLabel.refresh()
                        }
                    }
                }
            }

            // Sağ: güç anahtarı
            ColumnLayout {
                spacing: 4
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                Text {
                    text: "Güç"; color: "#64748b"; font.pixelSize: 11
                    Layout.alignment: Qt.AlignHCenter
                }
                PowerSwitchItem {
                    state: session ? session.powerState : "Off"
                    onRequestState: {
                        if (session)
                            Dispatcher.setPower(session.ecuId, session.moduleNumber, newState)
                    }
                }
            }
        }
    }
}
