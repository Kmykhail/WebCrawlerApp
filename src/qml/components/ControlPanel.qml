import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    width: parent.width
    height: 100
    color: "#222"

    Row {
        anchors.centerIn: parent
        spacing: 10

        TextField {
            placeholderText: "Target URL"
            width: 300
        }

        SpinBox {
            id: depthSpinBox
            from: 1
            to: 10
            value: 3
        }

        ComboBox {
            id: pageLimitComboBox
            model: ["10 pages", "25 pages", "50 pages"]
            currentIndex: 1
        }

        Button { text: "START" }
        Button { text: "PAUSE" }
        Button { text: "STOP" }
    }
}
