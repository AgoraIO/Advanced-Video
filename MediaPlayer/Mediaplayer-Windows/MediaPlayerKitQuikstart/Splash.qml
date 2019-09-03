import QtQuick 2.5

SplashForm {
    mouseArea.onClicked: main.joinRoom()
    Timer {
        interval: 3000; running: true; repeat: false
        onTriggered: main.joinRoom()
    }
}

