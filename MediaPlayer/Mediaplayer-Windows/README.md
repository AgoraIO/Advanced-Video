# Mediaplayer-Windows

*Other Language: [简体中文](README.zh.md)*

This MediaPlayerKitQuikStart describes how to share video with others using MediaPlayerKit components.

## Run the Project
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "MediaPlayerKitQuikStart.cpp" with your App ID.


```
context.appId = APP_ID;
```
Next, download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the **sdk** under the same directory .

and download  [MediaPlayerKit](https://download.agora.io/sdk/release/Agora_MediaPlayerKit_for_Windows(x86)_v1_0_0.zip) unzip the zip file ,copy the **MediaPlayerKit**  file  the "libs" under the same directory .

Finally, Open MediaPlayerKitQuikstart.sln,run it and open the video which you want to share.


## Notes
1. MediaPlayerKit/dll Put the *.dll  in the execution path

2. sdk/dll Put the *.dll  in the execution path

3. If the QT header file is not found, please note that vs. need to re-import the .pro file. Regenerate the QT related items. and then run the MediaPlayerKitQuikstart .sln. 



## Usage

1. enter the channel id and join channel；
2. Open the "Open Media" btn and select one video；
3. Run the other project and join the same channel；

## Developer Environment Requirements

* Visual studio 2015 
* Qt5.9.0 +

## Connect US

- You can find full API document at [Document Center](https://docs.agora.io/en/Interactive%20Broadcast/mediaplayer_win?platform=Windows#quickly-experience-mediaplayer-kit)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## Code license

The MIT License (MIT).
