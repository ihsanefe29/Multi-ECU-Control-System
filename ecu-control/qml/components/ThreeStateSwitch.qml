import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GroupBox {

    id: root

    property string state: "OFF"

    title: "Switch"

    Layout.fillWidth: true
    Layout.preferredHeight: 85

    background: Rectangle {
        color: "#252A33"
        radius: 8
        border.color: "#4A5563"
        border.width: 1
    }

    label: Label {
        text: parent.title
        color: "#D6D6D6"
        font.bold: true
        padding: 0
    }

    ButtonGroup {
        id: switchGroup
    }

    RowLayout {

        anchors.fill: parent
        anchors.margins: 6

        spacing: 10

        RadioButton {

            text: "OFF"

            checked: root.state === "OFF"
            enabled: root.state !== "KILL"

            ButtonGroup.group: switchGroup

            onClicked: {

                if (root.state !== "KILL") {

                    root.state = "OFF"

                }

            }

            contentItem: Text {

                text: parent.text

                color: "white"

                font.bold: true

                font.pixelSize: 13

                verticalAlignment: Text.AlignVCenter

                leftPadding: parent.indicator.width + parent.spacing

            }

        }

        RadioButton {

            text: "ON"

            checked: root.state === "ON"
            enabled: root.state !== "KILL"

            ButtonGroup.group: switchGroup

            onClicked: {

                if (root.state !== "KILL") {

                    root.state = "ON"

                }

            }

            contentItem: Text {

                text: parent.text

                color: "white"

                font.bold: true

                font.pixelSize: 13

                verticalAlignment: Text.AlignVCenter

                leftPadding: parent.indicator.width + parent.spacing

            }

        }

        RadioButton {

            text: "KILL"

            checked: root.state === "KILL"

            ButtonGroup.group: switchGroup

            onClicked: {

                root.state = "KILL"

            }

            contentItem: Text {

                text: parent.text

                color: "#FF5252"

                font.bold: true

                font.pixelSize: 13

                verticalAlignment: Text.AlignVCenter

                leftPadding: parent.indicator.width + parent.spacing

            }

        }

    }

}