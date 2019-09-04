# Mediaplayer-Mac
*Other Language: [简体中文](README.zh.md)*

This MediaPlayerKitQuikStart describes how to share video with others using MediaPlayerKit components.

## Run the Project
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "KeyCenter.swift" with your App ID.

```
static let AppId: String = "YOUR_APPID";
```

Next, download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the **libs/AgoraRtcEngineKit.framework** to the "libs" folder .

and download  [MediaPlayerKit]( https://download.agora.io/sdk/release/Agora_MediaPlayerKit_for_macOS_v1_0_0.zip) unzip the zip file ,copy the MediaPlayerKit.framework to the "libs" folder.

Finally, Open MediaPlayerKitQuikstart.xcodeproj ,run it and open the video which you want to share.


## Usage

1. enter the channel id and join channel；
2. Open the "Open Media" btn and select one video；
3. Run the other project and join the same channel；

## Developer Environment Requirements
* Xcode 10.0 +
* Mac OS 10.12 +

## Connect US

- You can find full API document at [Document Center](https://docs.agora.io/en/Interactive%20Broadcast/mediaplayer_mac?platform=macOS#quickly-experience-mediaplayer-kit)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT).
