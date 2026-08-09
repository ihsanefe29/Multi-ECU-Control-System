import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    implicitHeight: 40

    // Seçili mod artık dışarıdan (controller'dan) gelir, burada tutulmaz.
    // "OFF" | "ON" | "KILL" | "" (hiçbiri)
    property string currentMode: ""

    // Kullanıcı bir moda basınca dışarı haber ver; ne yapılacağına dışarısı karar verir.
    signal modeRequested(string mode)

    function tryChange(target) {
        // ON <-> KILL doğrudan geçiş yasak, arada OFF şart
        if ((root.currentMode === "ON"   && target === "KILL") ||
            (root.currentMode === "KILL" && target === "ON")) {
            return
        }
        root.modeRequested(target)
    }

    RowLayout {
        anchors.fill: parent
        spacing: 6

        Repeater {
            model: ["OFF", "ON", "KILL"]

            Button {
                text: modelData
                Layout.fillWidth: true
                Layout.fillHeight: true
                highlighted: root.currentMode === modelData   // dışarıdan gelen moda göre
                onClicked: root.tryChange(modelData)
            }
        }
    }
}