# Open Duo for Android

*Other languages: [简体中文](README.zh.md)*

This readme describes the steps and considerations for demonstrating the Agora OpenDuo sample app.

## Introduction

Built upon the Agora Video SDK and the Agora RTM SDK, the Agora OpenDuo for Android is an open-source demo that integrates video chat into your Android applications.

This sample app allows you to:

- Login the RTM server
- Inquire whether the calling subscriber is online
- Call each other
- Accept or hang up a call
- Mute/unmute a user
- Switch the camera

## Developer Environment Requirements

- Android Studio 3.3 or later
- Real Android devices, for example Nexus 5X

NOTE: Agora recommends using a real device instead of an emulator.

## Running the App
1. Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID.

2. Fill in your App ID in the **app/src/main/res/values/strings.xml**.

        <string name="agora_app_id"><#YOUR APP ID#></string>

3. Download the Agora Video SDK and the Agora RTM SDK from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads).

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
