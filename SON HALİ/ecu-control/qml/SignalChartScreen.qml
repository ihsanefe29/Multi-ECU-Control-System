import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import Qt.labs.platform 1.1

// Standalone screen shown whenever any module's mode is switched to
// "Veri Görüntüleme" (Data View). Not tied to a specific module.
//
// Two independent data sources feed the same chart:
//   - signalLoader: loads a complete file at once, static plot
//   - liveSource: mock "live" data arriving sample-by-sample,
//     plotted as a scrolling/rolling window (like an oscilloscope)
// Only one is "active" at a time - starting one stops the other.
Item {
    id: root
    signal backRequested()

    property bool liveMode: false
    property var liveBuffer: []   // rolling window of {t, v} while live
    property real liveMinValue: 0
    property real liveMaxValue: 0
    property real liveMeanValue: 0
    property real liveRmsValue: 0
    readonly property real liveWindowSeconds: 10

    function stopLive() {
        liveSource.stop()
        root.liveMode = false
    }

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
            text: qsTr("Sinyal Görüntüleme")
            color: "#e8ebee"
            font.pixelSize: 20
            font.bold: true
        }
        Item { Layout.fillWidth: true }
        Button {
            text: qsTr("Dosya Seç")
            onClicked: {
                root.stopLive()
                signalFileDialog.open()
            }
        }
        Button {
            text: liveSource.running ? qsTr("Canlı Veriyi Durdur") : qsTr("Canlı Veri Başlat")
            onClicked: {
                if (liveSource.running) {
                    root.stopLive()
                } else {
                    root.liveMode = true
                    root.liveBuffer = []
                    series.clear()
                    liveSource.start()
                }
            }
        }
    }

    Text {
        id: errorText
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        text: signalLoader.lastError
        visible: signalLoader.lastError.length > 0
        color: "#e6483a"
        font.pixelSize: 12
        wrapMode: Text.WordWrap
    }

    Text {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.margins: 16
        text: qsTr("Henüz veri yok. \"Dosya Seç\" ile bir sinyal dosyası aç ya da \"Canlı Veri Başlat\"a bas.")
        visible: !signalLoader.loaded && !root.liveMode
        color: "#9aa4ad"
        font.pixelSize: 13
    }

    // Parameters column - fixed width on the left. Shows file
    // parameters or live-generation parameters depending on mode.
    ColumnLayout {
        id: paramsColumn
        visible: signalLoader.loaded || root.liveMode
        anchors.top: errorText.bottom
        anchors.left: parent.left
        anchors.bottom: measurementsRow.top
        anchors.margins: 16
        width: 220
        spacing: 8

        Text { text: qsTr("Parametreler"); color: "#e8ebee"; font.pixelSize: 14; font.bold: true }

        // File mode parameters
        Text { visible: !root.liveMode; text: qsTr("Dosya: %1").arg(signalLoader.fileName); color: "#9aa4ad"; font.pixelSize: 12; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Text { visible: !root.liveMode; text: qsTr("Örnek Sayısı: %1").arg(signalLoader.sampleCount); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: !root.liveMode; text: qsTr("Örnekleme Hızı: %1 Hz").arg(signalLoader.sampleRateHz.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: !root.liveMode; text: qsTr("Süre: %1 s").arg(signalLoader.durationSeconds.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }

        // Live mode parameters - these are the signal's actual
        // generating parameters, unlike the file case where they're
        // just descriptive.
        Text { visible: root.liveMode; text: qsTr("Frekans: %1 Hz").arg(liveSource.frequencyHz.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: root.liveMode; text: qsTr("Genlik: %1").arg(liveSource.amplitude.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: root.liveMode; text: qsTr("Örnekleme Hızı: %1 Hz").arg(liveSource.sampleRateHz.toFixed(2)); color: "#9aa4ad"; font.pixelSize: 12 }
        Text { visible: root.liveMode; text: qsTr("Pencere: son %1 s").arg(root.liveWindowSeconds); color: "#9aa4ad"; font.pixelSize: 12 }

        Item { Layout.fillHeight: true }
    }

    // Chart fills every remaining pixel to the right of the
    // parameters column, all the way to the window edge.
    ChartView {
        id: chart
        visible: signalLoader.loaded || root.liveMode
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

        ValueAxis {
            id: axisX
            titleText: qsTr("Zaman (s)")
        }
        ValueAxis {
            id: axisY
            titleText: qsTr("Genlik")
        }

        LineSeries {
            id: series
            axisX: axisX
            axisY: axisY
            color: "#2f80e6"
        }
    }

    // Bottom: measurements - from the loaded file, or running stats
    // over the current live window.
    RowLayout {
        id: measurementsRow
        visible: signalLoader.loaded || root.liveMode
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        spacing: 24

        Text { text: qsTr("Min: %1").arg((root.liveMode ? root.liveMinValue : signalLoader.minValue).toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { text: qsTr("Maks: %1").arg((root.liveMode ? root.liveMaxValue : signalLoader.maxValue).toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { text: qsTr("Ortalama: %1").arg((root.liveMode ? root.liveMeanValue : signalLoader.meanValue).toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { text: qsTr("RMS: %1").arg((root.liveMode ? root.liveRmsValue : signalLoader.rmsValue).toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Text { text: qsTr("Tepe-Tepe: %1").arg((root.liveMode ? (root.liveMaxValue - root.liveMinValue) : signalLoader.peakToPeakValue).toFixed(3)); color: "#e8ebee"; font.pixelSize: 13 }
        Item { Layout.fillWidth: true }
    }

    FileDialog {
        id: signalFileDialog
        title: qsTr("Sinyal Dosyası Seç")
        nameFilters: [qsTr("Metin Dosyaları (*.txt)"), qsTr("Tüm Dosyalar (*)")]
        onAccepted: signalLoader.loadFile(file)
    }

    // Static file plot - full rebuild each time a new file loads.
    Connections {
        target: signalLoader
        function onDataChanged() {
            if (root.liveMode)
                return
            series.clear()
            const times = signalLoader.timeValues
            const amps = signalLoader.amplitudeValues
            for (let i = 0; i < times.length; i++)
                series.append(times[i], amps[i])

            if (times.length > 0) {
                axisX.min = times[0]
                axisX.max = times[times.length - 1]
            }
            axisY.min = signalLoader.minValue
            axisY.max = signalLoader.maxValue
        }
    }

    // Live rolling plot - one new sample per tick, old samples
    // outside the window drop off, chart and stats rebuild from the
    // small in-memory window each tick.
    Connections {
        target: liveSource
        function onSampleGenerated(time, value) {
            if (!root.liveMode)
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
