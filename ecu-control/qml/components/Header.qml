import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    Layout.fillWidth: true
    implicitHeight: 80

    color: "#232830"

    border.color: "#3A404B"
    border.width: 1

    radius: 10

    property string currentTime: Qt.formatTime(new Date(), "HH:mm:ss")
    property string currentDate: Qt.formatDate(new Date(), "dd MMM yyyy")

    Timer {
        interval: 1000
        running: true
        repeat: true

        onTriggered: {
            root.currentTime = Qt.formatTime(new Date(), "HH:mm:ss")
            root.currentDate = Qt.formatDate(new Date(), "dd MMM yyyy")
        }
    }

    RowLayout {

        anchors.fill: parent

        anchors.leftMargin: 25
        anchors.rightMargin: 25

        ColumnLayout {

            spacing: 2

            Label {

                text: "ECU CONTROL PANEL"

                color: "white"

                font.pixelSize: 28

                font.bold: true
            }

            Label {

                text: "Electronic Control Unit Management"

                color: "#9AA4B2"

                font.pixelSize: 13
            }

        }

        Item {

            Layout.fillWidth: true

        }

        ColumnLayout {

            spacing: 2

            Label {

                text: root.currentDate

                color: "#C9D1D9"

                font.pixelSize: 13

                horizontalAlignment: Text.AlignRight
            }

            Label {

                text: root.currentTime

                color: "#58A6FF"

                font.pixelSize: 24

                font.bold: true

                horizontalAlignment: Text.AlignRight
            }

        }

    }

}