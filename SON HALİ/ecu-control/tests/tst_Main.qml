import QtQuick 2.15
import QtTest 1.3
import TestHelpers 1.0

TestCase {
    name: "MainQmlTest"

    function createDispatcher() {
        return Qt.createQmlObject('import QtQuick 2.15; QtObject { property var ecuAModule1; property var ecuAModule2; property var ecuBModule1; property var ecuBModule2 }', this);
    }

    function createMain() {
        var root = Qt.createQmlObject('import QtQuick 2.15; Item { property var dispatcher }', this);
        root.dispatcher = createDispatcher();
        var component = Qt.createComponent("qml/Main.qml");
        var main = component.createObject(root, { dispatcher: root.dispatcher });
        return { root: root, main: main };
    }

    function test_hasTwoScreens() {
        var env = createMain();
        var controlScreen = TestHelpers.findChild(env.main, "visible", true);
        verify(controlScreen !== null);
        env.main.destroy();
        env.root.destroy();
    }
}
