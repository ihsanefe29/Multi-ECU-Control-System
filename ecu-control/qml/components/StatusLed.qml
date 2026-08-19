import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {

    id: root

    property string status: "Offline"

    spacing: 8

    Rectangle {

        width: 14
        height: 14

        radius: width / 2

        color: {

            switch (root.status) {

            case "Ready":
                return "#4CAF50"

            case "Standby":
                return "#FFC107"

            case "Powered":
                return "#2196F3"

            case "Emergency":
                return "#F44336"

            default:
                return "#7A7A7A"
            }
        }

        border.width: 1
        border.color: Qt.darker(color, 1.3)
    }

    Label {

        text: root.status.toUpperCase()

        color: "#D6D6D6"

        font.pixelSize: 13

        font.bold: true
    }

}