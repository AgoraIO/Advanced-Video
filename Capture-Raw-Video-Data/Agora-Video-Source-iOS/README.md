# Agora Video Source iOS

*其他语言版本： [简体中文](README.zh.md)*

The Agora Video Source iOS Sample App is an open-source demo that will help you use external video source with Agora Video SDK to start a video chat.

With this sample app, you can:

- Join / leave channel
- Mute / unmute audio
- Use capture session to get camera data
- Switch camera
- Enable / disable video

A tutorial demo can be found here: [Agora-iOS-Tutorial-Swift-1to1](https://github.com/AgoraIO/Agora-iOS-Tutorial-Swift-1to1)

You can find demo for Android here:

- [Agora-Video-Source-Android](https://github.com/AgoraIO/Agora-Video-Source-Android)

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "AppID.swift" with your App ID.

```
let AppID: String = "Your App ID"
```

Next, download the **Agora Video SDK** from [Agora.io SDK](https://www.agora.io/en/blog/download/). Unzip the downloaded SDK package and copy the **libs** folder to the project folder.

Finally, Open Agora Video Source.xcodeproj, connect your iPhone／iPad device, setup your development signing and run.

## Developer Environment Requirements
* XCode 8.0 +
* Real devices (iPhone or iPad)
* iOS simulator is NOT supported

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Agora-Video-Source-iOS/issues)

## License

The MIT License (MIT).
