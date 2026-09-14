import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WebCrawlerApp 1.0

Item {
    id: headerBar
    implicitHeight: 30
    property CrawlerViewModel crawlerViewModel: null
    property int elapsedTime: 0

    readonly property int controlState: crawlerViewModel ? crawlerViewModel.state.controlState : CrawlerViewModel.IDLE;
    onControlStateChanged: {
        if (controlState === CrawlerViewModel.RUN) {
            elapsedTime = 0;
        }
    }

    Timer {
        id: elapsedTimer
        repeat: true
        interval: 1000
        running: crawlerViewModel && crawlerViewModel.state.running
        onTriggered: elapsedTime++
    }

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 12

        Label {
            text: "WEBCRAWLER v0.0.1"
            color: "green"
            font.weight: Font.Medium
        }

        Item {
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 6
            Label {
                text: qsTr("ELAPSED")
                color: "grey"
            }
            Text {
                function formatTime(totalSeconds) {
                    let hours = Math.floor(totalSeconds / 3600);
                    let minutes = Math.floor((totalSeconds % 3600) / 60);
                    let seconds = totalSeconds % 60;

                    let pad = (num) => String(num).padStart(2, '0');
                    return `${pad(hours)}:${pad(minutes)}:${pad(seconds)}`;
                }

                text: formatTime(elapsedTime)
                color: "black"
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignLeft
            }
        }

        RowLayout {
            spacing: 6

            Rectangle {
                id: statusIndicator
                width: 6
                height: 6
                radius: width / 2

                color: controlState === CrawlerViewModel.RUN ? "green" :
                       controlState === CrawlerViewModel.PAUSE ? "orange" :
                       controlState === CrawlerViewModel.STOP ? "red" : "grey"

                opacity: controlState === CrawlerViewModel.RUN ? animOpacity : 1.0
                property real animOpacity: 1.0

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    running: controlState === CrawlerViewModel.RUN
                    onRunningChanged: if (!running) statusIndicator.animOpacity = 1.0

                    PropertyAnimation {
                        target: statusIndicator
                        property: "animOpacity"
                        to: 0.3
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }

                    PropertyAnimation {
                        target: statusIndicator
                        property: "animOpacity"
                        to: 1.0
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                }
            }

            Label {
                text: controlState === CrawlerViewModel.RUN ? "CRAWLING" :
                      controlState === CrawlerViewModel.PAUSE ? "PAUSED"   :
                      controlState === CrawlerViewModel.STOP ? "STOPPED" : "IDLE"

                color: controlState === CrawlerViewModel.RUN ? "green"  :
                       controlState === CrawlerViewModel.PAUSE ? "orange" :
                       controlState === CrawlerViewModel.STOP ? "red" : "grey"

                Layout.preferredWidth: 80
            }
        }
    }
}
