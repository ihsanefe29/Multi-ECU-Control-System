// LedIndicator.qml — Durum LED'i
// state: "active" | "inactive" | "warning" | "error"
import QtQuick 2.15

Item {
    id: root
    implicitWidth: 14
    implicitHeight: 14

    property string state: "inactive"

    readonly property color _core: state === "active"   ? "#22c55e"
                                 : state === "warning"  ? "#f59e0b"
                                 : state === "error"    ? "#ef4444"
                                 :                        "#334155"

    readonly property color _glow: state === "active"   ? "#16a34a"
                                 : state === "warning"  ? "#d97706"
                                 : state === "error"    ? "#b91c1c"
                                 :                        "transparent"

    // Dış parıltı halkası
    Rectangle {
        anchors.centerIn: parent
        width: parent.width * 1.9
        height: width
        radius: width / 2
        color: "transparent"
        border.color: root._glow
        border.width: 1
        opacity: state !== "inactive" ? 0.5 : 0

        SequentialAnimation on opacity {
            running: root.state === "active"
            loops: Animation.Infinite
            NumberAnimation { to: 0.6; duration: 900; easing.type: Easing.InOutSine }
            NumberAnimation { to: 0.1; duration: 900; easing.type: Easing.InOutSine }
        }
    }

    // Ana LED dairesi
    Rectangle {
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        radius: width / 2
        color: root._core

        // İç beyaz parıltı
        Rectangle {
            anchors { top: parent.top; topMargin: parent.height * 0.12; horizontalCenter: parent.horizontalCenter }
            width: parent.width * 0.4
            height: parent.height * 0.25
            radius: width / 2
            color: "white"
            opacity: root.state !== "inactive" ? 0.45 : 0.1
        }
    }
}
