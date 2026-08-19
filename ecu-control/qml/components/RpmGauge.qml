// RpmGauge.qml — Seçilebilir metrik göstergesi (N1, EGT, Fuel Flow, Vibration)
// Her pencere boyutunda çalışır; tüm ölçüler parent'a göre oransal.
import EcuControl 1.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    // Karta özgü aksent rengi (A-1: mavi, A-2: mor, B-1: teal, B-2: turuncu)
    property string accentColor: "#3b82f6"

    // Seçili metrik indeksi
    property int metricIndex: 0

    // Metrik tanımları — renk, aralık, birim
    readonly property var metrics: [
        { name: "N1",         label: "N1",         unit: "%",    min: 0,   max: 105,  color: root.accentColor },
        { name: "EGT",        label: "EGT",         unit: "°C",   min: 300, max: 950,  color: "#ef4444"        },
        { name: "Fuel Flow",  label: "Yakıt",       unit: "kg/h", min: 200, max: 2500, color: "#f97316"        },
        { name: "Vibration",  label: "Titreşim",    unit: "",     min: 0.0, max: 5.0,  color: "#a855f7"        }
    ]

    property var  currentMetric: metrics[metricIndex]

    // Değeri ParameterModel'den oku (Parametre ekranından veya DataSimulator'dan güncellenir)
    property real currentValue: {
        var v = Dispatcher.parameterModel.getValue(currentMetric.name)
        if (v === undefined || v === null || isNaN(Number(v))) return currentMetric.min
        return Number(v)
    }

    // ─────────────────────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        spacing: 4

        // ── Metrik seçici ─────────────────────────────────────────────────────
        ComboBox {
            id: metricCombo
            Layout.fillWidth: true
            implicitHeight: 26
            model: root.metrics.map(function(m) { return m.label + "  " + m.unit })
            currentIndex: root.metricIndex
            onCurrentIndexChanged: root.metricIndex = currentIndex

            background: Rectangle {
                color: "#0f1a2e"; radius: 5
                border.color: root.currentMetric.color; border.width: 1
            }
            contentItem: Text {
                leftPadding: 8
                text: metricCombo.displayText
                color: root.currentMetric.color
                font.pixelSize: 11; font.bold: true
                verticalAlignment: Text.AlignVCenter
            }
            // Basit ok ikonu
            indicator: Text {
                text: "▾"; color: "#64748b"
                font.pixelSize: 14
                anchors { right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
            }
        }

        // ── Arc Gauge ─────────────────────────────────────────────────────────
        Canvas {
            id: gaugeCanvas
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Canvas yeniden çizimi tetikleyelim
            property real  _val:    root.currentValue
            property color _color:  root.currentMetric.color
            property real  _min:    root.currentMetric.min
            property real  _max:    root.currentMetric.max
            property string _unit:  root.currentMetric.unit
            property string _label: root.currentMetric.label

            on_ValChanged:    requestPaint()
            on_ColorChanged:  requestPaint()
            on_MinChanged:    requestPaint()
            on_MaxChanged:    requestPaint()
            onWidthChanged:   requestPaint()
            onHeightChanged:  requestPaint()

            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()

                // Merkez ve yarıçap — yüksekliğin %60'ında merkez, oransal r
                var cx = width  / 2
                var cy = height * 0.60
                var r  = Math.min(width * 0.42, height * 0.58)
                if (r < 10) return

                // Ark açıları: 135° → 405° (270° süpürme, alt-sol'dan alt-sağ'a)
                var startA = Math.PI * 0.75
                var endA   = Math.PI * 2.25
                var sweep  = endA - startA

                // Normalize değer [0,1]
                var norm = (_max > _min) ? (_val - _min) / (_max - _min) : 0
                norm = Math.max(0, Math.min(1, norm))
                var valA = startA + norm * sweep

                var trackW = r * 0.15

                // 1. Arka plan arkı (koyu)
                ctx.beginPath()
                ctx.arc(cx, cy, r, startA, endA)
                ctx.strokeStyle = "#1e293b"
                ctx.lineWidth   = trackW
                ctx.lineCap     = "round"
                ctx.stroke()

                // 2. Değer arkı (renkli)
                if (norm > 0.005) {
                    ctx.beginPath()
                    ctx.arc(cx, cy, r, startA, valA)
                    ctx.strokeStyle = _color
                    ctx.lineWidth   = trackW
                    ctx.lineCap     = "round"
                    ctx.stroke()

                    // Hafif dış parıltı
                    ctx.beginPath()
                    ctx.arc(cx, cy, r, startA, valA)
                    ctx.strokeStyle = _color
                    ctx.lineWidth   = trackW * 2.2
                    ctx.globalAlpha = 0.10
                    ctx.lineCap     = "round"
                    ctx.stroke()
                    ctx.globalAlpha = 1.0
                }

                // 3. Uç nokta parlak daire
                if (norm > 0.005) {
                    var tipX = cx + r * Math.cos(valA)
                    var tipY = cy + r * Math.sin(valA)
                    ctx.beginPath()
                    ctx.arc(tipX, tipY, trackW * 0.55, 0, Math.PI * 2)
                    ctx.fillStyle = _color
                    ctx.fill()
                }

                // 4. Merkez arka daire
                ctx.beginPath()
                ctx.arc(cx, cy, r * 0.30, 0, Math.PI * 2)
                ctx.fillStyle = "#0d1829"
                ctx.fill()

                // 5. Ana değer yazısı
                var fs = Math.max(11, r * 0.36)
                ctx.fillStyle   = "#e2e8f0"
                ctx.font        = "bold " + fs.toFixed(0) + "px sans-serif"
                ctx.textAlign   = "center"
                ctx.textBaseline = "middle"
                var disp = ((_val % 1) < 0.05) ? _val.toFixed(0) : _val.toFixed(1)
                ctx.fillText(disp, cx, cy - r * 0.02)

                // 6. Birim yazısı
                ctx.fillStyle = "#64748b"
                ctx.font      = Math.max(8, r * 0.19).toFixed(0) + "px sans-serif"
                ctx.fillText(_unit, cx, cy + r * 0.32)

                // 7. Min / Max etiketleri
                ctx.fillStyle = "#334155"
                ctx.font      = Math.max(7, r * 0.16).toFixed(0) + "px sans-serif"
                var minLX = cx + (r + trackW) * Math.cos(startA) * 1.12
                var minLY = cy + (r + trackW) * Math.sin(startA) * 1.12
                var maxLX = cx + (r + trackW) * Math.cos(endA)   * 1.12
                var maxLY = cy + (r + trackW) * Math.sin(endA)   * 1.12
                ctx.textAlign = "right";  ctx.fillText(_min.toFixed(0), minLX, minLY)
                ctx.textAlign = "left";   ctx.fillText(_max.toFixed(0), maxLX, maxLY)
            }
        }
    }
}
