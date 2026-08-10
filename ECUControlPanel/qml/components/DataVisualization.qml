import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15

Rectangle {

    id: root

    color: "#1B1F24"

    property int sampleIndex: 0
    property int maxSamples: 100


    // =========================================================
    // PARAMETRE BİLGİSİ
    // =========================================================

    function getParameterInfo() {

        return rawDataParser.parameterInfo(
            parameterCombo.currentText
        )
    }


    // =========================================================
    // PARAMETRE DEĞERİ
    // =========================================================

    function getCurrentValue() {

        var value =
            rawDataParser.values[
                parameterCombo.currentText
            ]

        if (value === undefined ||
            value === null)
        {
            return "--"
        }

        return value
    }


    ColumnLayout {

        anchors.fill: parent

        anchors.margins: 20

        spacing: 15


        // =====================================================
        // PAGE HEADER
        // =====================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.preferredHeight: 70

            radius: 10

            color: "#232830"

            border.color: "#404854"

            border.width: 1


            RowLayout {

                anchors.fill: parent

                anchors.margins: 18


                ColumnLayout {

                    spacing: 2


                    Label {

                        text: "DATA VISUALIZATION"

                        color: "white"

                        font.pixelSize: 24

                        font.bold: true
                    }


                    Label {

                        text:
                            "ECU telemetry and parameter monitoring"

                        color: "#9AA4B2"

                        font.pixelSize: 13
                    }
                }


                Item {
                    Layout.fillWidth: true
                }


                Label {

                    text: "LIVE DATA"

                    color: "#4CAF50"

                    font.bold: true
                }
            }
        }


        // =====================================================
        // FILTERS
        // =====================================================

        Rectangle {

            Layout.fillWidth: true

            Layout.preferredHeight: 90

            radius: 10

            color: "#232830"

            border.color: "#404854"

            border.width: 1


            RowLayout {

                anchors.fill: parent

                anchors.margins: 15

                spacing: 15


                Label {

                    text: "ECU"

                    color: "#D6D6D6"

                    font.bold: true
                }


                ComboBox {

                    id: ecuCombo

                    Layout.preferredWidth: 180


                    model: [
                        "ECU A-1",
                        "ECU A-2",
                        "ECU B-1",
                        "ECU B-2"
                    ]
                }


                Label {

                    text: "Parameter"

                    color: "#D6D6D6"

                    font.bold: true
                }


                ComboBox {

                    id: parameterCombo

                    Layout.preferredWidth: 220


                    model: [
                        "RPM",
                        "Temperature",
                        "Voltage",
                        "Current",
                        "Power",
                        "Bus Voltage",
                        "Motor Temp",
                        "Inverter Temp"
                    ]


                    onCurrentTextChanged: {

                        dataSeries.clear()

                        root.sampleIndex = 0

                        xAxis.min = 0
                        xAxis.max = root.maxSamples
                    }
                }


                Item {
                    Layout.fillWidth: true
                }


                Button {

                    text: "Clear Graph"

                    onClicked: {

                        dataSeries.clear()

                        root.sampleIndex = 0

                        xAxis.min = 0
                        xAxis.max = root.maxSamples

                        yAxis.min = 0
                        yAxis.max = 100
                    }
                }
            }
        }


        // =====================================================
        // MAIN CONTENT
        // =====================================================

        RowLayout {

            Layout.fillWidth: true

            Layout.fillHeight: true

            spacing: 15


            // =================================================
            // GRAPH
            // =================================================

            Rectangle {

                Layout.fillWidth: true

                Layout.fillHeight: true

                radius: 10

                color: "#232830"

                border.color: "#404854"

                border.width: 1


                ColumnLayout {

                    anchors.fill: parent

                    anchors.margins: 15


                    Label {

                        text:
                            parameterCombo.currentText
                            + " — Time Series"

                        color: "white"

                        font.pixelSize: 18

                        font.bold: true
                    }


                    ChartView {

                        id: chartView

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        antialiasing: true

                        backgroundColor: "#1E242B"

                        legend.visible: false

                        margins.left: 15
                        margins.right: 15
                        margins.top: 10
                        margins.bottom: 10

                        ValueAxis {

                            id: xAxis

                            min: 0
                            max: 100

                            titleText: "Sample"
                        }

                        ValueAxis {

                            id: yAxis

                            min: 0
                            max: 100

                            titleText: parameterCombo.currentText
                        }

                        LineSeries {
                            id: dataSeries

                            axisX: xAxis
                            axisY: yAxis
                        }
                    }
                }
            }


            // =================================================
            // CURRENT VALUE
            // =================================================

            Rectangle {

                Layout.preferredWidth: 280

                Layout.fillHeight: true

                radius: 10

                color: "#232830"

                border.color: "#404854"

                border.width: 1


                ColumnLayout {

                    anchors.fill: parent

                    anchors.margins: 18


                    // -----------------------------------------
                    // TITLE
                    // -----------------------------------------

                    Label {

                        text: "Current Value"

                        color: "#D6D6D6"

                        font.pixelSize: 18

                        font.bold: true
                    }


                    // -----------------------------------------
                    // CURRENT VALUE BOX
                    // -----------------------------------------

                    Rectangle {

                        Layout.fillWidth: true

                        Layout.preferredHeight: 120

                        radius: 8

                        color: "#1E242B"


                        ColumnLayout {

                            anchors.centerIn: parent

                            spacing: 5


                            Label {

                                text:
                                    parameterCombo.currentText

                                color: "#9AA4B2"

                                font.pixelSize: 14

                                Layout.alignment:
                                    Qt.AlignHCenter
                            }


                            Label {

                                Layout.fillWidth: true

                                text: {

                                    var value = getCurrentValue()

                                    if (value === "--")
                                        return "--"

                                    if (typeof value === "number")
                                    {
                                        if (value % 1 === 0)
                                            return value.toFixed(0)

                                        return value.toFixed(3)
                                    }

                                    return value.toString()
                                }

                                color: "#4DA3FF"

                                font.pixelSize: 28
                                font.bold: true

                                horizontalAlignment: Text.AlignHCenter

                                elide: Text.ElideRight
                            }
                        }
                    }


                    // -----------------------------------------
                    // SEPARATOR
                    // -----------------------------------------

                    Rectangle {

                        Layout.fillWidth: true

                        height: 1

                        color: "#404854"
                    }


                    // -----------------------------------------
                    // PARAMETER INFORMATION
                    // -----------------------------------------

                    Label {

                        text: "Parameter Information"

                        color: "#D6D6D6"

                        font.bold: true
                    }


                    // ECU

                    Label {

                        text:
                            "ECU: "
                            + ecuCombo.currentText

                        color: "#9AA4B2"
                    }


                    // ADDRESS

                    Label {

                        text: {

                            var info =
                                root.getParameterInfo()

                            if (info.address === undefined)
                            {
                                return "Address: --"
                            }

                            return "Address: "
                                   + info.address
                        }

                        color: "#9AA4B2"
                    }


                    // WIDTH

                    Label {

                        text: {

                            var info =
                                root.getParameterInfo()

                            if (info.width === undefined)
                            {
                                return "Width: -- bytes"
                            }

                            return "Width: "
                                   + info.width
                                   + " bytes"
                        }

                        color: "#9AA4B2"
                    }


                    // TYPE

                    Label {

                        text: {

                            var info =
                                root.getParameterInfo()

                            if (info.type === undefined)
                            {
                                return "Type: --"
                            }

                            return "Type: "
                                   + info.type
                        }

                        color: "#9AA4B2"
                    }


                    // CONVERSION

                    Label {

                        text: {

                            var info =
                                root.getParameterInfo()

                            if (info.formula === undefined ||
                                info.formula === "-" ||
                                info.formula === "")
                            {
                                return "Conversion: None"
                            }

                            return "Conversion: "
                                   + info.formula
                        }

                        color: "#9AA4B2"

                        wrapMode:
                            Text.Wrap
                    }


                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }

    Timer {

        id: dataTimer

        interval: 500

        running: true

        repeat: true

        onTriggered: {

            var value = getCurrentValue()

            if (value === "--")
                return

            var numericValue = Number(value)

            if (isNaN(numericValue))
                return

            // Yeni veri noktasını ekle
            dataSeries.append(
                root.sampleIndex,
                numericValue
            )

            root.sampleIndex++

            // Maksimum 100 nokta tut
            if (dataSeries.count > root.maxSamples)
            {
                dataSeries.remove(0)
            }

            // X eksenini kaydır
            if (root.sampleIndex > root.maxSamples)
            {
                xAxis.min =
                    root.sampleIndex - root.maxSamples

                xAxis.max =
                    root.sampleIndex
            }

            // Y eksenini otomatik ayarla
            var minValue = numericValue
            var maxValue = numericValue

            for (var i = 0;
                 i < dataSeries.count;
                 i++)
            {
                var point =
                    dataSeries.at(i)

                if (point.y < minValue)
                    minValue = point.y

                if (point.y > maxValue)
                    maxValue = point.y
            }

            // Değerler aynıysa grafiğe biraz alan bırak
            if (minValue === maxValue)
            {
                var padding =
                    Math.abs(minValue) * 0.01

                if (padding < 1)
                    padding = 1

                yAxis.min =
                    minValue - padding

                yAxis.max =
                    maxValue + padding
            }
            else
            {
                var range =
                    maxValue - minValue

                padding =
                    range * 0.1

                yAxis.min =
                    minValue - padding

                yAxis.max =
                    maxValue + padding
            }
        }
    }

}