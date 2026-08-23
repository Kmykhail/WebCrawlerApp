import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 1280
    height: 720
    title: qsTr("WebCrawler")

    Column {
        anchors.fill: parent

        HeaderBar {}
        ControlPanel {}
        UrlListView {}
    }
}
