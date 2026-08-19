import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {

    id: root

    property alias model: logView.model

    Layout.fillWidth: true
    Layout.preferredHeight: 130

    radius: 10

    color: "#2A2F36"

    border.color: "#404854"
    border.width: 1

    ColumnLayout {

        anchors.fill: parent
        anchors.margins: 10
        spacing: 8

        Label {

            text: "System Log"

            color: "white"

            font.pixelSize: 18

            font.bold: true

        }

        Rectangle {

            Layout.fillWidth: true
            height: 1
            color: "#404854"

        }

        ListView {

            id: logView

            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true

            delegate: RowLayout {

                width: logView.width

                spacing: 10

                Label {

                    text: time

                    color: "#58A6FF"

                    font.pixelSize: 12

                    Layout.preferredWidth: 70

                }

                Label {

                    text: level

                    font.bold: true

                    color: level === "ERROR"
                           ? "#F44336"
                           : level === "WARNING"
                             ? "#FF9800"
                             : "#4CAF50"

                    Layout.preferredWidth: 80

                }

                Label {

                    text: message

                    color: "#DDDDDD"

                    Layout.fillWidth: true

                    elide: Text.ElideRight

                }

            }

        }

    }

}