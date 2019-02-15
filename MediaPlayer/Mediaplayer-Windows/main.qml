import QtQuick 2.5
import QtQuick.Controls 1.5


Item {
    property int videoResolution: 40
    property int maxFrameRate: 15
    property int maxBitrate: 300 //in 10kbps
    property string channelName: ""
    width: 800
    height: 600
    id: main

    Loader{
        id: loader
        focus: true
        anchors.fill: parent
    }
    
    Component.onCompleted: {
        splashPage()
    }
    function joinChannel(channel) {
        if (channel.length > 0 && agoraRtcEngine.joinChannel("", channel, 0) === 0) {
            channelName = channel
            //loader.setSource(Qt.resolvedUrl("InRoom.qml"))
        }
    }
    
    function leaveChannel() {
        if (agoraRtcEngine.leaveChannel() === 0) {
            channelName = ""
            joinRoom()
        }
    }
    function muteLocalAudioStream(muted) {
        return agoraRtcEngine.muteLocalAudioStream(muted)
    }
    function enableVideo(enabled) {

        return agoraRtcEngine.enableVideo(enabled)
    }
    function selectVideoSettings() {
        loader.setSource(Qt.resolvedUrl("VideoSettings.qml"))

    }
    function joinRoom() {
        loader.setSource(Qt.resolvedUrl("JoinRoom.qml"))

    }
    function splashPage() {
        loader.setSource(Qt.resolvedUrl("Splash.qml"))
    }

    function getVideoResolutionText() {
        switch (videoResolution) {
        case 0:
            return "160x120"
        case 10:
            return "320x180"
        case 20:
            return "320x240"
        case 30:
            return "640x360"
        case 40:
            return "640x480"
        case 50:
            return "1280x720"
        case 60:
            return "1920x1080"
        case 70:
            return "3840x2160"
        default:
            videoResolution = 40
            return "640x480"
        }
    }
    function getMaxFrameRateText() {
        return maxFrameRate.toString()+"fps"
    }
    function getMaxBitrateText() {
        return getMaxBitrateText2(main.maxBitrate)
    }

    function getMaxBitrateText2(val) {
        //val is in 10kbps
        if (val < 100)
            return (val*10).toFixed(0).toString()+"kbps"
        else {
            var n = val/100
            if (Math.abs(n.toFixed(0)*100 - val) < 0.1)
                return n.toFixed(0).toString()+"mbps"
            else
                return n.toFixed(2).toString()+"mbps"
        }
    }
}

