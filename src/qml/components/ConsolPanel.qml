import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Crawler 1.0

Item {
    id: root
    property bool expanded: true
    property int panelHeight: 200
    property int minHeight: 80
    property int maxHeight: 500

    height: expanded ? (header.height + panelHeight) : header.height
    Layout.minimumHeight: header.height
    Layout.preferredHeight: height

    // Drag handle for resizing when expanded
    MouseArea {
        id: dragArea
        visible: root.expanded
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        height: 8
        y: header.height - 4
        cursorShape: Qt.SizeVerCursor

        property int previousY: 0

        onPressed: (mouse) => {
            previousY = mouse.screenY
        }
        onPositionChanged: (mouse) => {
            let dy = previousY - mouse.screenY
            let target = root.panelHeight + dy
            if (target >= root.minHeight && target <= root.maxHeight) {
                root.panelHeight = target
            }
            previousY = mouse.screenY
        }
    }

    // Header Bar
    Rectangle {
        id: header
        width: parent.width
        height: 30
        color: "#e0e0e0"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10

            Text {
                text: root.expanded ? "CONSOLE ▼" : "CONSOLE ▲"
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                text: "CLEAR"
                visible: root.expanded
                font.pixelSize: 10
                implicitHeight: 22
                onClicked: {
                    LogModel.clear()
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            z: -1
            onClicked: {
                root.expanded = !root.expanded
            }
        }
    }

    // Content / ListView
    Rectangle {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        width: parent.width
        visible: root.expanded
        color: "#f9f9f9"
        clip: true

        ListView {
            id: listView
            anchors.fill: parent
            anchors.margins: 4
            model: LogModel
            delegate: Row {
                spacing: 8
                Text {
                    text: Qt.formatTime(time, "hh:mm:ss")
                    color: "#666"
                }
                Text {
                    text: {
                        switch(type) {
                            case 0: return "DBG";
                            case 1: return "WARN";
                            case 2: return "ERR";
                            case 3: return "INFO";
                            case 4: return "SYS";
                            default: return "INFO";
                        }
                    }
                    font.bold: true
                    color: {
                        // SYS (green), INFO (blue), DBG (muted), WARN (amber), ERR (red)
                        switch(type) {
                            case 0: return "gray";       // DBG
                            case 1: return "darkorange"; // WARN (amber)
                            case 2: return "red";        // ERR
                            case 3: return "blue";       // INFO
                            case 4: return "green";      // SYS
                            default: return "black";
                        }
                    }
                }
                Text {
                    text: message
                    color: "black"
                }
            }
        }
    }

    // 4px visual handle line
    Rectangle {
        visible: root.expanded
        anchors.top: header.bottom
        width: parent.width
        height: 4
        color: "#cccccc"
    }
}
