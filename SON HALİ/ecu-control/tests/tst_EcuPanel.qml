import QtQuick 2.15
import QtQuick.Controls 2.15
import QtTest 1.3
import TestHelpers 1.0

TestCase {
    name: "EcuPanelTest"

    function createDispatcher() {
        return Qt.createQmlObject('import QtQuick 2.15; QtObject { signal disconnectModule(string,int); signal connectModule(string,int); signal setMode(string,int,string); signal setPower(string,int,string) }', this);
    }

    function createSession() {
        return Qt.createQmlObject('import QtQuick 2.15; QtObject { property string ecuId: "A"; property int moduleNumber: 1; property bool connected: false; property string mode: "DataView"; property string powerState: "Off" }', this);
    }

    function createPanel() {
        var root = Qt.createQmlObject('import QtQuick 2.15; Item { property var dispatcher; property var session }', this);
        root.dispatcher = createDispatcher();
        root.session = createSession();
        var component = Qt.createComponent("qml/EcuPanel.qml");
        var panel = component.createObject(root, { session: root.session });
        return { root: root, panel: panel, dispatcher: root.dispatcher };
    }

    function test_displayTitleUsesSession() {
        var env = createPanel();
        compare(env.panel.displayTitle, "ECU A — Modül 1");
        env.panel.destroy();
        env.root.destroy();
    }

    function test_dataViewSelectedSignalEmitted() {
        var env = createPanel();
        var fired = false;
        env.panel.dataViewSelected.connect(function() { fired = true; });
        env.panel.dataViewSelected();
        verify(fired);
        env.panel.destroy();
        env.root.destroy();
    }

    function test_modeSelectionCallsDispatcher() {
        var env = createPanel();
        var modeCall = false;
        env.dispatcher.setMode.connect(function(ecuId, moduleNumber, mode) {
            if (ecuId === "A" && moduleNumber === 1 && mode === "FirmwareUpload")
                modeCall = true;
        });

        var comboBox = TestHelpers.findComboBox(env.panel);
        if (comboBox)
            comboBox.currentIndex = 1;

        verify(modeCall);
        env.panel.destroy();
        env.root.destroy();
    }
}
