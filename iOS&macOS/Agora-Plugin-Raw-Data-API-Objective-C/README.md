# AgoraPluginRawDataAPI for iOS

*其他语言版本： [简体中文](README.cn.md)*

The Agora-Plugin-Raw-Data-API-iOS-Objective-C Sample App is an open-source demo that will help you how to call Agora Raw Data API and Screenshot API of Objective C.

With this sample app, you can:

- Receive local video data, and other remote video data
- Receive local audio data, and other remote audio data
- Local video and remote video screenshot

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "KeyCenter.m" with your App ID.

```
+ (NSString *)AppId {
    return <#Your App Id#>;
}
```

Next, download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the **AgoraRtcKit.framework** of "libs" folder to **"../libs"** folder.

Finally, Open Agora-Plugin-Raw-Data-API-iOS-Objective-C.xcodeproj, connect your iPhone／iPad device, setup your development signing and run.

## Developer Environment Requirements
* XCode 8.0 +
* Real devices (iPhone or iPad)
* iOS simulator is NOT supported

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).
