import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "components"

ApplicationWindow {

    id: window

    property int currentPage: 0

    ListModel {
        id: logModel
    }

    visible: true

    width: 1400
    height: 900

    minimumWidth: 1200
    minimumHeight: 700

    title: "ECU Control Panel"

    Rectangle {

        anchors.fill: parent

        color: "#1B1F24"

        ColumnLayout {

            anchors.fill: parent

            anchors.margins: 15

            spacing: 10


            // =====================================================
            // ECU CONTROL PAGE
            // =====================================================

            Item {

                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: window.currentPage === 0

                ColumnLayout {

                    anchors.fill: parent

                    spacing: 10


                    // HEADER

                    Header {

                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                    }

                    // ECU PANELS + SYSTEM LOG

                    RowLayout {

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        spacing: 15


                        // ECU PANELS

                        GridLayout {

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            columns: 2

                            rowSpacing: 8
                            columnSpacing: 12


                            // ECU A-1

                            ECUPanel {

                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                title: "ECU A-1"
                                ecuId: 0

                                onLogRequested: {

                                    var t =
                                        Qt.formatTime(
                                            new Date(),
                                            "HH:mm:ss"
                                        )

                                    logModel.append({
                                        "time": t,
                                        "level": level,
                                        "message": message
                                    })
                                }
                            }


                            // ECU A-2

                            ECUPanel {

                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                title: "ECU A-2"
                                ecuId: 1

                                onLogRequested: {

                                    var t =
                                        Qt.formatTime(
                                            new Date(),
                                            "HH:mm:ss"
                                        )

                                    logModel.append({
                                        "time": t,
                                        "level": level,
                                        "message": message
                                    })
                                }
                            }


                            // ECU B-1

                            ECUPanel {

                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                title: "ECU B-1"
                                ecuId: 2

                                onLogRequested: {

                                    var t =
                                        Qt.formatTime(
                                            new Date(),
                                            "HH:mm:ss"
                                        )

                                    logModel.append({
                                        "time": t,
                                        "level": level,
                                        "message": message
                                    })
                                }
                            }


                            // ECU B-2

                            ECUPanel {

                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                title: "ECU B-2"
                                ecuId: 3

                                onLogRequested: {

                                    var t =
                                        Qt.formatTime(
                                            new Date(),
                                            "HH:mm:ss"
                                        )

                                    logModel.append({
                                        "time": t,
                                        "level": level,
                                        "message": message
                                    })
                                }
                            }
                        }


                        // SYSTEM LOG

                        SystemLog {

                            Layout.preferredWidth: 280
                            Layout.maximumWidth: 280
                            Layout.fillHeight: true

                            model: logModel
                        }
                    }
                }
            }


            // =====================================================
            // SOFTWARE UPLOAD PAGE
            // =====================================================

            SoftwareUpload {

                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: window.currentPage === 1
            }

            // =====================================================
            // DATA VISUALIZATION PAGE
            // =====================================================

            DataVisualization {

                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: window.currentPage === 2
            }

            // =====================================================
            // BOTTOM TABS
            // =====================================================

            TabBar {

                id: tabBar

                Layout.fillWidth: true

                TabButton {
                    text: "ECU Kontrol"
                }

                TabButton {
                    text: "Yazılım Yükleme"
                }

                TabButton {
                    text: "Veri Görüntüleme"
                }

                onCurrentIndexChanged: {

                    window.currentPage = currentIndex
                }
            }
        }
    }
}