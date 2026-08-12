import QtQuick 2.15

// Visual ON/OFF/KILL switch. Deliberately dumb: clicking a zone just
// asks (calls onRequestState) for that state - it does NOT decide
// whether the transition is allowed. ModuleSession (C++) is the only
// place that rule lives; this component just reflects whatever
// `state` it's given after the fact. Clicking a rejected zone simply
// produces no visible change, since the bound state won't move.
Rectangle {
    id: root
    property string state: "Off" // "On" | "Off" | "Kill"
    signal requestState(string newState)

    width: 64
    height: 168
    radius: 14
    color: "#1b1f24"
    border.width: 1.5
    border.color: "#33393f"

    readonly property var zones: [
        { key: "On",   label: "ON",   activeColor: "#2fbf71" },
        { key: "Off",  label: "OFF",  activeColor: "#6b7280" },
        { key: "Kill", label: "KILL", activeColor: "#e6483a" }
    ]

    Column {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 0

        Repeater {
            model: root.zones
            delegate: Rectangle {
                required property var modelData
                width: root.width - 12
                height: (root.height - 12) / 3
                radius: 10
                color: root.state === modelData.key ? modelData.activeColor : "transparent"

                Text {
                    anchors.centerIn: parent
                    text: modelData.label
                    font.bold: true
                    font.pixelSize: 12
                    color: root.state === modelData.key ? "#0e1114" : "#8a9299"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.requestState(modelData.key)
                }
            }
        }
    }
}
