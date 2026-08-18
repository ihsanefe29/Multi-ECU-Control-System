import QtQuick 2.15
import QtTest 1.3

TestCase {
    name: "PowerSwitchItemTest"

    function createSwitch(properties) {
        var component = Qt.createComponent("qml/components/PowerSwitchItem.qml");
        return component.createObject(this, properties || {});
    }

    function test_initialStateOff() {
        var item = createSwitch();
        compare(item.state, "Off");
        item.destroy();
    }

    function test_requestStateSignal() {
        var item = createSwitch();
        var received = "";
        item.requestState.connect(function(newState) {
            received = newState;
        });
        item.requestState("Kill");
        compare(received, "Kill");
        item.destroy();
    }
}