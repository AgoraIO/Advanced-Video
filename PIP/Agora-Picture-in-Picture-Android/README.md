# Agora Picture in Picture

*其他语言版本： [简体中文](README.zh.md)*

The Agora Picture in Picture sample app is an open-source demo that will help you get Agora video app integrated with PiP mode in Android 8.0.

With this sample app, you can:

- Join / leave channel
- Enter / leave Picture in Picture mode

More information related to Picture in Picture mode can be found here: [Picture in Picture](https://developer.android.com/guide/topics/ui/picture-in-picture.html)

## Running the App
**First**, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "app/src/main/res/values/strings.xml" with your App ID.

```
<string name="agora_app_id"><#YOUR APP ID#></string>
```

**Next**, integrate the Agora RTC SDK and there are two ways to integrate:

- The recommended way to integrate:

Add the address which can integrate the Agora RTC SDK automatically through JCenter in the property of the dependence of the "app/build.gradle":
```
compile 'io.agora.rtc:full-sdk:2.0.0'
```
(This sample program has added this address and do not need to add again. Adding the link address is the most important step if you want to integrate the Agora RTC SDK in your own application.)

- Alternative way to integrate:

First, download the Agora RTC SDK from [Agora.io SDK](https://www.agora.io/en/download/). Unzip the downloaded SDK package and copy ***.jar** under **libs** to **app/libs**, **arm64-v8a**/**x86**/**armeabi-v7a** under **libs** to **app/src/main/jniLibs**.

Then, add the fllowing code in the property of the dependence of the "app/build.gradle":

```
compile fileTree(dir: 'libs', include: ['*.jar'])
```

**Finally**, open project with Android Studio, connect your Android device, build and run.

Or use `Gradle` to build and run.

## Developer Environment Requirements
- Android Studio 3.0 +
- Real devices (Nexus 5X or other devices with Android 8.0 +)

## Screenshot
![pip sample with agora sdk](screenshots/pip-sample-agora.png)

## Connect Us
- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License
The MIT License (MIT).
