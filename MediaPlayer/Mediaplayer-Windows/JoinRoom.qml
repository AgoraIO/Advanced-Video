import QtQuick 2.5

JoinRoomForm {
    anchors.fill: parent
    btnJoin.onClicked: main.joinChannel(txtChannelName.text)
    btnBack.onClicked: main.splashPage()//containerWindow.openDeviceSettings()
    btnVideoSettings.onClicked: main.selectVideoSettings()
    Component.onCompleted: {
        btnVideoSettings.text =
                main.getVideoResolutionText()+", "+
                main.getMaxFrameRateText()+", "+
                main.getMaxBitrateText()
    }

}

