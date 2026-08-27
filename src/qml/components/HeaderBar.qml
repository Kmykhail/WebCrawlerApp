import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: headerBar
    implicitHeight: 30

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 12

        Text {
            text: "WEBCRAWLER v0.0.1"
            color: "green"
            font.bold: true
        }

        Item {
            Layout.fillWidth: true
        }

        Text {
            text: qsTr("DISCOVERED")
        }

        Text {
            text: qsTr("ELAPSED")
        }
    }
}
