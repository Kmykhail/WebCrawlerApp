import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WebCrawlerApp 1.0

Item {
    id: root
    clip: true

    property CrawlerController controller: null

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
                            case UrlModel.NumberColumn: return "#"
                            case UrlModel.TimeColumn: return qsTr("Time")
                            case UrlModel.StatusColumn: return qsTr("Status")
                            case UrlModel.DepthColumn: return qsTr("Depth")
                            case UrlModel.UrlColumn: return qsTr("Url")
                            case UrlModel.SizeColumn: return qsTr("Size")
                            case UrlModel.FetchedColumn: return qsTr("Fetched")
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

            columnWidthProvider: function(column) { // TODO fix width
                switch(column) {
                    case UrlModel.NumberColumn: return tableView.width * 0.08;
                    case UrlModel.TimeColumn: return tableView.width * 0.08;
                    case UrlModel.StatusColumn: return tableView.width * 0.08;
                    case UrlModel.DepthColumn: return tableView.width * 0.08;
                    case UrlModel.UrlColumn: return tableView.width * 0.42;
                    case UrlModel.SizeColumn: return tableView.width * 0.15;
                    case UrlModel.FetchedColumn: return tableView.width * 0.05;
                    default: return 100;
                }
            }

            onWidthChanged: tableView.forceLayout()

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
                    horizontalAlignment: column === 0 || column === 1 ? Text.AlignLeft : Text.AlignHCenter

                    text: {
                        if (url === undefined) return ""
                        switch(column) {
                            case UrlModel.NumberColumn: return index + 1
                            case UrlModel.TimeColumn: return Qt.formatTime(time, "hh:mm:ss")
                            case UrlModel.StatusColumn: return status
                            case UrlModel.DepthColumn: return depth
                            case UrlModel.UrlColumn: return url
                            case UrlModel.SizeColumn: return (htmlSize / 1024).toFixed(1) + " KB"
                            case UrlModel.FetchedColumn: return fetched ? "YES" : "NO"
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
