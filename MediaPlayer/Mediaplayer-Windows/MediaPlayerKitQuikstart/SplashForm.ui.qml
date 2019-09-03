import QtQuick 2.5
import "./controls"

Rectangle {
    id: splash
    property alias mouseArea: mouseArea
    width: 600
    height: 600

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }

    ATitleBar {
        width: parent.width
        height: 24
        anchors.top: parent.top
        anchors.topMargin: 0
    }

    Image {
        id: image1
        x: 236
        width: 130
        height: 72
        anchors.top: parent.top
        anchors.topMargin: 204
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        sourceSize.height: 72
        sourceSize.width: 130
        source: "images/icon_logo.png"
    }

    Text {
        id: text1
        x: 288
        color: "#9196a0"
        text: qsTr("Agora Video Call")
        anchors.top: image1.bottom
        anchors.topMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 16
    }

    Text {
        id: text2
        x: 289
        color: "#009deb"
        text: qsTr("Powering Real-Time Communications")
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: text1.bottom
        anchors.topMargin: 12
        font.pixelSize: 16
    }
}

