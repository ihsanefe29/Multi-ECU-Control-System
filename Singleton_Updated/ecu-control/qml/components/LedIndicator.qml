import QtQuick 2.15

// Simple status LED. state drives the color:
//   "inactive" -> gray, "success" -> green, "fail" -> red,
//   "pending"  -> blue (waiting/in-progress)
//
// This is the generic single-boolean-friendly LED discussed in
// requirements: if a caller only has a plain bool, they should map
// it to "success"/"fail" themselves (true isn't assumed to mean
// success by default - that mapping is caller-specific).
Rectangle {
    id: root
    property string state: "inactive" // inactive | pending | success | fail
    property string pendingColor: "#2f80e6" // swap to orange (#e6a23c) if preferred per-instance

    width: 16
    height: 16
    radius: width / 2
    border.width: 1
    border.color: "#00000055"

    color: {
        switch (state) {
        case "success": return "#2fbf71"
        case "fail":    return "#e6483a"
        case "pending": return pendingColor
        default:        return "#5a626b" // inactive
        }
    }

    Behavior on color { ColorAnimation { duration: 150 } }
}
