import QtQuick 2.5

Rectangle {
    id: titleBar
    width: 600
    height: 24
    color: "#009eeb"

    ADragArea {
        window: containerWindow
    }
    Text {
        id: titleText
        y: 0
        height: 24
        color: "#ffffff"
        text: "Agora Video Call"
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.leftMargin: 12
        font.pixelSize: 12
        font.bold: true
    }

    AImageButton {
        id: btnMinimize
        x: 546
        y: 0
        width: 24
        height: 24
        anchors.right: btnClose.left
        anchors.rightMargin: 0
        defaultImageSource: "/images/btn_window_minimize.png"
        hoverImageSource: "/images/btn_window_minimize_touch.png"
        pressedImageSource: "/images/btn_window_minimize_push.png"
        onClicked: containerWindow.showMinimized()
    }
    AImageButton {
        id: btnClose
        x: 525
        y: 1
        width: 24
        height: 24
        opacity: 1
        anchors.right: parent.right
        anchors.rightMargin: 0
        defaultImageSource: "/images/btn_window_close.png"
        hoverImageSource: "/images/btn_window_close_touch.png"
        pressedImageSource: "/images/btn_window_close_push.png"
        onClicked: containerWindow.close()
    }
}

