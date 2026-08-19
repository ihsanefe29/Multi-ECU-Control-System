import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Parametre kontrol ekranı (esra_parameter-page branch).
// N1, EGT, Fuel Flow, Bleed Valve, Vibration parametrelerini
// slider / ComboBox ile gösterir ve Dispatcher.parameterModel'e yazar.
Item {
    id: root
    signal backRequested()

    // Parametre tanımları — tek yerden yönetilir
    readonly property var paramDefs: [
        { name: "N1",          type: "slider", unit: "%",     min: 0,   max: 105,  step: 1,   defaultValue: 0   },
        { name: "EGT",         type: "slider", unit: "°C",    min: 300, max: 950,  step: 1,   defaultValue: 300 },
        { name: "Fuel Flow",   type: "slider", unit: "kg/h",  min: 200, max: 2500, step: 1,   defaultValue: 200 },
        { name: "Bleed Valve", type: "enum",   unit: "",      options: ["Kapalı", "Kısmi", "Açık"] },
        { name: "Vibration",   type: "slider", unit: "",      min: 0.0, max: 5.0,  step: 0.1, defaultValue: 0.0 }
    ]

    Component.onCompleted: {
        var names = []
        for (var i = 0; i < paramDefs.length; i++)
            names.push(paramDefs[i].name)
        Dispatcher.parameterModel.setParameterOrder(names)
    }

    // ── Header ─────────────────────────────────────────────────────────────
    RowLayout {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        spacing: 12

        Button {
            text: qsTr("< Geri")
            onClicked: root.backRequested()
        }
        Text {
            text: qsTr("Parametre Kontrolü")
            color: "#e8ebee"
            font.pixelSize: 20
            font.bold: true
        }
        Item { Layout.fillWidth: true }

        // DataSimulator durumu
        Text {
            text: qsTr("Simülatör aktif — 500ms'de bir sahte veri üretiliyor")
            color: "#2fbf71"
            font.pixelSize: 11
        }
    }

    // ── İçerik ─────────────────────────────────────────────────────────────
    ScrollView {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 16
        clip: true

        ColumnLayout {
            width: parent.width
            spacing: 16

            Repeater {
                model: root.paramDefs

                delegate: Rectangle {
                    Layout.fillWidth: true
                    height: paramContent.implicitHeight + 24
                    radius: 8
                    color: "#1e2226"
                    border.width: 1
                    border.color: "#2c3238"

                    ColumnLayout {
                        id: paramContent
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        // Parametre adı + değer
                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                text: modelData.name
                                color: "#e8ebee"
                                font.pixelSize: 14
                                font.bold: true
                            }
                            Item { Layout.fillWidth: true }
                            Text {
                                id: valueLabel
                                color: "#2f80e6"
                                font.pixelSize: 13
                                font.bold: true
                                text: {
                                    var v = Dispatcher.parameterModel.getValue(modelData.name)
                                    if (v === undefined || v === "") return "—"
                                    return (modelData.unit ? v + " " + modelData.unit : v)
                                }
                            }
                        }

                        // Slider
                        Slider {
                            visible: modelData.type === "slider"
                            Layout.fillWidth: true
                            from:  modelData.min  !== undefined ? modelData.min  : 0
                            to:    modelData.max  !== undefined ? modelData.max  : 100
                            stepSize: modelData.step !== undefined ? modelData.step : 1
                            value: {
                                var v = Dispatcher.parameterModel.getValue(modelData.name)
                                return (v !== undefined && v !== "") ? v : (modelData.defaultValue || 0)
                            }
                            onMoved: {
                                Dispatcher.parameterModel.setValue(modelData.name, value)
                                Dispatcher.addLog(
                                    Qt.formatTime(new Date(), "hh:mm:ss") +
                                    "  Parametre güncellendi: " + modelData.name +
                                    " = " + value.toFixed(modelData.step < 1 ? 1 : 0) +
                                    (modelData.unit ? " " + modelData.unit : "")
                                )
                            }
                        }

                        // ComboBox (enum tipi)
                        ComboBox {
                            visible: modelData.type === "enum"
                            model: modelData.options || []
                            Layout.preferredWidth: 200
                            onActivated: {
                                Dispatcher.parameterModel.setValue(modelData.name, currentText)
                                Dispatcher.addLog(
                                    Qt.formatTime(new Date(), "hh:mm:ss") +
                                    "  Parametre güncellendi: " + modelData.name + " = " + currentText
                                )
                            }
                        }
                    }
                }
            }

            // DataSimulator canlı veri gösterimi
            Rectangle {
                Layout.fillWidth: true
                height: simContent.implicitHeight + 24
                radius: 8
                color: "#1a2820"
                border.width: 1
                border.color: "#2a3a2a"

                ColumnLayout {
                    id: simContent
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 6

                    Text {
                        text: qsTr("DataSimulator — Canlı Paket")
                        color: "#2fbf71"
                        font.pixelSize: 13
                        font.bold: true
                    }
                    Text {
                        id: simStatus
                        text: qsTr("500ms'de bir paket yayınlanıyor…")
                        color: "#9aa4ad"
                        font.pixelSize: 11
                    }
                }

                // DataSimulator'dan gelen paketi yakala
                Connections {
                    target: Dispatcher.dataSimulator
                    function onDataReady(data) {
                        simStatus.text = qsTr("Son paket: %1 byte — %2")
                            .arg(data.length)
                            .arg(Qt.formatTime(new Date(), "hh:mm:ss.zzz"))
                    }
                }
            }

            Item { height: 16 }
        }
    }
}
