import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    clip: true

    property var controller: null

    onControllerChanged: {
        if (controller) {
            tableView.model = controller.model
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView {
            id: horizontalHeader
            syncView: tableView
            Layout.fillWidth: true
            implicitHeight: 35
            delegate: Rectangle {
                color: "#333333"
                border.color: "#444444"
                implicitHeight: 35

                Text {
                    anchors.centerIn: parent
                    text: {
                        switch(index) {
                            case 0: return qsTr("Url")
                            case 1: return qsTr("Time")
                            case 2: return qsTr("Status")
                            case 3: return qsTr("Depth")
                            default: return ""
                        }
                    }
                    color: "white"
                    font.bold: true
                }
            }
        }

        TableView {
            id: tableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            columnWidthProvider: function(column) {
                var totalWidth = tableView.width;
                switch(column) {
                    case 0: return totalWidth * 0.55;
                    case 1: return totalWidth * 0.15;
                    case 2: return totalWidth * 0.15;
                    case 3: return totalWidth * 0.15;
                    default: return 100;
                }
            }

            rowHeightProvider: function(row) {
                return 40;
            }

            delegate: Rectangle {
                color: row % 2 === 0 ? "#444444" : "#555555"
                border.color: "#333333"

                Text {
                    anchors.centerIn: parent
                    width: parent.width - 10
                    color: "white"
                    elide: Text.ElideRight
                    horizontalAlignment: column === 0 ? Text.AlignLeft : Text.AlignHCenter

                    text: {
                        if (url === undefined) return ""
                        switch(column) {
                            case 0: return url
                            case 1: return Qt.formatTime(time, "hh:mm:ss")
                            case 2: return status
                            case 3: return depth
                            default: return ""
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                active: true
            }

            ScrollBar.horizontal: ScrollBar {
                active: true
            }
        }
    }
}
