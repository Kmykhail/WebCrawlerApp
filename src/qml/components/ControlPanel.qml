import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Crawler 1.0

Item {
    id: controlPanel
    implicitHeight: 100
    property var controller: null

    Rectangle {
        anchors.fill: parent
        border.width: 1
        border.color: "grey"
        color: "#f5f5f5"
        GridLayout {
            anchors.centerIn: parent
            columns: 6
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

            Button {
                id: startButton
                text: "START"
                font.weight: Font.Medium
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

                        startButton.enabled = false;
                        pauseButton.enabled = true;
                        resumeButton.enabled = false;
                        stopButton.enabled = true;
                    }
                }
            }

            Button {
                id: pauseButton
                text: "PAUSE"
                enabled: false
                font.weight: Font.Medium
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

                        startButton.enabled = false;
                        pauseButton.enabled = false;
                        resumeButton.enabled = true;
                        stopButton.enabled = true;
                    }
                }
            }


            Button {
                id: resumeButton
                text: "RESUME"
                enabled: false
                font.weight: Font.Medium
                contentItem: Text {
                    text: resumeButton.text
                    font: resumeButton.font
                    color: resumeButton.enabled ? "green" : "grey"
                }

                background: Rectangle {
                    anchors.fill: parent
                    color: "#f5f5f5"
                    border.color: pauseButton.enabled ? "green" : "grey"
                    border.width: 1
                }
                onClicked: {
                    if (controller && controller.manager) {
                        controller.manager.resume();

                        startButton.enabled = false;
                        pauseButton.enabled = true;
                        resumeButton.enabled = false;
                        stopButton.enabled = true;
                    }
                }
            }

            Button {
                id: stopButton
                text: "STOP"
                enabled: false
                font.weight: Font.Medium
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

                        startButton.enabled = true;
                        pauseButton.enabled = false;
                        resumeButton.enabled = false;
                        stopButton.enabled = false;
                    }
                }
            }
        }
    }
}
