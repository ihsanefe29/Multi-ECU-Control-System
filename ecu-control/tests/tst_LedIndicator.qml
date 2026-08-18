import QtQuick 2.15
import QtTest 1.3

TestCase {
    name: "LedIndicatorTest"

    function createIndicator(properties) {
        var component = Qt.createComponent("qml/components/LedIndicator.qml");
        return component.createObject(this, properties || {});
    }

    function test_defaultColor() {
        var indicator = createIndicator();
        compare(indicator.state, "inactive");
        compare(indicator.color, "#5a626b");
        indicator.destroy();
    }

    function test_stateColorMapping() {
        var indicator = createIndicator({ state: "success" });
        compare(indicator.color, "#2fbf71");
        indicator.state = "fail";
        compare(indicator.color, "#e6483a");
        indicator.state = "pending";
        compare(indicator.color, "#2f80e6");
        indicator.destroy();
    }

    function test_pendingColorOverride() {
        var indicator = createIndicator({ state: "pending", pendingColor: "#ff8800" });
        compare(indicator.color, "#ff8800");
        indicator.destroy();
    }
}