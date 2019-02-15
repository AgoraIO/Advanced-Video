import QtQuick 2.5
import QtQuick.Controls 1.3
import "./controls"

Rectangle {
    id: rectangle1
    property alias cbResolution: cbResolution
    property alias cbMaxFrameRate: cbMaxFrameRate
    property alias sliderMaxBitrate: sliderMaxBitrate
    property alias txtMaxBitrate: txtMaxBitrate
    property alias btnConfirm: btnConfirm

    width: 600
    height: 600
    ATitleBar {
        width: parent.width
        height: 24
        anchors.top: parent.top
        anchors.topMargin: 0
    }

    Text {
        id: text1
        color: "#009eeb"
        text: qsTr("Resolution")
        anchors.top: parent.top
        anchors.topMargin: 96
        anchors.left: parent.left
        anchors.leftMargin: 145
        font.bold: true
        font.pixelSize: 14
    }

    AComboBox {
        id: cbResolution
        width: 300
        height: 40
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: text1.bottom
        anchors.topMargin: 11
        editable: false
        model: ListModel {
         ListElement {text: "160x120"}
         ListElement {text: "320x180"}
         ListElement {text: "320x240"}
         ListElement {text: "640x360"}
         ListElement {text: "640x480"}
         ListElement {text: "1280x720"}
         ListElement {text: "1920x1080"}
         ListElement {text: "3840x2160"}
        }
    }

    Text {
        id: text2
        color: "#009eeb"
        text: qsTr("Max Frame Rate")
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: cbResolution.bottom
        anchors.topMargin: 30
        font.bold: true
        font.pixelSize: 14
    }

    AComboBox {
        id: cbMaxFrameRate
        width: 300
        height: 40
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: text2.bottom
        anchors.topMargin: 11
        editable: false
        model: ListModel {
         ListElement {text: "15"}
         ListElement {text: "30"}
         ListElement {text: "60"}
        }
    }

    Text {
        id: text3
        color: "#009eeb"
        text: qsTr("Max Bit Rate")
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: cbMaxFrameRate.bottom
        anchors.topMargin: 30
        font.bold: true
        font.pixelSize: 14
    }

    ASlider {
        id: sliderMaxBitrate
        width: 216
        anchors.top: text3.bottom
        anchors.topMargin: 21
        anchors.left: text1.left
        anchors.leftMargin: 0
        stepSize: 5
    }

    Text {
        id: txtMaxBitrate
        x: 391
        text: qsTr("3mbps")
        anchors.top: sliderMaxBitrate.top
        anchors.topMargin: 0
        anchors.right: cbMaxFrameRate.right
        anchors.rightMargin: 0
        horizontalAlignment: Text.AlignRight
        font.bold: true
        font.pixelSize: 16
    }

    AButton {
        id: btnConfirm
        width: 300
        height: 48
        text: qsTr("Confirm")
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: sliderMaxBitrate.bottom
        anchors.topMargin: 56
        defaultColor: "#009eed"
        hoverColor: "#71caf5"
        pressedColor: "#1c1c1c"
        color: "#009eed"
        textColor: "#ffffff"
    }
    Text {
        id: txtAgora
        x: 266
        y: 513
        color: "#009eeb"
        text: qsTr("agora.io")
        font.underline: true
        anchors.bottom: txtVersion.top
        anchors.bottomMargin: 12
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 14
    }

    Text {
        id: txtVersion
        x: 288
        y: 542
        color: "#9196a0"
        text: qsTr("v1.2")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        font.pixelSize: 14
    }
}

