# Agora RTC With CallKit

*其他语言版本： [简体中文](README.zhCN.md)*

The Agora-Video-With-CallKit sample app is an open-source demo that will help you get live video chat integrated into your iOS CallKit applications using the Agora Video SDK.

With this sample app, you can:

- Start a simulated video chat call
- Receive a simulated video chat call
- Mute / unmute audio during call

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "ViewController.swift" with your App ID.

Next, download the **Agora Video SDK** from [Agora.io SDK](https://www.agora.io/en/blog/download/). Unzip the downloaded SDK package and copy the **libs/AgoraRtcEngineKit.framework** to the "AgoraRTCWithCallKit" folder in project.

Finally, Open AgoraRTCWithCallKit.xcodeproj, connect your iPhone／iPad device, setup your development signing and run.

## Usage
1. Input phone number in textfield, "Return" to dismiss keyboard
2. Touch the call button to simulate an outgoing video chat call
3. Touch the incoming call button to simulate an incoming video chat call
4. Two devices with same phone number inputed can see each other

## Developer Environment Requirements
* Xcode 9.0 +
* iOS 10.0 +
* Real devices (iPhone or iPad), iOS simulator is NOT supported

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).
