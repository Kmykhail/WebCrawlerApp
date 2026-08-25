import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Crawler 1.0
import "components"

ApplicationWindow {
    visible: true
    width: 1280
    height: 720
    title: qsTr("WebCrawler")
    background: Rectangle {
        anchors.fill: parent
        color: "white"
    }

    CrawlerController {
        id: controller
    }

    ColumnLayout {
        anchors.fill: parent

        HeaderBar {
            Layout.fillWidth: true
        }

        ControlPanel {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            controller: controller
        }

        UrlTableView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            controller: controller
        }
    }
}
