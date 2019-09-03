import QtQuick 2.5
import "./controls"
import QtQuick.Controls 1.5

Rectangle {
    id: rectangleRoom
    width: 800
    height: 800
    property alias rectControlBar: rectControlBar
    property alias rectangleRoom: rectangleRoom
    property alias btnSettingsCall: btnSettings
    property alias btnVoiceCall: btnVoice
    property alias btnMuteVoice: btnMuteVoice
    property alias btnExpandView: btnExpandView
    property alias btnEndCall: btnEndCall
    property int maxViewWidth: rectangleRoom.width
    property int maxViewHeight: rectangleRoom.height-titleBar.height-rectToolbar.height
    ATitleBar {
        id: titleBar
        width: parent.width
        height: 24
        anchors.top: parent.top
        anchors.topMargin: 0
    }
    property alias channelName: channelName
    Text {
        id: channelName
        x: 342
        y: 50
        width: 100
        color: "#ffffff"
        text: qsTr("")
        font.bold: true
        z: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: titleBar.bottom
        anchors.topMargin: 20
        font.pixelSize: 14
    }

    property alias localVideo: localVideo
    AVideoView {
        id: localVideo
        x: 0
        showVideo: true
        width: rectangleRoom.maxViewWidth
        height: rectangleRoom.maxViewHeight
        backgroundImage: "images/icon_background_video.png"
        anchors.top: titleBar.bottom
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
    }
    property alias remoteVideo1: remoteVideo1
    AVideoView {
        id: remoteVideo1
        visible: false
        showVideo: false
        z: 1
        width: rectangleRoom.maxViewWidth/4
        height: rectangleRoom.maxViewHeight/4
        backgroundImage: "images/icon_background_video.png"
        anchors.top: titleBar.bottom
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
    }

    property alias remoteVideo2: remoteVideo2
    AVideoView {
        id: remoteVideo2
        visible: false
        showVideo: false
        z: 1
        width: rectangleRoom.maxViewWidth/4
        height: rectangleRoom.maxViewHeight/4
        backgroundImage: "images/icon_background_video.png"
        anchors.top: titleBar.bottom
        anchors.topMargin: rectangleRoom.maxViewHeight/4
        anchors.right: parent.right
        anchors.rightMargin: 0
    }

    property alias remoteVideo3: remoteVideo3
    AVideoView {
        id: remoteVideo3
        visible: false
        showVideo: false
        z: 1
        width: rectangleRoom.maxViewWidth/4
        height: rectangleRoom.maxViewHeight/4
        backgroundImage: "images/icon_background_video.png"
        anchors.top: titleBar.bottom
        anchors.topMargin: rectangleRoom.maxViewHeight/2
        anchors.right: parent.right
        anchors.rightMargin: 0
    }

    property alias remoteVideo4: remoteVideo4
    AVideoView {
        id: remoteVideo4
        visible: false
        showVideo: false
        z: 1
        width: rectangleRoom.maxViewWidth/4
        height: rectangleRoom.maxViewHeight/4
        backgroundImage: "images/icon_background_video.png"
        anchors.top: titleBar.bottom
        anchors.topMargin: rectangleRoom.maxViewHeight*3/4
        anchors.right: parent.right
        anchors.rightMargin: 0
    }
    property alias sliderHorizontal1:sliderHorizontal1
    Slider {
        id: sliderHorizontal1
        x: 0
        y: 682
        width: 800
        height: 22
        anchors.left: rectToolbar.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 96
        anchors.right: rectToolbar.right
        anchors.rightMargin: 0
        maximumValue: 1000



    }

    Rectangle {
        id: rectToolbar
        y: 705
        height: 96
        color: "#262626"
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        Rectangle {
            id: rectControlBar
            x: 109
            y: 0
            width: 432
            height: rectToolbar.height
            color: "#00000000"
            anchors.horizontalCenter: parent.horizontalCenter

            AImageButton {
                id: btnSettings
                width: 72
                height: 72
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.verticalCenter: parent.verticalCenter
                defaultImageSource: "images/btn_settings.png"
                hoverImageSource: "images/btn_settings_touch.png"
                pressedImageSource: "images/btn_settings_push.png"
            }

            AImageButton {
                id: btnVoice
                width: 72
                height: 72
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: btnSettings.right
                anchors.leftMargin: 48
                defaultImageSource: "images/btn_voice.png"
                hoverImageSource: "images/btn_voice_touch.png"
                pressedImageSource: "images/btn_voice_push.png"
            }

            AImageButton {
                id: btnMuteVoice
                width: 72
                height: 72
                anchors.left: btnVoice.right
                anchors.leftMargin: 48
                anchors.verticalCenter: parent.verticalCenter
                defaultImageSource: "images/btn_mute_close.png"
                hoverImageSource: "images/btn_mute_close_touch.png"
                pressedImageSource: "images/btn_mute_close_push.png"
            }

            AImageButton {
                id: btnExpandView
                width: 72
                height: 72
                anchors.left: btnMuteVoice.right
                anchors.leftMargin: 48
                anchors.verticalCenter: parent.verticalCenter
                defaultImageSource: "images/btn_maximize.png"
                hoverImageSource: "images/btn_maximize_touch.png"
                pressedImageSource: "images/btn_maximize_push.png"
            }
        }
        AImageButton {
            id: btnEndCall
            width: 72
            height: 72
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: rectToolbar.right
            anchors.rightMargin: 48
            defaultImageSource: "images/btn_endcall.png"
            hoverImageSource: "images/btn_endcall.png"
            pressedImageSource: "images/btn_endcall_push.png"
        }
    }

    property alias playorpause : playorpause
    Button {
        id: playorpause
        y: 733
        width: 71
        text: qsTr("play/pause")
        anchors.left: parent.left
        anchors.leftMargin: 99
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 33
    }

    property alias openvideobtn : openvideobtn
    Button {
        id: openvideobtn
        x: 7
        y: 710
        width: 75
        height: 39
        visible: true
        text: qsTr("openVideo")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 36

    }



}

