import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WebCrawlerApp 1.0

Item {
    id: infoPanel
    implicitHeight: mainColumn.implicitHeight
    property CrawlerViewModel crawlerViewModel: null
    readonly property int controlState: crawlerViewModel ? crawlerViewModel.state.controlState : CrawlerViewModel.IDLE

    ColumnLayout {
        id: mainColumn
        anchors.fill: parent
        spacing: 6

        ProgressBar{
            id: progressBar
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            from: 0
            to: 100
            value: crawlerViewModel ? crawlerViewModel.progress : 0
            background: Rectangle{
                anchors.fill: parent
                color: "#e0e0e0"
                border.width: 1
                border.color: "transparent"
            }

            contentItem: Item {
                implicitHeight: 8
                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    color: {
                        switch (controlState) {
                            case CrawlerViewModel.RUN: return "green"
                            case CrawlerViewModel.PAUSE: return "orange"
                            case CrawlerViewModel.STOP: return crawlerViewModel.progress === 100.0 ? "green" : "red"
                            case CrawlerViewModel.IDLE: return "#e0e0e0"
                            default: return "#e0e0e0"

                        }
                    }
                }
            }
        }

        RowLayout {
            id: rowLayout
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            spacing: 12

            Repeater {
                model: [
                    qsTr("DISCOVERED"),
                    qsTr("QUEUE"),
                    qsTr("FETCHED"),
                    qsTr("FAILED")
                ]

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    Label {
                        Layout.fillWidth: true
                        text: modelData
                        color: "grey"
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Text {
                        Layout.fillWidth: true
                        text: {
                            if (!crawlerViewModel)
                                return "0"

                            switch (index) {
                                case 0: return crawlerViewModel.state.discovered
                                case 1: return crawlerViewModel.state.queued
                                case 2: return crawlerViewModel.state.fetched
                                case 3: return crawlerViewModel.state.failed
                                default: return "0"
                            }
                        }

                        color: "black"
                        font.weight: Font.Medium
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
