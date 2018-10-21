# Large Group Video Chat

*其他语言版本： [简体中文](README.zhCN.md)*

The Large Group Video Chat iOS Sample App is an open-source demo that will help you get a large group video chat using the Agora Video SDK.

With this sample app, you can:

- Join / leave channel
- Mute / unmute audio
- Switch camera
- Mute / unmute remote audio and video
- Group video chat up to 17 people

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "KeyCenter.swift" with your App ID.

```
static let AppId: String = "Your App ID"
```

Next, download the **Agora Video SDK** from [Agora.io SDK](https://www.agora.io/en/blog/download/). Unzip the downloaded SDK package and copy the **AgoraRtcEngineKit.framework** to the "LargeGroupVideoChat" folder in project.

Finally, Open LargeGroupVideoChat.xcodeproj, connect your iPhone／iPad device, setup your development signing and run.

## Developer Environment Requirements
* XCode 10.0 +
* Real devices (iPhone or iPad)
* iOS simulator is NOT supported

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).
