import QtQuick 2.15
import QtTest 1.3

TestCase {
    name: "SignalChartScreenTest"

    function createMockSignalLoader() {
        return Qt.createQmlObject('import QtQuick 2.15; QtObject { property bool loaded: false; property string lastError: ""; property var timeValues: []; property var amplitudeValues: []; property real minValue: 0; property real maxValue: 0; property real meanValue: 0; property real rmsValue: 0; property real peakToPeakValue: 0; property string fileName: ""; property int sampleCount: 0; property real sampleRateHz: 0; property real durationSeconds: 0; function loadFile(file) {} }', this);
    }

    function createMockLiveSource() {
        return Qt.createQmlObject('import QtQuick 2.15; QtObject { property bool running: false; property real frequencyHz: 1; property real amplitude: 1; property real sampleRateHz: 10; function start() { running = true; } function stop() { running = false; } }', this);
    }

    function createScreen() {
        var root = Qt.createQmlObject('import QtQuick 2.15; Item { property var signalLoader; property var liveSource }', this);
        root.signalLoader = createMockSignalLoader();
        root.liveSource = createMockLiveSource();
        var component = Qt.createComponent("qml/SignalChartScreen.qml");
        var screen = component.createObject(root, {});
        return { root: root, screen: screen, signalLoader: root.signalLoader, liveSource: root.liveSource };
    }

    function test_defaultLiveMode() {
        var env = createScreen();
        compare(env.screen.liveMode, false);
        env.screen.destroy();
        env.root.destroy();
    }

    function test_stopLiveSetsLiveModeFalse() {
        var env = createScreen();
        env.screen.liveMode = true;
        env.signalLoader.loaded = true;
        env.liveSource.running = true;
        env.screen.stopLive();
        compare(env.screen.liveMode, false);
        compare(env.liveSource.running, false);
        env.screen.destroy();
        env.root.destroy();
    }

    function test_backRequestedSignalExists() {
        var env = createScreen();
        var received = false;
        env.screen.backRequested.connect(function() { received = true; });
        env.screen.backRequested();
        verify(received);
        env.screen.destroy();
        env.root.destroy();
    }
}
