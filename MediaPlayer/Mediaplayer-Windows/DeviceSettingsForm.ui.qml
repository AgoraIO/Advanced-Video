import QtQuick 2.5
import "./controls"
import AVideoWidget 1.0

Rectangle {
    id: rectangle1
    width: 512
    height: 640

    Text {
        id: text1
        color: "#009eeb"
        text: qsTr("Input Device")
        font.bold: false
        anchors.left: parent.left
        anchors.leftMargin: 66
        anchors.top: parent.top
        anchors.topMargin: 40
        font.pixelSize: 14
    }

    property alias cbMicrophones: cbMicrophones
    AComboBox {
        id: cbMicrophones
        width: 300
        height: 40
        fontSize: 9
        fontBold: false
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: text1.bottom
        anchors.topMargin: 11
        model: ListModel {
//         ListElement {text: "160x120"}
        }
    }

    property alias btnTestMichrophone: btnTestMichrophone
    AButton {
        id: btnTestMichrophone
        width: 72
        height: 40
        defaultColor: "#009eed"
        hoverColor: "#71caf5"
        pressedColor: "#1c1c1c"
        color: "#009eed"
        textColor: "#ffffff"
        text: qsTr("Test")
        anchors.left: cbMicrophones.right
        anchors.leftMargin: 8
        anchors.top: cbMicrophones.top
        anchors.topMargin: 0
    }

    Text {
        id: text2
        width: 72
        height: 24
        text: qsTr("Volume")
        anchors.left: text1.left
        anchors.leftMargin: 0
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors.top: cbMicrophones.bottom
        anchors.topMargin: 18
        font.pixelSize: 12
    }

    property alias sliderMicrophoneVolume: sliderMicrophoneVolume
    ASlider {
        id: sliderMicrophoneVolume
        width: 284
        height: 24
        minimumValue: 0
        maximumValue: 255
        anchors.top: text2.top
        anchors.topMargin: 0
        anchors.left: text2.right
        anchors.leftMargin: 12
    }

    Text {
        id: text3
        color: "#009eeb"
        text: qsTr("Output Device")
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: text2.bottom
        anchors.topMargin: 24
        font.pixelSize: 14
    }

    property alias cbSpeakers: cbSpeakers
    AComboBox {
        id: cbSpeakers
        width: 300
        height: 40
        fontSize: 9
        fontBold: false
        anchors.left: text1.left
        anchors.leftMargin: 0
        anchors.top: text3.bottom
        anchors.topMargin: 11
        model: ListModel {
//         ListElement {text: "160x120"}
        }
    }

    property alias btnTestSpeaker: btnTestSpeaker
    AButton {
        id: btnTestSpeaker
        width: 72
        height: 40
        defaultColor: "#009eed"
        hoverColor: "#71caf5"
        pressedColor: "#1c1c1c"
        color: "#009eed"
        textColor: "#ffffff"
        text: qsTr("Test")
        anchors.left: cbSpeakers.right
        anchors.leftMargin: 8
        anchors.top: cbSpeakers.top
        anchors.topMargin: 0
    }

    Text {
        id: text4
        x: 0
        y: 0
        width: 72
        height: 24
        text: qsTr("Volume")
        anchors.topMargin: 18
        anchors.top: cbSpeakers.bottom
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        anchors.left: text1.left
        anchors.leftMargin: 0
        horizontalAlignment: Text.AlignLeft
    }

    property alias sliderSpeakerVolume: sliderSpeakerVolume
    ASlider {
        id: sliderSpeakerVolume
        x: 3
        y: 2
        width: 284
        height: 24
        minimumValue: 0
        maximumValue: 255
        anchors.topMargin: 0
        anchors.top: text4.top
        anchors.left: text4.right
        anchors.leftMargin: 12
    }

    Text {
        id: text5
        x: 2
        y: -5
        color: "#009eeb"
        text: qsTr("Camera")
        anchors.topMargin: 24
        anchors.top: text4.bottom
        font.pixelSize: 14
        anchors.left: text1.left
        anchors.leftMargin: 0
    }

    property alias cbCameras: cbCameras
    AComboBox {
        id: cbCameras
        x: -6
        width: 300
        height: 40
        fontSize: 9
        fontBold: false
        anchors.top: text5.bottom
        anchors.topMargin: 11
        anchors.left: text1.left
        anchors.leftMargin: 0
        model: ListModel {
//         ListElement {text: "160x120"}
        }
    }

    property alias btnTestCamera: btnTestCamera
    AButton {
        id: btnTestCamera
        x: 374
        y: -6
        width: 72
        height: 40
        defaultColor: "#009eed"
        hoverColor: "#71caf5"
        pressedColor: "#1c1c1c"
        color: "#009eed"
        textColor: "#ffffff"
        text: qsTr("Test")
        anchors.topMargin: 0
        anchors.top: cbCameras.top
        anchors.left: cbCameras.right
        anchors.leftMargin: 8
    }
    property alias winVideo: winVideo
    Rectangle {
        x: 150
        width: 192
        height: 120
        color: "#c8c8c8"
        anchors.top: cbCameras.bottom
        anchors.topMargin: 18
        anchors.horizontalCenter: parent.horizontalCenter
        AVideoWidget {
            id: winVideo
            width: parent.width
            height: parent.height
        }
    }

    property alias btnCancel : btnCancel
    AButton {
        id: btnCancel
        width: 180
        height: 48
        text: qsTr("Cancel")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 66
    }
    property alias btnConfirm : btnConfirm
    AButton {
        id: btnConfirm
        width: 180
        height: 48
        defaultColor: "#f8aa1e"
        hoverColor: "#eebf6b"
        pressedColor: "#1c1c1c"
        color: "#f8aa1e"
        text: qsTr("Confirm")
        anchors.left: btnCancel.right
        anchors.leftMargin: 20
        anchors.top: btnCancel.top
        anchors.topMargin: 0
    }
}

