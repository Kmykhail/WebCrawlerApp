import QtQuick
import QtQuick.Controls
import Crawler 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    CrawlerManager {
        id: crawler
        onNewUrlFound: function(url) {
            urlModel.addUrl(url)
        }
        Component.onCompleted: {
            console.log("QML CrawlerManager instance address:", crawler)
            console.log("QML UrlModel instance address:", urlModel)
        }
    }

    UrlModel {
        id: urlModel
    }

    Column {
        Button {
            text: "Start"
            onClicked: crawler.start("https://luckyland.com.ua/")
        }

        Button {
            text: "Pause"
            onClicked: crawler.pause()
        }

        Button {
            text: "Stop"
            onClicked: crawler.stop()
        }

        ScrollView {
            width: parent.width
            height: 480

            ListView {
                id: listview
                width: parent.width
                height: contentHeight
                model: urlModel
                delegate: Rectangle {
                    width: listview.width; height: 25
                    color: "steelblue"
                    Text {
                        text: "#" + (index + 1) + ": " + model.url
                        color: "black"
                    }
                }
            }
        }
    }
}
