// PowerSwitchItem.qml — Güç anahtarı (Off / On)
// Sinyal: onRequestState(newState) — "Off" veya "On"
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    implicitWidth: 52
    implicitHeight: 90

    property string state: "Off"   // "Off" | "On"

    signal requestState(string newState)

    readonly property bool isOn: root.state === "On"

    Column {
        anchors.fill: parent
        spacing: 0

        // ON butonu
        Rectangle {
            id: onBtn
            width: parent.width
            height: parent.height / 2
            radius: 8
            color: root.isOn
                   ? "#16a34a"
                   : (onArea.containsMouse ? "#14532d" : "#0f2318")
            border.color: root.isOn ? "#22c55e" : "#1c3829"
            border.width: 1

            Behavior on color { ColorAnimation { duration: 130 } }

            Text {
                anchors.centerIn: parent
                text: "ON"
                color: root.isOn ? "white" : "#4ade80"
                font.pixelSize: 12; font.bold: true; font.letterSpacing: 1
            }

            // Aktifken üst parıltı çizgisi
            Rectangle {
                visible: root.isOn
                anchors { top: parent.top; left: parent.left; right: parent.right; topMargin: 3; leftMargin: 8; rightMargin: 8 }
                height: 2; radius: 1
                color: "#4ade80"; opacity: 0.6
            }

            MouseArea {
                id: onArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: if (!root.isOn) root.requestState("On")
                cursorShape: !root.isOn ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }

        // OFF butonu
        Rectangle {
            id: offBtn
            width: parent.width
            height: parent.height / 2
            radius: 8
            color: !root.isOn
                   ? "#7f1d1d"
                   : (offArea.containsMouse ? "#2d0f0f" : "#1a0a0a")
            border.color: !root.isOn ? "#ef4444" : "#3d1515"
            border.width: 1

            Behavior on color { ColorAnimation { duration: 130 } }

            Text {
                anchors.centerIn: parent
                text: "OFF"
                color: !root.isOn ? "white" : "#f87171"
                font.pixelSize: 12; font.bold: true; font.letterSpacing: 1
            }

            // Aktifken alt parıltı çizgisi
            Rectangle {
                visible: !root.isOn
                anchors { bottom: parent.bottom; left: parent.left; right: parent.right; bottomMargin: 3; leftMargin: 8; rightMargin: 8 }
                height: 2; radius: 1
                color: "#f87171"; opacity: 0.6
            }

            MouseArea {
                id: offArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: if (root.isOn) root.requestState("Off")
                cursorShape: root.isOn ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
