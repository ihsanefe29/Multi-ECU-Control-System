import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

// Shown when a specific module's mode is switched to "Yazılım
// Yükleme". Unlike SignalChartScreen, this IS tied to a specific
// module (firmware has to go to a particular ECU/module) - the
// target is passed in via ecuId/moduleNumber.
//
// This only stages a firmware file (holds it in the Dispatcher) - it
// does not perform a real CCP DNLOAD upload yet. That would be a
// separate step once ModuleWorker grows an actual flashing sequence.
//
// Staging requires the target module to already be connected -
// Dispatcher.selectFirmware() enforces this and refuses (returning
// false, emitting firmwareRejected) otherwise; the button here is
// also disabled preemptively so the common case never even reaches
// that rejection.
Item {
    id: root
    property string ecuId: ""
    property int moduleNumber: 0
    signal backRequested()

    property string stagedFileName: ""
    property real stagedSizeKB: 0
    property string rejectionMessage: ""

    readonly property var session: {
        if (root.ecuId === "A")
            return root.moduleNumber === 1 ? Dispatcher.ecuAModule1 : Dispatcher.ecuAModule2
        if (root.ecuId === "B")
            return root.moduleNumber === 1 ? Dispatcher.ecuBModule1 : Dispatcher.ecuBModule2
        return null
    }
    readonly property bool moduleConnected: session ? session.connected : false

    function refresh() {
        stagedFileName = Dispatcher.firmwareFileName(root.ecuId, root.moduleNumber)
        stagedSizeKB = Dispatcher.firmwareSizeBytes(root.ecuId, root.moduleNumber) / 1024.0
    }

    onEcuIdChanged: refresh()
    onModuleNumberChanged: refresh()
    Component.onCompleted: refresh()

    Connections {
        target: Dispatcher
        function onFirmwareChanged(ecuId, moduleNumber) {
            if (ecuId === root.ecuId && moduleNumber === root.moduleNumber) {
                root.rejectionMessage = ""
                root.refresh()
            }
        }
        function onFirmwareRejected(ecuId, moduleNumber, reason) {
            if (ecuId === root.ecuId && moduleNumber === root.moduleNumber)
                root.rejectionMessage = reason
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Button {
                text: qsTr("< Geri")
                onClicked: root.backRequested()
            }
            Text {
                text: qsTr("Yazılım Yükleme — ECU %1 Modül %2").arg(root.ecuId).arg(root.moduleNumber)
                color: "#e8ebee"
                font.pixelSize: 20
                font.bold: true
            }
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("Firmware Dosyası Seç")
                enabled: root.moduleConnected
                onClicked: firmwareFileDialog.open()
            }
        }

        Text {
            visible: !root.moduleConnected
            text: qsTr("Bu modül bağlı değil - firmware seçmeden önce kontrol panelinden \"Bağlan\"a basmalısınız.")
            color: "#e6a23c"
            font.pixelSize: 12
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Text {
            visible: root.rejectionMessage.length > 0
            text: root.rejectionMessage
            color: "#e6483a"
            font.pixelSize: 12
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 140
            radius: 10
            color: "#1e2226"
            border.width: 1
            border.color: "#2c3238"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8

                Text {
                    text: qsTr("Seçili Firmware")
                    color: "#e8ebee"
                    font.pixelSize: 14
                    font.bold: true
                }

                Text {
                    text: root.stagedFileName.length > 0
                        ? qsTr("Dosya: %1").arg(root.stagedFileName)
                        : qsTr("Henüz bir firmware dosyası seçilmedi.")
                    color: "#9aa4ad"
                    font.pixelSize: 13
                }

                Text {
                    visible: root.stagedFileName.length > 0
                    text: qsTr("Boyut: %1 KB").arg(root.stagedSizeKB.toFixed(1))
                    color: "#9aa4ad"
                    font.pixelSize: 13
                }

                Text {
                    visible: root.stagedFileName.length > 0
                    text: qsTr("Durum: Yüklemeye hazır (gerçek CCP DNLOAD adımı henüz bağlanmadı)")
                    color: "#e6a23c"
                    font.pixelSize: 12
                }
            }
        }

        Item { Layout.fillHeight: true }

        FileDialog {
            id: firmwareFileDialog
            title: qsTr("Firmware Dosyası Seç")
            nameFilters: [qsTr("Firmware Dosyaları (*.bin *.hex)"), qsTr("Tüm Dosyalar (*)")]
            onAccepted: Dispatcher.selectFirmware(root.ecuId, root.moduleNumber, file)
        }
    }
}
