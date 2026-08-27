import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: headerBar
    implicitHeight: 30
    property var controller: null

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 12

        Label {
            text: "WEBCRAWLER v0.0.1"
            color: "green"
            font.bold: true
        }

        Item {
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 6

            Label {
                text: qsTr("DISCOVERED")
                color: "grey"
            }
            Text {
                text: headerBar.controller ? headerBar.controller.discovered : "0"
                color: "black"
                font.weight: Font.Medium

                Layout.preferredWidth: 40
                horizontalAlignment: Text.AlignLeft
            }
        }

        Label {
            text: qsTr("ELAPSED")
            color: "grey"
        }
    }
}
