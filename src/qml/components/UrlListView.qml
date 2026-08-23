import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    width: parent.width
    height: parent.height - 150
    model: urlModel

    delegate: Rectangle {
        width: parent.width
        height: 40
        color: index % 2 === 0 ? "#444" : "#555"

        Row {
            anchors.fill: parent
            spacing: 10

            Text { text: time; width: 80 }
            Text { text: status; width: 50 }
            Text { text: depth; width: 50 }
            Text { text: url; width: 400 }
            Text { text: size; width: 80 }
        }
    }
}
