import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import Qt.labs.platform 1.1

// "Veri Görüntüleme" screen with three data source modes:
//
//   1. Raw Data (new): loads a parameter-definition CSV and a raw hex
//      data file, cleans the hex (strip whitespace, pair→byte), parses
//      each parameter by address/offset/width/type, applies conversion
//      formulas, and shows the cleaned signal on the chart with all
//      parsed parameter values listed in the panel.
//
//   2. Live: mock streaming source, rolling 10-second window
//      (unchanged from the original).
//
// Only one mode is active at a time — starting one stops the other.
Item {
    id: root
    signal backRequested()

    // "none" | "raw" | "live"
    property string activeMode: "none"

    // Live mode state (unchanged)
    property var liveBuffer: []
    property real liveMinValue: 0
    property real liveMaxValue: 0
    property real liveMeanValue: 0
    property real liveRmsValue: 0
    readonly property real liveWindowSeconds: 10

    // Raw mode state
    property bool rawParamsLoaded: false
    property bool rawDataLoaded: false

    function stopLive() {
        Dispatcher.liveSource.stop()
        if (root.activeMode === "live")
            root.activeMode = "none"
    }

    function enterRawMode() {
        stopLive()
        root.activeMode = "raw"
    }

    // ============================================================
    // HEADER
    // ============================================================
    RowLayout {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        spacing: 12

        Button {
            text: qsTr("< Geri")
            onClicked: {
                root.stopLive()
                root.backRequested()
            }
        }
        Text {
            text: qsTr("Sinyal Görüntüleme")
            color: "#e8ebee"
            font.pixelSize: 20
            font.bold: true
        }
        Item { Layout.fillWidth: true }

        // Raw data buttons
        Button {
            text: qsTr("Parametre Dosyası Yükle")
            onClicked: {
                root.enterRawMode()
                paramFileDialog.open()
            }
        }
        Button {
            text: qsTr("Ham Veri Yükle")
            enabled: root.rawParamsLoaded
            onClicked: {
                root.enterRawMode()
                rawDataFileDialog.open()
            }
        }

        // Separator
        Rectangle { width: 1; height: 28; color: "#2c3238" }

        // Live button (unchanged)
        Button {
            text: Dispatcher.liveSource.running ? qsTr("Canlı Veriyi Durdur") : qsTr("Canlı Veri Başlat")
            onClicked: {
                if (Dispatcher.liveSource.running) {
                    root.stopLive()
                } else {
                    root.activeMode = "live"
                    root.liveBuffer = []
                    series.clear()
                    Dispatcher.liveSource.start()
                }
            }
        }
    }

    // ============================================================
    // ERROR TEXT
    // ============================================================
    Text {
        id: errorText
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        text: {
            if (root.activeMode === "raw")
                return Dispatcher.rawSignalLoader.lastError
            return ""
        }
        visible: text.length > 0
        color: "#e6483a"
        font.pixelSize: 12
        wrapMode: Text.WordWrap
    }

    // Placeholder when nothing is loaded
    Text {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.margins: 16
        text: qsTr("Henüz veri yok. \"Parametre Dosyası Yükle\" ile başlayıp ardından \"Ham Veri Yükle\" ile sinyal yükleyin, ya da \"Canlı Veri Başlat\"a basın.")
        visible: root.activeMode === "none"
        color: "#9aa4ad"
        font.pixelSize: 13
        wrapMode: Text.WordWrap
        width: parent.width - 32
    }

    // ============================================================
    // LEFT PANEL — parameters / info
    // ============================================================
    ColumnLayout {
        id: paramsColumn
        visible: root.activeMode !== "none"
        anchors.top: errorText.bottom
        anchors.left: parent.left
        anchors.bottom: measurementsRow.top
        anchors.margins: 16
        width: 280
        spacing: 4

        // --- RAW DATA MODE: parsed parameter list ---
        Text {
            visible: root.activeMode === "raw"
            text: qsTr("Ayrıştırılan Parametreler")
            color: "#e8ebee"
            font.pixelSize: 14
            font.bold: true
        }
        Text {
            visible: root.activeMode === "raw" && root.rawParamsLoaded && !root.rawDataLoaded
            text: qsTr("Parametre tanımları yüklendi (%1 adet).\nŞimdi \"Ham Veri Yükle\" ile hex dosyasını seçin.").arg(Dispatcher.rawSignalLoader.parameterCount)
            color: "#e6a23c"
            font.pixelSize: 11
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        Text {
            visible: root.activeMode === "raw" && Dispatcher.rawSignalLoader.loaded
            text: qsTr("Temizlenen veri: %1 byte").arg(Dispatcher.rawSignalLoader.rawByteCount)
            color: "#9aa4ad"
            font.pixelSize: 11
        }

        ScrollView {
            visible: root.activeMode === "raw" && Dispatcher.rawSignalLoader.loaded
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: paramListView
                model: Dispatcher.rawSignalLoader.parsedParameters
                spacing: 2
                delegate: Rectangle {
                    width: paramListView.width
                    height: paramRow.implicitHeight + 8
                    radius: 4
                    color: index % 2 === 0 ? "#1b1f24" : "#22262b"

                    ColumnLayout {
                        id: paramRow
                        anchors.fill: parent
                        anchors.margins: 4
                        spacing: 1

                        Text {
                            text: modelData.name
                            color: "#2f80e6"
                            font.pixelSize: 11
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                        RowLayout {
                            spacing: 8
                            Text {
                                text: qsTr("Ham: %1").arg(
                                    modelData.rawValue !== undefined
                                        ? modelData.rawValue.toString()
                                        : "—"
                                )
                                color: "#7a838c"
                                font.pixelSize: 10
                            }
                            Text {
                                text: qsTr("Değer: %1").arg(
                                    modelData.convertedValue !== undefined
                                        ? modelData.convertedValue.toString()
                                        : "—"
                                )
                                color: "#e8ebee"
                                font.pixelSize: 10
                                font.bold: true
                            }
                        }
                        Text {
                            text: qsTr("[%1] Adr:%2+%3 %4B %5")
                                .arg(modelData.type)
                                .arg(modelData.address)
                                .arg(modelData.offset)
                                .arg(modelData.width)
                                .arg(modelData.formula ? modelData.formula : "")
                            color: "#555b63"
                            font.pixelSize: 9
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }

        // --- LIVE MODE: generation parameters (unchanged) ---
        Text {
            visible: root.activeMode === "live"
            text: qsTr("Canlı Sinyal Parametreleri")
            color: "#e8ebee"
            font.pixelSize: 14
            font.bold: true
        }
        Text { visible: root.activeMode === "live"; text: qsTr("Frekans: %1 Hz").arg(Dispatcher.liveSource.frequencyHz.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: root.activeMode === "live"; text: qsTr("Genlik: %1").arg(Dispatcher.liveSource.amplitude.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: root.activeMode === "live"; text: qsTr("Örnekleme Hızı: %1 Hz").arg(Dispatcher.liveSource.sampleRateHz.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: root.activeMode === "live"; text: qsTr("Pencere: son %1 s").arg(root.liveWindowSeconds); color: "#9aa4ad"; font.pixelSize: 12 }

        Item { Layout.fillHeight: true }
    }

    // ============================================================
    // CHART
    // ============================================================
    ChartView {
        id: chart
        visible: root.activeMode !== "none"
        anchors.top: errorText.bottom
        anchors.left: paramsColumn.right
        anchors.right: parent.right
        anchors.bottom: measurementsRow.top
        anchors.topMargin: 16
        anchors.rightMargin: 16
        anchors.bottomMargin: 16
        anchors.leftMargin: 16
        theme: ChartView.ChartThemeDark
        backgroundColor: "#1e2226"
        antialiasing: true
        legend.visible: false

        property real origXMin: 0
        property real origXMax: 1
        property real origYMin: 0
        property real origYMax: 1

        function saveOriginalRange() {
            origXMin = axisX.min
            origXMax = axisX.max
            origYMin = axisY.min
            origYMax = axisY.max
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: {
                axisX.min = chart.origXMin
                axisX.max = chart.origXMax
                axisY.min = chart.origYMin
                axisY.max = chart.origYMax
            }
            onWheel: {
                var factor = wheel.angleDelta.y > 0 ? 0.8 : 1.25
                var pos = chart.mapToValue(Qt.point(wheel.x, wheel.y), series)

                var xMin = axisX.min
                var xMax = axisX.max
                var yMin = axisY.min
                var yMax = axisY.max

                var newXMin = pos.x - (pos.x - xMin) * factor
                var newXMax = pos.x + (xMax - pos.x) * factor
                var newYMin = pos.y - (pos.y - yMin) * factor
                var newYMax = pos.y + (yMax - pos.y) * factor

                axisX.min = newXMin
                axisX.max = newXMax
                axisY.min = newYMin
                axisY.max = newYMax
            }
        }

        ValueAxis {
            id: axisX
            titleText: root.activeMode === "raw" ? qsTr("Byte Konumu") : qsTr("Zaman (s)")
        }
        ValueAxis {
            id: axisY
            titleText: root.activeMode === "raw" ? qsTr("Byte Değeri") : qsTr("Genlik")
        }

        LineSeries {
            id: series
            axisX: axisX
            axisY: axisY
            color: "#2f80e6"
            onHovered: function(point, state) {
                if (state) {
                    tooltip.xValue = point.x
                    tooltip.yValue = point.y
                    var pos = chart.mapToPosition(point, series)
                    tooltip.x = pos.x + 12
                    tooltip.y = pos.y - tooltip.height - 8
                    tooltip.visible = true
                } else {
                    tooltip.visible = false
                }
            }
        }

        Rectangle {
            id: tooltip
            visible: false
            z: 10
            width: tooltipText.implicitWidth + 16
            height: tooltipText.implicitHeight + 10
            radius: 6
            color: "#2c3238"
            border.width: 1
            border.color: "#3e454d"

            property real xValue: 0
            property real yValue: 0

            Text {
                id: tooltipText
                anchors.centerIn: parent
                color: "#e8ebee"
                font.pixelSize: 11
                font.bold: true
                text: {
                    if (root.activeMode === "raw")
                        return qsTr("Byte %1: %2").arg(Math.round(tooltip.xValue)).arg(Math.round(tooltip.yValue))
                    return qsTr("t: %1 s  |  %2").arg(tooltip.xValue.toFixed(3)).arg(tooltip.yValue.toFixed(3))
                }
            }
        }
    }

    // ============================================================
    // BOTTOM: measurements / stats
    // ============================================================
    RowLayout {
        id: measurementsRow
        visible: (root.activeMode === "raw" && Dispatcher.rawSignalLoader.loaded) || root.activeMode === "live"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        spacing: 24

        // Raw mode: stats computed from cleaned byte values
        Text {
            visible: root.activeMode === "raw"
            text: qsTr("Parametre Sayısı: %1").arg(Dispatcher.rawSignalLoader.parameterCount)
            color: "#e8ebee"; font.pixelSize: 13
        }
        Text {
            visible: root.activeMode === "raw"
            text: qsTr("Toplam Byte: %1").arg(Dispatcher.rawSignalLoader.rawByteCount)
            color: "#e8ebee"; font.pixelSize: 13
        }

        // Live mode: rolling stats (unchanged)
        Text { visible: root.activeMode === "live"; text: qsTr("Min: %1").arg(root.liveMinValue.toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { visible: root.activeMode === "live"; text: qsTr("Maks: %1").arg(root.liveMaxValue.toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { visible: root.activeMode === "live"; text: qsTr("Ortalama: %1").arg(root.liveMeanValue.toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { visible: root.activeMode === "live"; text: qsTr("RMS: %1").arg(root.liveRmsValue.toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { visible: root.activeMode === "live"; text: qsTr("Tepe-Tepe: %1").arg((root.liveMaxValue - root.liveMinValue).toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Item { Layout.fillWidth: true }
    }

    // ============================================================
    // FILE DIALOGS
    // ============================================================
    FileDialog {
        id: paramFileDialog
        title: qsTr("Parametre Tanım Dosyası Seç (CSV/Excel)")
        nameFilters: [qsTr("Desteklenen Dosyalar (*.csv *.xlsx)"), qsTr("CSV Dosyaları (*.csv)"), qsTr("Excel Dosyaları (*.xlsx)"), qsTr("Tüm Dosyalar (*)")]
        onAccepted: {
            if (Dispatcher.rawSignalLoader.loadParameterFile(file)) {
                root.rawParamsLoaded = true
            }
        }
    }

    FileDialog {
        id: rawDataFileDialog
        title: qsTr("Ham Hex Veri Dosyası Seç")
        nameFilters: [qsTr("Metin Dosyaları (*.txt)"), qsTr("Tüm Dosyalar (*)")]
        onAccepted: {
            if (Dispatcher.rawSignalLoader.loadRawDataFile(file)) {
                root.rawDataLoaded = true
            }
        }
    }

    // ============================================================
    // CONNECTIONS — Raw data plot
    // ============================================================
    Connections {
        target: Dispatcher.rawSignalLoader
        function onDataChanged() {
            if (root.activeMode !== "raw")
                return
            if (!Dispatcher.rawSignalLoader.loaded)
                return

            // Plot the cleaned raw bytes as a waveform
            series.clear()
            const bytes = Dispatcher.rawSignalLoader.rawByteValues
            let minV = 255, maxV = 0
            for (let i = 0; i < bytes.length; i++) {
                series.append(i, bytes[i])
                if (bytes[i] < minV) minV = bytes[i]
                if (bytes[i] > maxV) maxV = bytes[i]
            }
            axisX.min = 0
            axisX.max = bytes.length - 1
            axisY.min = minV - 5
            axisY.max = maxV + 5
            chart.saveOriginalRange()
        }
    }

    // ============================================================
    // CONNECTIONS — Live rolling plot (unchanged)
    // ============================================================
    Connections {
        target: Dispatcher.liveSource
        function onSampleGenerated(time, value) {
            if (root.activeMode !== "live")
                return

            root.liveBuffer.push({ t: time, v: value })
            while (root.liveBuffer.length > 0 && time - root.liveBuffer[0].t > root.liveWindowSeconds)
                root.liveBuffer.shift()

            series.clear()
            let sum = 0, sumSq = 0
            let minV = root.liveBuffer[0].v, maxV = root.liveBuffer[0].v
            for (let i = 0; i < root.liveBuffer.length; i++) {
                const p = root.liveBuffer[i]
                series.append(p.t, p.v)
                sum += p.v
                sumSq += p.v * p.v
                if (p.v < minV) minV = p.v
                if (p.v > maxV) maxV = p.v
            }
            const n = root.liveBuffer.length
            root.liveMinValue = minV
            root.liveMaxValue = maxV
            root.liveMeanValue = n > 0 ? sum / n : 0
            root.liveRmsValue = n > 0 ? Math.sqrt(sumSq / n) : 0

            axisX.min = root.liveBuffer[0].t
            axisX.max = time
            axisY.min = minV - 0.1
            axisY.max = maxV + 0.1
        }
    }
}
