# Another FaceTime iOS

*其他语言版本： [简体中文](README.zh.md)*

This readme describes the steps and considerations for demonstrating the Agora Anoter FaceTime sample app.

## Introduction

Built upon the Agora Video SDK and the Agora Signaling SDK, the Agora Anoter FaceTime is an open-source demo that integrates video chat into your applications.

This sample app allows you to:

- Login the signaling server
- Make a call
- Accept or hang up a call
- Mute/unmute a user
- Switch the camera

## Preparing the Developer Environment

* Xcode 10.0 or later
* An iPhone or iPad

NOTE: The iOS Simulator is NOT supported.

## Running the App
1. Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), obtain an App ID, and enable the App Certificate. 
2. Fill in the AppID in the *KeyCenter.swift*.

		static let appId: String = <#Your App Id#>

3. Unzip the downloaded [Agora Video SDK](https://docs.agora.io/en/Video/downloads) and copy `AgoraRtcEngineKit.framework` to the *AgoraFacetime* folder of the project.
4. Unzip the downloaded [Agora Signaling SDK](https://docs.agora.io/en/Signaling/downloads) and copy `AgoraSigKit.framework` to the *AgoraFacetime* folder of the project.
5. Open AgoraFacetime.xcodeproj, connect your iPhone／iPad device, set up your code signature, and run the sample app.

## Contact Us

- You can find the API documentation at [Developer Center](https://docs.agora.io/en/).
- You can report issues about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues).

## License

The MIT License (MIT). 
