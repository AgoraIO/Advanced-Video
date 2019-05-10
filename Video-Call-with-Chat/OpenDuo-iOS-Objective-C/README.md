# Agora OpenDuo for iOS (Objective C)

*其他语言版本： [简体中文](README.zh.md)*

This readme describes the steps and considerations for demonstrating the Agora OpenDuo iOS sample app.

## Introduction

Built upon the Agora Video SDK and the Agora RTM SDK, the Agora OpenDuo for iOS is an open-source demo that integrates video chat into your iOS applications.

This sample app allows you to:

- Login the RTM server
- Make a call
- Accept or hang up a call
- Mute/unmute a user
- Switch the camera

## Preparing the Developer Environment

* Xcode 10.0 or later
* An iPhone or iPad

NOTE: The iOS emulator is NOT supported.

## Running the App
1. Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), obtain an App ID, and enable the App Certificate. 
2. Fill in the AppID in the *KeyCenter.m*.

        static NSString * const kAppID = @"Your App ID";

3. Download the **Agora Video SDK** and the **Agora RTM SDK** from [Agora.io](https://docs.agora.io/en/Agora%20Platform/downloads).
4. Unzip the downloaded **Agora Video SDK** and copy **libs/AgoraRtcEngineKit.framework** to the *OpenDuo* folder of your project.
5. Unzip the downloaded **Agora RTM SDK** and copy **libs/AgoraRtmKit.framework** to the *OpenDuo* folder of your project.
6. Open OpenDuo.xcodeproj, connect your iPhone／iPad device, set up your code signature, and run the sample app.

## About the Dynamic Key

For security, Agora recommends enabling the Dynamic Key, see [RTM key](https://docs.agora.io/cn/Real-time-Messaging/RTM_key?platform=All%20Platforms).


## Contact Us

- You can find the API documentation at [Developer Center](https://docs.agora.io/en/).
- You can report issues about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues).

## License

The MIT License (MIT). 

