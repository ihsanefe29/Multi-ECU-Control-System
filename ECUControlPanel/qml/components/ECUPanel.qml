import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {

    id: root

    property string title: "ECU"
    property int ecuId: 0

    property bool connected: false
    property bool powered: false
    property bool killed: false
    property string switchState: "OFF"

    signal logRequested(string level, string message)

    radius: 10
    color: "#2A2F36"

    border.color: "#404854"
    border.width: 1

    ColumnLayout {

        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // HEADER

        RowLayout {

            Layout.fillWidth: true

            Label {
                text: root.title
                color: "white"
                font.pixelSize: 22
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            StatusLed {

                status: {

                    if (root.killed)
                        return "Emergency"

                    if (!root.connected)
                        return "Offline"

                    if (root.powered)
                        return "Powered"

                    if (root.switchState === "ON")
                        return "Ready"

                    return "Standby"
                }

            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#404854"
        }

        // POWER

        RowLayout {

            Layout.fillWidth: true

            Label {
                text: "⚡"
                font.pixelSize: 18
            }

            Label {
                text: "Power"
                color: "#D6D6D6"
                font.bold: true
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: root.powered ? "ON" : "OFF"
                color: root.powered ? "#4CAF50" : "#F44336"
                font.bold: true
            }
        }

        // MODE

        ColumnLayout {

            Layout.fillWidth: true
            spacing: 5

            Label {
                text: "Mode"
                color: "#D6D6D6"
                font.bold: true
            }

            ComboBox {

                Layout.fillWidth: true

                model: [
                    "Normal",
                    "Test",
                    "Maintenance"
                ]

                onActivated: {

                    logRequested(
                        "INFO",
                        root.title + " Mode changed to " + currentText
                    )

                    uiCollector.modeRequest(
                        root.ecuId,
                        currentIndex
                    )

                }
            }
        }

        // ECU DATA

        Rectangle {

            Layout.fillWidth: true
            Layout.preferredHeight: 70

            color: "#20252B"
            radius: 8

            RowLayout {

                anchors.fill: parent
                anchors.margins: 10
                spacing: 15

                Label {
                    text: "RPM: " +
                          rawDataParser.values["RPM"]

                    color: "white"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                Label {
                    text: "Voltage: " +
                          rawDataParser.values["Voltage"]

                    color: "white"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                Label {
                    text: "Temp: " +
                          rawDataParser.values["Temperature"]

                    color: "white"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }
            }
        }

        // BUTTONS

        RowLayout {

            Layout.fillWidth: true
            spacing: 8

            Button {

                Layout.fillWidth: true

                text: "🔌 Connect"

                enabled: !root.connected && !root.killed

                onClicked: {

                    root.connected = true
                    root.switchState = "OFF"

                    logRequested(
                        "INFO",
                        root.title + " Connected"
                    )

                    uiCollector.connectRequest(root.ecuId)
                }
            }

            Button {

                Layout.fillWidth: true

                text: "⛔ Disconnect"

                enabled: root.connected && !root.killed

                onClicked: {

                    root.connected = false
                    root.powered = false
                    root.switchState = "OFF"

                    logRequested(
                        "INFO",
                        root.title + " Disconnected"
                    )

                    uiCollector.disconnectRequest(root.ecuId)
                }
            }

            Button {

                Layout.fillWidth: true

                text: "⚡ Power"

                enabled: root.connected
                         && root.switchState === "ON"
                         && !root.killed

                onClicked: {

                    root.powered = !root.powered

                    logRequested(
                        "INFO",
                        root.title + (root.powered
                                      ? " Power Enabled"
                                      : " Power Disabled")
                    )

                    uiCollector.powerRequest(root.ecuId)
                }
            }
        }

        // SWITCH

        ThreeStateSwitch {

            Layout.fillWidth: true

            state: root.switchState

            onStateChanged: {

                root.switchState = state

                if (state === "ON") {

                    logRequested(
                        "INFO",
                        root.title + " Switch ON"
                    )

                    uiCollector.switchOnRequest(root.ecuId)
                }

                else if (state === "OFF") {

                    root.killed = false

                    root.powered = false

                    logRequested(
                        "INFO",
                        root.title + " Switch OFF"
                    )

                    uiCollector.switchOffRequest(root.ecuId)
                }

                else if (state === "KILL") {

                    root.killed = true
                    root.connected = false
                    root.powered = false

                    logRequested(
                        "WARNING",
                        root.title + " Emergency Stop Activated"
                    )

                    uiCollector.killRequest(root.ecuId)
                }

            }
        }

        Label {

            visible: root.killed

            text: "⚠ EMERGENCY STOP ACTIVE"

            color: "#FF5252"

            font.bold: true

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
        }
    }
}