import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WebCrawlerApp 1.0

Item {
    id: controlPanel
    implicitHeight: 100
    property CrawlerController controller: null
    readonly property int controlState: controller ? controller.state.controlState : CrawlerController.IDLE;

    Rectangle {
        anchors.fill: parent
        border.width: 1
        border.color: "grey"
        color: "#f5f5f5"
        GridLayout {
            anchors.centerIn: parent
            columns: 8
            rowSpacing: 4
            columnSpacing: 15

            // ===first row===
            Label {
                text: qsTr("TARGET URL")
                color: "grey"
            }

            Label {
                id: depthLabel
                text: qsTr("DEPTH")
                color: "grey"
            }

            Label {
                id: limitLabel
                text: qsTr("PAGE LIMIT")
                color: "grey"
            }

            Item { Layout.fillWidth: true }
            Item { Layout.fillWidth: true }
            Item { Layout.fillWidth: true }
            Item { Layout.fillWidth: true }
            Item { Layout.fillWidth: true }

            // ===second row===
            TextField {
                id: textField
                placeholderText: "Target URL"
                implicitWidth: 300
                text: "https://news.google.com/home?hl=uk&gl=UA&ceid=UA%3Auk" // TODO: remove it
                color: "black"
                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: "grey"
                    border.width: 1
                }
            }

            SpinBox {
                id: depthSpinBox
                from: 1
                to: 10
                value: 3

                Layout.preferredWidth: Math.max(depthLabel.width, implicitWidth)

                contentItem: TextInput {
                    z: 2
                    text: depthSpinBox.textFromValue(depthSpinBox.value, depthSpinBox.locale)
                    font: depthSpinBox.font
                    color: "black"
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    readOnly: depthSpinBox.editable
                    validator: depthSpinBox.validator
                    inputMethodHints: depthSpinBox.inputMethodHints
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: "grey"
                    border.width: 1
                }
                onValueChanged: {
                    if (controller && controller.manager) {
                        controller.manager.setUrlDepth(depthSpinBox.value);
                    }
                }

                Component.onCompleted: {
                    if (controller && controller.manager) {
                        controller.manager.setUrlDepth(depthSpinBox.value);
                    }
                }
            }

            ComboBox {
                id: limitComboBox

                Layout.preferredWidth: 100
                Layout.preferredHeight: 25

                model: [
                    "100",
                    "500",
                    "1000",
                    "5000",
                    "10000",
                    "50000",
                    "100000",
                    "Unlimited"
                ]
                currentIndex: 0

                function updateUrlLimit() {
                    if (controller && controller.manager) {
                        controller.manager.setUrlLimit(currentText);
                    }
                }

                contentItem: Text {
                    leftPadding: 10
                    rightPadding:  limitComboBox.indicator.width + limitComboBox.spacing
                    text: limitComboBox.displayText
                    font: limitComboBox.font
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: "grey"
                    border.width: 1
                }

                onActivated: updateUrlLimit()
                delegate: ItemDelegate {
                    width: limitComboBox.width
                    contentItem: Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: "black"
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: highlighted ? "#e0e0e0" : "white"
                    }
                }

                Component.onCompleted: updateUrlLimit()
            }

            Button {
                id: startButton
                text: "START"
                font.weight: Font.Medium
                enabled: controlState === CrawlerController.IDLE || controlState == CrawlerController.STOP
                contentItem: Text {
                    text: startButton.text
                    font: startButton.font
                    color: startButton.enabled ? "green" : "grey"
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: startButton.enabled ? "green" : "grey"
                    border.width: 1
                }


                onClicked: {
                    if (controller && controller.manager) {
                        controller.manager.start(textField.text);
                    }
                }
            }

            Button {
                id: pauseButton
                text: "PAUSE"
                font.weight: Font.Medium
                enabled: controlState === CrawlerController.RUN
                contentItem: Text {
                    text: pauseButton.text
                    font: pauseButton.font
                    color: pauseButton.enabled ? "orange" : "grey"
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: pauseButton.enabled ? "orange" : "grey"
                    border.width: 1
                }

                onClicked: {
                    if (controller && controller.manager) {
                        controller.manager.pause();
                    }
                }
            }

            Button {
                id: resumeButton
                text: "RESUME"
                font.weight: Font.Medium
                enabled: controlState === CrawlerController.PAUSE
                contentItem: Text {
                    text: resumeButton.text
                    font: resumeButton.font
                    color: resumeButton.enabled ? "green" : "grey"
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: resumeButton.enabled ? "green" : "grey"
                    border.width: 1
                }
                onClicked: {
                    if (controller && controller.manager) {
                        controller.manager.resume();
                    }
                }
            }

            Button {
                id: stopButton
                text: "STOP"
                font.weight: Font.Medium
                enabled: controlState === CrawlerController.RUN  || controlState === CrawlerController.PAUSE
                contentItem: Text {
                    text: stopButton.text
                    font: stopButton.font
                    color: stopButton.enabled ? "red" : "grey"
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: stopButton.enabled ? "red" : "grey"
                    border.width: 1
                }

                onClicked: {
                    if (controller && controller.manager) {
                        controller.manager.stop();
                    }
                }
            }

            Button {
                id: clearButton
                text: "CLEAR"
                font.weight: Font.Medium
                enabled: controlState === CrawlerController.STOP
                contentItem: Text {
                    text: clearButton.text
                    font: clearButton.font
                    color: clearButton.enabled ? "black" : "grey"
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: clearButton.enabled ? "black" : "grey"
                    border.width: 1
                }

                onClicked: {
                    if (controller && controller.manager) {
                        controller.manager.clear();
                    }
                }
            }
        }
    }
}
