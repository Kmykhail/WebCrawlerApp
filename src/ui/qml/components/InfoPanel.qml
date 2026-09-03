import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WebCrawlerApp 1.0

Item {
    id: infoPanel
    implicitHeight: 46
    property CrawlerController controller: null

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 12

        ColumnLayout {
            spacing: 2

            Label {
                text: qsTr("DISCOVERED")
                color: "grey"
            }
            Text {
                text: controller ? controller.state.discovered : "0"
                color: "black"
                font.weight: Font.Medium

                Layout.preferredWidth: 40
                horizontalAlignment: Text.AlignLeft
            }
        }

        ColumnLayout {
            spacing: 2

            Label {
                text: qsTr("Queued")
                color: "grey"
            }

            Text {
                text: controller ? controller.state.queued : "0"
                color: "black"
                font.weight: Font.Medium

                Layout.preferredWidth: 40
                horizontalAlignment: Text.AlignLeft
            }
        }


        ColumnLayout {
            spacing: 2

            Label {
                text: qsTr("FETCHED")
                color: "grey"
            }

            Text {
                text: controller ? controller.state.fetched : "0"
                color: "black"
                font.weight: Font.Medium

                Layout.preferredWidth: 40
                horizontalAlignment: Text.AlignLeft
            }
        }

        ColumnLayout {
            spacing: 2

            Label {
                text: qsTr("FAILED")
                color: "grey"
            }

            Text {
                text: controller ? controller.state.failed : "0"
                color: "black"
                font.weight: Font.Medium

                Layout.preferredWidth: 40
                horizontalAlignment: Text.AlignLeft
            }
        }
    }
}
