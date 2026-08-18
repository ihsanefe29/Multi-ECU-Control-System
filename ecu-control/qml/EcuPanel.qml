import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

// One panel = one ModuleSession. `session` is one of
// Dispatcher.ecuAModule1 / ecuAModule2 / ecuBModule1 / ecuBModule2.
//
// Every user action here goes back through `Dispatcher`, never
// touches `session`'s slots directly - keeps QML's only contact
// point with the backend at the Dispatcher, matching the "UI
// Connector is a mediator" requirement.
Rectangle {
    id: root
    property var session
    property string displayTitle: session ? ("ECU " + session.ecuId + " — Modül " + session.moduleNumber) : ""

    // Emitted whenever the user picks "Veri Görüntüleme" in the mode
    // combo - fired unconditionally on every such selection, not
    // gated on whether the backend's mode value actually changed
    // (mode defaults to DataView already, so a same-value reselect
    // must still navigate to the chart screen).
    signal dataViewSelected()

    // Same idea, for "Yazılım Yükleme" - fired unconditionally on
    // every selection of that mode.
    signal firmwareUploadSelected()
    signal parameterPageSelected()   // esra branch: parametre sayfasına git

    radius: 10
    color: "#1e2226"
    border.width: 1
    border.color: "#2c3238"

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        RowLayout {
            width: parent.width
            spacing: 8

            Text {
                text: root.displayTitle
                color: "#e8ebee"
                font.pixelSize: 15
                font.bold: true
            }

            // Fills whatever space is left, however wide the title/
            // LED/button actually render at under the active style -
            // no more guessing a fixed pixel gap.
            Item { Layout.fillWidth: true }

            LedIndicator {
                Layout.alignment: Qt.AlignVCenter
                state: session ? session.ledState : "inactive"
            }

            Button {
                text: session && session.connected ? qsTr("Bağlantıyı Kes") : qsTr("Bağlan")
                onClicked: {
                    if (!session) return
                    if (session.connected)
                        Dispatcher.disconnectModule(session.ecuId, session.moduleNumber)
                    else
                        Dispatcher.connectModule(session.ecuId, session.moduleNumber)
                }
            }
        }

        Rectangle { width: parent.width; height: 1; color: "#2c3238" }

        RowLayout {
            width: parent.width
            spacing: 16

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10

                RowLayout {
                    spacing: 8
                    Text { text: qsTr("Mod:"); color: "#9aa4ad"; Layout.alignment: Qt.AlignVCenter }
                    ComboBox {
                        // esra branch: Parametre Kontrolü eklendi
                        model: [qsTr("Veri Görüntüleme"), qsTr("Yazılım Yükleme"), qsTr("Parametre Kontrolü")]
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
                    }
                }

                RpmGauge {
                    // Placeholder value until live CAN FD/CCP
                    // measurement data is wired in - structure is
                    // ready, data source isn't yet.
                    value: 0
                }

                Text {
                    text: qsTr("Bağlantı: %1  |  Mod: %2  |  Güç: %3")
                        .arg(session && session.connected ? qsTr("Bağlı") : qsTr("Bağlı değil"))
                        .arg(session ? session.mode : "")
                        .arg(session ? session.powerState : "")
                    color: "#9aa4ad"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Text {
                    id: firmwareLabel
                    property string fileName: ""
                    text: fileName.length > 0 ? qsTr("Firmware: %1").arg(fileName) : qsTr("Firmware: seçilmedi")
                    color: "#9aa4ad"
                    font.pixelSize: 12
                    Layout.fillWidth: true

                    function refresh() {
                        if (session)
                            fileName = Dispatcher.firmwareFileName(session.ecuId, session.moduleNumber)
                    }

                    Component.onCompleted: refresh()

                    Connections {
                        target: Dispatcher
                        function onFirmwareChanged(ecuId, moduleNumber) {
                            if (session && ecuId === session.ecuId && moduleNumber === session.moduleNumber)
                                firmwareLabel.refresh()
                        }
                    }
                }
            }

            ColumnLayout {
                spacing: 4
                Text {
                    text: qsTr("Güç")
                    color: "#9aa4ad"
                    font.pixelSize: 11
                    Layout.alignment: Qt.AlignHCenter
                }
                PowerSwitchItem {
                    state: session ? session.powerState : "Off"
                    // Qt 5.15: sinyal parametresi 'newState' adıyla
                    // otomatik gelir, arrow function gerekmez
                    onRequestState: {
                        if (session)
                            Dispatcher.setPower(session.ecuId, session.moduleNumber, newState)
                    }
                }
            }
        }
    }
}
