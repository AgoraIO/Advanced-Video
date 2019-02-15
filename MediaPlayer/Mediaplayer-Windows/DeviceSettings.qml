import QtQuick 2.5
import "./controls"

DeviceSettingsForm {
    Item {
        width: parent.width
        height: 40
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        ADragArea {
            window: mainWindow
        }
    }
    Component.onCompleted: {
        var i, devices
        devices = agoraRtcEngine.getRecordingDeviceList()
//        console.log(devices.name)
        if (devices && devices.length > 0) {
            for (i = 0; i < devices.length; i++) {
                cbMicrophones.model.append({text: devices.name[i], guid: devices.guid[i]})
            }
        }

        devices = agoraRtcEngine.getPlayoutDeviceList()
//        console.log(devices.name)
        if (devices && devices.length > 0) {
            for (i = 0; i < devices.length; i++) {
                cbSpeakers.model.append({text: devices.name[i], guid: devices.guid[i]})
            }
        }

        devices = agoraRtcEngine.getVideoDeviceList()
//        console.log(devices.name)
        if (devices && devices.length > 0) {
            for (i = 0; i < devices.length; i++) {
                cbCameras.model.append({text: devices.name[i], guid: devices.guid[i]})
            }
        }

        sliderMicrophoneVolume.value = agoraRtcEngine.getRecordingDeviceVolume()
        sliderSpeakerVolume.value = agoraRtcEngine.getPalyoutDeviceVolume()
    }
    property bool testingMicrophone: false
    property bool testingSpeaker: false
    property bool testingCamera: false
    btnTestMichrophone.onClicked: {
        testingMicrophone = !testingMicrophone
        agoraRtcEngine.testMicrophone(testingMicrophone, 500)
        if (testingMicrophone)
            btnTestMichrophone.text = "Stop"
        else
            btnTestMichrophone.text = "Test"
    }

    btnTestSpeaker.onClicked: {
        testingSpeaker = !testingSpeaker
        agoraRtcEngine.testSpeaker(testingSpeaker)
        if (testingSpeaker)
            btnTestSpeaker.text = "Stop"
        else
            btnTestSpeaker.text = "Test"
    }

    btnTestCamera.onClicked: {
        testingCamera = !testingCamera
        agoraRtcEngine.testCamera(testingCamera, winVideo)
        if (testingCamera)
            btnTestCamera.text = "Stop"
        else
            btnTestCamera.text = "Test"
    }
    btnCancel.onClicked: {
        this.stopAllTests()
        mainWindow.close()
    }
    btnConfirm.onClicked: {
        this.stopAllTests()

        agoraRtcEngine.setRecordingDeviceVolume(sliderMicrophoneVolume.value)
        agoraRtcEngine.setPalyoutDeviceVolume(sliderSpeakerVolume.value)

        var current = cbMicrophones.model.get(cbMicrophones.currentIndex)
        if (current && current.guid)
            agoraRtcEngine.setRecordingDevice(current.guid)

        current = cbSpeakers.model.get(cbSpeakers.currentIndex)
        if (current && current.guid)
            agoraRtcEngine.setPlayoutDevice(current.guid)

        current = cbCamera.model.get(cbCamera.currentIndex)
        if (current && current.guid)
            agoraRtcEngine.setVideoDevice(current.guid)

        mainWindow.close()
    }
    function stopAllTests() {
        if (testingSpeaker) {
            testingSpeaker = false
            agoraRtcEngine.testSpeaker(testingSpeaker)
        }
        if (testingMicrophone) {
            testingMicrophone = false
            agoraRtcEngine.testMicrophone(testingMicrophone)
        }

        if (testingCamera) {
            testingCamera = false
            agoraRtcEngine.testCamera(testingCamera, null)
        }
    }
}

