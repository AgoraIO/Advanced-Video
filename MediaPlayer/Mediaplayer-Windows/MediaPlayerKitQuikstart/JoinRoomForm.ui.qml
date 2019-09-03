import QtQuick 2.5
import "./controls"

Rectangle {
    id: rectangle1
    property alias btnJoin: btnJoin
    property alias btnBack: btnBack
    property alias btnVideoSettings: btnVideoSettings
    property alias txtChannelName: txtChannelName

    width: 600
    height: 600
    color: "#ffffff"

    ATitleBar {
        width: parent.width
        height: 24
        anchors.top: parent.top
        anchors.topMargin: 0
    }

    Text {
        id: text1
        width: 300
        height: 57
        color: "#009eeb"
        text: qsTr("Enter a conference room name")
        anchors.left: parent.left
        anchors.leftMargin: 145
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignLeft
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 96
        font.bold: true
        font.pixelSize: 24
    }

    Text {
        id: text2
        width: 300
        height: 61
        color: "#9196a0"
        text: qsTr("If you are the first person to specify this name, the room will be created and you will be placed in it. If it has already been created you will join the conference in progress.")
        anchors.left: parent.left
        anchors.leftMargin: 145
        anchors.top: text1.bottom
        anchors.topMargin: 12
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: 12
    }

    Rectangle {
        id: txtChannelName
        property alias text: textInput.text
        width: 300
        height: 48
        color: "#f5f5f5"
        anchors.top: text2.bottom
        anchors.topMargin: 29
        border.color: "#c8c8c8"
        anchors.left: parent.left
        anchors.leftMargin: 145
        MouseArea {
            anchors.fill: parent
        }
        TextInput {
            id: textInput
            width: 300
            height: 48
            color: "#000000"
            text: qsTr("")
            anchors.top: parent.top
            anchors.topMargin: 0
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.centerIn: parent
            opacity: 1
            selectionColor: "#000000"
            font.bold: true
            font.pixelSize: 16
            focus: true
        }
    }

    AButton {
        id: btnBack
        width: 144
        height: 48
        defaultColor: "#009eed"
        hoverColor: "#71caf5"
        pressedColor: "#1c1c1c"
        color: "#009eed"
        textColor: "#ffffff"
        text: qsTr("Back")
        anchors.left: parent.left
        anchors.leftMargin: 145
        anchors.top: txtChannelName.bottom
        anchors.topMargin: 10
    }
    AButton {
        id: btnJoin
        width: 144
        height: 48
        defaultColor: "#f8aa1e"
        hoverColor: "#eebf6b"
        pressedColor: "#1c1c1c"
        color: "#f8aa1e"
        text: qsTr("Join")
        anchors.top: txtChannelName.bottom
        anchors.topMargin: 10
        textColor: "#ffffff"
        anchors.left: btnBack.right
        anchors.leftMargin: 12
    }

    Rectangle {
        id: rectSep
        x: 145
        width: 300
        height: 1
        color: "#c8c8c8"
        anchors.top: btnJoin.bottom
        anchors.topMargin: 23
    }

    AButton {
        id: btnVideoSettings
        x: 144
        width: 300
        height: 48
        text: qsTr("1920x1080, 15fps, 3mbps")
        defaultColor: "#009eed"
        hoverColor: "#71caf5"
        pressedColor: "#1c1c1c"
        color: "#009eed"
        textColor: "#ffffff"
        anchors.top: rectSep.bottom
        anchors.topMargin: 23
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

