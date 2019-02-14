import QtQuick 2.5
import "./controls"
import QtQuick.Controls 1.5

Rectangle {
    id: rectangleRoom
    width: 800
    height: 800

    property alias localVideo: localVideo
    AVideoView {
        id: localVideo
        x: 0
        showVideo: true
        width: rectangleRoom.maxViewWidth
        height: rectangleRoom.maxViewHeight
        backgroundImage: "images/icon_background_video.png"
        anchors.top: rectangleRoom.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
    }
}

