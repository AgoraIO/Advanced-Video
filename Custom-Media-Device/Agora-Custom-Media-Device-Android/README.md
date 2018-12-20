# Agora-Custom-Media-Device-Android

*Other languages: [简体中文](README.zh.md)*

This open source sample project shows how to use the new MeidaIO interface provided in Agora RTC SDK(>= 2.1).

In this sample project, the following features are implemented using the new MediaIO API:

1. Enable to sharing mobile phone screen or share a specific View to the remote

2, Use both local video and Camera as the data source, source can be local played and sent to the remote, and the source can be switched.

A tutorial demo for Agora Video SDK can be found here: [Agora-Android-Tutorial-1to1](https://github.com/AgoraIO/Basic-Video-Call/tree/master/One-to-One-Video/Agora-Android-Tutorial-1to1)

You can find share screen demo for Android with old APIs here: [Agora-Screen-Sharing-Android](https://github.com/AgoraIO/Advanced-Video/tree/master/Screensharing/Agora-Screen-Sharing-Android)

### Create an Account and Obtain an App ID
In order to build and run the sample application you must obtain an App ID: 

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Locate the file **app/src/main/res/values/strings_config.xml** and replace <#YOUR APP ID#> with the App ID in the dashboard.

```xml
<string name="agora_app_id"><#YOUR APP ID#></string>
```


### Integrate the Agora Video SDK into the sample project
The SDK must be integrated into the sample project before it can opened and built. There are two methods for integrating the Agora Video SDK into the sample project. The first method uses JCenter to automatically integrate the SDK files. The second method requires you to manually copy the SDK files to the project.

#### Method 1 - Integrate the SDK Automatically Using JCenter (Recommended)

1. Clone this repository.
2. Open **app/build.gradle** and add the following line to the `dependencies` list:

```
...
dependencies {
    ...
    compile 'io.agora.rtc:full-sdk:2.2.3' 
}
```

#### Method 2 - Manually copy the SDK files

1. Clone this repository.
2. Download the Agora Video SDK from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads).
3. Unzip the downloaded SDK package.
4. Copy the .jar file from the **libs** folder of the downloaded SDK package to the **/apps/libs** folder of the sample application.
5. Copy the .so files from the **armeabi-v7a** folder of the downloaded SDK package to the **/app/src/main/jniLibs/armeabi-v7a** folder of the sample application.
6. *(Optional)* Copy the .so files from the **arm64-v8a** folder of the downloaded SDK package to the **/app/src/main/jniLibs/arm64-v8a** folder of the sample application.
7. *(Optional)* Copy the .so files from the **x86** folder of the downloaded SDK package to the **/app/src/main/jniLibs/x86** folder of the sample application.

**Finally**, open project with Android Studio, connect your Android device, build and run.

Or use `Gradle` to build and run.

## Developer Environment Requirements
- Android Studio 2.0 or above
- Real devices (Nexus 5X or other devices)
- Some simulators are function missing or have performance issue, so real device is the best choice

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).
