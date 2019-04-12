# Agora-RTC-With-ConnectionService-Android

*其他语言版本： [简体中文](README.zh.md)*

This readme describes the steps and considerations for demonstrating the Agora calling sample app with [ConnectionService](https://developer.android.com/reference/android/telecom/ConnectionService).

## Introduction

Built upon the Agora Video SDK and the Agora Signaling SDK, the Agora-RTC-With-ConnectionService for Android is an open-source demo that integrates video chat into your Android applications. This demo also shows how to use the [ConnectionService](https://developer.android.com/reference/android/telecom/ConnectionService) provided by Android framework to achieve the iOS [CallKit](https://developer.apple.com/documentation/callkit) alike user experience.

This sample app allows you to:

- Login the signaling server
- Inquire whether the calling subscriber is online
- Call each other
- Accept or hang up a call initiated by this app using system incall ui just like a telephone call
- View the app call history in the system phone app call history just like a telephone call
- Mute/unmute a user
- Switch the camera

## Developer Environment Requirements

- Android Studio 3.0 or later
- Android API level >= 23
- Real Android devices with API level >= 23

NOTE: Agora recommends using a real device instead of an emulator. May fail to run on some device not supporting ConnectionService

## Running the App
1. Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID.

   NOTE: For more information, see [Obtaining an App ID](https://docs.agora.io/en/Signaling/key_signaling?platform=All%20Platforms#how-to-get-and-use-an-app-id).

2. Fill in your App ID in the **app/src/main/res/values/strings.xml**.


        <string name="agora_app_id"><#YOUR APP ID#></string>

3. Download the Agora Video SDK and the Agora Signaling SDK from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads).

   NOTE: This project is built upon the **Agora Video SDK 2.4.0** and the **Agora Signaling SDK 1.4.0**.

4. Unzip the downloaded SDK package and:

  - copy ***.jar** under **/libs** to **app/libs**
  - copy **arm64-v8a**/**x86**/**armeabi-v7a** under **libs** to **app/src/main/jniLibs**.

5. Check whether the following code snippet is in the property of the dependency of **app/build.gradle** (included in the sample app):

          implementation fileTree(dir: 'libs', include: ['*.jar'])

6. Use Android Studio to open the project, connect your Android device to your computer, and build and run the sample app.

   NOTE: You can also use `Gradle` to build and run the sample app.

## Contact Us

- You can find the API documentation at [Developer Center](https://docs.agora.io/en/).
- You can report issues about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues).

## License

The MIT License (MIT).
