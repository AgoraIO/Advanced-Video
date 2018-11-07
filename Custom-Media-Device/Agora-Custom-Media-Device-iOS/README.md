# Agora Custom Media Device

*其他语言版本： [简体中文](README.zhCN.md)*

## Introduction

The Agora-Custom-Media-Device Sample App is an open-source demo that helps you get live video chat integrated into your applications with custom media devices.

With this Sample App, you can:

* Send captured image from custom video source to live video channel
* Render frames of live video channel with custom video renderer

## Preparing the Developer Environment

* Ensure that your Xcode version is later than Xcode 9.0 (included)
* Prepare iOS devices with audio and video functionalities
* iOS simulators are NOT supported

## Running the App

1. Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. 
2. Download the Sample App.
3. Fill in the App ID in the *KeyCenter.swift* file of the Sample App:
   ```
   static let AppId: String = <#Your App Id#>
   ```
4. Download the **Agora Video SDK** from [Agora.io SDK](https://www.agora.io/en/blog/download/). Unzip the downloaded SDK package.
   > Custom media device protocols are provided from 2.1.0.
5. Copy the **AgoraRtcEngineKit.framework** file under the **libs** folder of the SDK to the **Agora-Custom-Media-Device** folder in project.
6. Open **Agora-Custom-Media-Device.xcodeproj**, connect your iPhone／iPad device, set up your development signing and run.

## Connect Us

* You can find full API documents at [Document Center](https://docs.agora.io/en/)
* You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).
