import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WebCrawlerApp 1.0

ApplicationWindow {
    visible: true
    width: 1280
    height: 1024
    title: qsTr("WebCrawler")
    background: Rectangle {
        anchors.fill: parent
        color: "white"
    }

    CrawlerViewModel {
        id: viewModel
    }

    ColumnLayout {
        anchors.fill: parent

        HeaderBar {
            Layout.fillWidth: true
            crawlerViewModel: viewModel
        }

        ControlPanel {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            crawlerViewModel: viewModel
        }

        InfoPanel {
            Layout.fillWidth: true
            crawlerViewModel: viewModel
        }

        UrlTableView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            crawlerViewModel: viewModel
        }

        ConsolPanel {
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            logModel: viewModel.logModel
        }
    }
}
