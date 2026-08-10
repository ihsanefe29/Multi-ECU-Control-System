import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

Rectangle {

    id: root

    color: "#1B1F24"

    property string uploadState: "READY"
    property string selectedFile: ""
    property int progressValue: 0
    property bool ecuSafe: false

    Connections {

        target: ecuManager

        function onSafetyStateChanged(safe) {
            root.ecuSafe = safe
        }
    }

    ColumnLayout {

        anchors.fill: parent
        anchors.margins: 30
        spacing: 20

        // TITLE
        ColumnLayout {

            Layout.fillWidth: true
            spacing: 5

            Label {
                text: "Software Upload"
                color: "white"
                font.pixelSize: 26
                font.bold: true
            }

            Label {
                text: "ECU firmware management"
                color: "#9AA3AD"
                font.pixelSize: 14
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#404854"
        }

        // ECU SELECTION
        ColumnLayout {

            Layout.fillWidth: true
            spacing: 7

            Label {
                text: "ECU"
                color: "#D6D6D6"
                font.bold: true
            }

            ComboBox {

                id: ecuCombo

                Layout.fillWidth: true

                model: [
                    "ECU A-1",
                    "ECU A-2",
                    "ECU B-1",
                    "ECU B-2"
                ]
            }

            Rectangle {

                Layout.fillWidth: true
                height: 45

                radius: 6

                color: root.ecuSafe
                       ? "#203A28"
                       : "#3A2225"

                border.color:
                    root.ecuSafe
                    ? "#4CAF50"
                    : "#F44336"

                Label {

                    anchors.centerIn: parent

                    text:
                        root.ecuSafe
                        ? "✓ ECU SAFE - Ready for software upload"
                        : "⚠ ECU NOT SAFE - Software upload unavailable"

                    color:
                        root.ecuSafe
                        ? "#4CAF50"
                        : "#FF5252"

                    font.bold: true
                }
            }
        }

        // FIRMWARE FILE
        ColumnLayout {

            Layout.fillWidth: true
            spacing: 7

            Label {
                text: "Firmware File"
                color: "#D6D6D6"
                font.bold: true
            }

            RowLayout {

                Layout.fillWidth: true
                spacing: 10

                TextField {

                    Layout.fillWidth: true

                    readOnly: true

                    placeholderText: "No file selected"

                    text: root.selectedFile

                    color: "white"
                    placeholderTextColor: "#777F88"

                    background: Rectangle {

                        color: "#2A2F36"
                        radius: 6
                        border.color: "#404854"
                    }
                }

                Button {

                    text: "Browse"

                    onClicked: {

                        fileDialog.open()

                    }
                }
            }
        }

        // FILE INFORMATION
        Rectangle {

            Layout.fillWidth: true

            height: 120

            radius: 8

            color: "#242930"

            border.color: "#404854"

            ColumnLayout {

                anchors.fill: parent
                anchors.margins: 15

                spacing: 8

                Label {
                    text: "Firmware Information"
                    color: "white"
                    font.bold: true
                }

                Label {
                    text: "Version:     --"
                    color: "#B8C0C8"
                }

                Label {
                    text: "File Size:   --"
                    color: "#B8C0C8"
                }

                Label {
                    text: "Checksum:    --"
                    color: "#B8C0C8"
                }
            }
        }

        // STATUS
        Rectangle {

            Layout.fillWidth: true

            height: 55

            radius: 8

            color: root.uploadState === "ERROR"
                   ? "#3A2225"
                   : "#242930"

            border.color:
                root.uploadState === "ERROR"
                ? "#F44336"
                : "#404854"

            Label {

                anchors.centerIn: parent

                text: {

                    if (root.uploadState === "READY")
                        return "✓ READY TO UPLOAD"

                    if (root.uploadState === "UPLOADING")
                        return "Uploading firmware..."

                    if (root.uploadState === "SUCCESS")
                        return "✓ Upload completed successfully"

                    if (root.uploadState === "ERROR")
                        return "⚠ Upload failed"

                    return ""
                }

                color:
                    root.uploadState === "ERROR"
                    ? "#FF5252"
                    : "#D6D6D6"

                font.bold: true
            }
        }

        // UPLOAD BUTTON
        Button {

            Layout.alignment: Qt.AlignHCenter

            Layout.preferredWidth: 220
            Layout.preferredHeight: 45

            text: "Upload Firmware"

            enabled:
                root.selectedFile !== ""
                && root.ecuSafe
                && root.uploadState !== "UPLOADING"

            onClicked: {

                root.uploadState = "UPLOADING"
                root.progressValue = 0

                uploadTimer.start()
            }
        }

        // PROGRESS
        ColumnLayout {

            Layout.fillWidth: true

            spacing: 8

            Label {

                text: "Upload Progress"

                color: "#D6D6D6"

                font.bold: true
            }

            ProgressBar {

                Layout.fillWidth: true

                from: 0
                to: 100

                value: root.progressValue
            }

            Label {

                Layout.alignment: Qt.AlignRight

                text: root.progressValue + "%"

                color: "#B8C0C8"
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    // FILE DIALOG
    FileDialog {

        id: fileDialog

        title: "Select Firmware File"

        nameFilters: [
            "Firmware files (*.bin *.hex)",
            "Binary files (*.bin)",
            "HEX files (*.hex)",
            "All files (*)"
        ]

        onAccepted: {

            root.selectedFile = fileDialog.fileUrl.toString()

            root.uploadState = "READY"

        }
    }

    // TEST UPLOAD TIMER
    Timer {

        id: uploadTimer

        interval: 100

        repeat: true

        onTriggered: {

            root.progressValue += 5

            if (root.progressValue >= 100) {

                root.progressValue = 100

                root.uploadState = "SUCCESS"

                uploadTimer.stop()
            }
        }
    }
}