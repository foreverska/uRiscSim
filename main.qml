import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Extras 1.4
import QtQuick.Controls 1.6
import QtQuick.Dialogs 1.2

import Qt.labs.settings 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    maximumWidth: width
    maximumHeight: height
    title: qsTr("uRiscSim")

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "Load Flash"
                onTriggered: {
                    loadFlashDialog.visible = true
                }
            }
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
            }
        }
    }

    TextArea {
        x: 50
        y: 100
        id: serialOutput
        readOnly: true
        text: riscvCore.serialString
    }

    Text {
        anchors.left: gpioOutputRect.left
        anchors.bottom: gpioOutputRect.top
        id: gpioOutputRectLabel
        text: qsTr("GPIO - Output")
    }

    Rectangle {
        id: gpioOutputRect
        anchors.top: serialOutput.top
        anchors.left: serialOutput.right
        anchors.leftMargin: 33
        width: 266
        height: 60
        color: "transparent"
        border.width: 2
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            spacing: 20
            StatusIndicator {
                id: si1
                active: (riscvCore.gpo & 0x1) != 0
            }
            StatusIndicator {
                id: si2
                active: (riscvCore.gpo & 0x2) != 0
            }
            StatusIndicator {
                id: si3
                active: (riscvCore.gpo & 0x4) != 0
            }
            StatusIndicator {
                id: si4
                active: (riscvCore.gpo & 0x8) != 0
            }
        }
    }

    Text {
        anchors.left: gpioInputRect.left
        anchors.bottom: gpioInputRect.top
        id: gpioInputRectLabel
        text: qsTr("GPIO - Input")
    }

    Rectangle {
        id: gpioInputRect
        anchors.top: gpioOutputRect.bottom
        anchors.left: gpioOutputRect.left
        anchors.topMargin: 33
        width: 266
        height: 60
        color: "transparent"
        border.width: 2
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10
            ToggleButton {
                id: tb1
                height: 55
                width: height
                onClicked:
                {
                    riscvCore.gpiChanged(0, checked)
                }
            }
            ToggleButton {
                id: tb2
                height: 55
                width: height
                onClicked:
                {
                    riscvCore.gpiChanged(1, checked)
                }
            }
            ToggleButton {
                id: tb3
                height: 55
                width: height
                onClicked:
                {
                    riscvCore.gpiChanged(2, checked)
                }
            }
            ToggleButton {
                id: tb4
                height: 55
                width: height
                onClicked:
                {
                    riscvCore.gpiChanged(3, checked)
                }
            }
        }
    }

    Rectangle {
        id: controllsRect
        height: 60
        width : 266
        color: "transparent"
        anchors.top: serialOutput.bottom
        anchors.left: serialOutput.left
        anchors.topMargin: 50
        Column {
            spacing: 20
            Row {
                Button {
                    id: playBtn
                    text: riscvCore.rpString
                    onClicked: riscvCore.runPauseCore()
                }
                Button {
                    id: pauseBtn
                    text: "Stop"
                    onClicked: riscvCore.stopCore()
                }
                Button {
                    id: resetBtn
                    text: "Reset"
                    onClicked: riscvCore.resetCore()
                }
            }
            Row {
                spacing: 10
                Text {
                    anchors.verticalCenter: pcVal.verticalCenter
                    text: "PC: "
                }
                TextField {
                    id: pcVal
                    readOnly: true
                    text: "0x" + riscvCore.pc.toString(16)
                }
            }
        }
    }

    FileDialog {
        id: loadFlashDialog
        title: "Choose Flash to Load"
        folder: shortcuts.home
        onAccepted: {
            console.log("Load Rom")
            riscvCore.loadRom(fileUrl);
        }
    }
}
