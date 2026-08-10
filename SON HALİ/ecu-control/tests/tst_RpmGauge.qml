import QtQuick 2.15
import QtTest 1.3

TestCase {
    name: "RpmGaugeTest"

    function createGauge(properties) {
        var component = Qt.createComponent("qml/components/RpmGauge.qml");
        return component.createObject(this, properties || {});
    }

    function test_defaultText() {
        var gauge = createGauge();
        compare(gauge.value, 0);
        compare(gauge.children[0].text, "0 RPM");
        gauge.destroy();
    }

    function test_valueUpdatesText() {
        var gauge = createGauge({ value: 1234 });
        compare(gauge.children[0].text, "1234 RPM");
        gauge.destroy();
    }
}
