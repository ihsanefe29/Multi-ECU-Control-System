import QtQuick 2.15

// Deliberately simple for now, per requirements: "basit bir label
// olarak göstermesini sağlayacağız" - a real dial/vertical gauge can
// replace the Text below later without changing how callers bind to
// `value`.
Item {
    id: root
    property real value: 0
    property string unit: "RPM"

    implicitWidth: label.implicitWidth
    implicitHeight: label.implicitHeight

    Text {
        id: label
        anchors.centerIn: parent
        color: "#e8ebee"
        font.pixelSize: 16
        font.bold: true
        text: root.value.toFixed(0) + " " + root.unit
    }
}
