# Customized encryption for Agora RTC SDK on iOS

*其他语言版本： [简体中文](README.zh.md)*

This project is an open-source demo that shows how to build a video chat application via Agora Video SDK, and make the data packets encrypted with your own algorithm.

With this sample app, you can:

- Join / leave channel
- Encrypt/Decrypt data packet for RTC SDK

A tutorial demo for Agora Video SDK can be found here: [Agora-iOS-Tutorial-Swift-1to1](https://github.com/AgoraIO/Agora-iOS-Tutorial-Swift-1to1)

You can find demo for Android here: [Agora-Customized-Encryption-Android](https://github.com/AgoraIO-Community/Agora-Customized-Encryption-Android)

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "AppID.swift" with your App ID.

```
let AppID: String = "Your App ID"
```

Next, download the **Agora Video SDK** from [Agora.io SDK](https://www.agora.io/en/download/). Unzip the downloaded SDK package and copy the **libs/AgoraRtcEngineKit.framework** to the "AgoraRtcCustomizedEncryptionTutorial" folder in project.

Finally, Open AgoraRtcCustomizedEncryptionTutorial.xcodeproj, connect your iPhone／iPad device, setup your development signing and run.

## Developer Environment Requirements
* XCode 9.0 +
* Real devices (iPhone or iPad)
* iOS simulator is NOT supported

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Agora-iOS-Tutorial-Swift-1to1/issues)

## License

The MIT License (MIT).
