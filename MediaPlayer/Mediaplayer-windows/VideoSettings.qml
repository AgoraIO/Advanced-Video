import QtQuick 2.5

VideoSettingsForm {
    anchors.fill: parent
    Component.onCompleted: {
        sliderMaxBitrate.maximumValue = 8
        sliderMaxBitrate.maximumValue = 1600

        updateResolutionControl()
        updateMaxFrameRateControl()
        updateMaxBitrateControl()
    }
    btnConfirm.onClicked: {
        updateVideoSettingsValues()
        main.joinRoom()
    }
    sliderMaxBitrate.onValueChanged: {
        txtMaxBitrate.text = main.getMaxBitrateText2(sliderMaxBitrate.value)
    }

    function updateVideoSettingsValues() {
        main.videoResolution = cbResolution.currentIndex*10
        switch (cbMaxFrameRate.currentIndex)
        {
        case 0:
            main.maxFrameRate = 15
            break
        case 1:
            main.maxFrameRate = 30
            break
        case 2:
            main.maxFrameRate = 60
            break
        }
        main.maxBitrate = sliderMaxBitrate.value
    }

    function updateResolutionControl() {
        cbResolution.currentIndex = main.videoResolution/10
    }

    function updateMaxFrameRateControl() {
        switch (main.maxFrameRate)
        {
        case 15:
            cbMaxFrameRate.currentIndex = 0
            break
        case 30:
            cbMaxFrameRate.currentIndex = 1
            break
        case 60:
            cbMaxFrameRate.currentIndex = 2
            break
        }
    }
    function updateMaxBitrateControl() {
        sliderMaxBitrate.value = main.maxBitrate
        //txtMaxBitrate.text = main.getMaxBitrateText()
    }
}
