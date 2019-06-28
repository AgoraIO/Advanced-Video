# AgoraPluginRawDataAPI for Android(Java)

*其他语言：[English](https://github.com/AgoraIO/Advanced-Video/blob/master/Capture-Raw-Video-Data/Agora-Plugin-Raw-Data-API-Android-Java/README.zh.md)*

The Agora-Plugin-Raw-Data-API-Android-Java Sample App is an open-source demo that will help you how to call Agora Raw Data API and Screenshot API of Java.

With this sample app, you can:

- Receive local video data, and other remote video data
- Receive local audio data, and other remote audio data
- Local video and remote video screenshot


## Running the App
**First**, create a developer account at [Agora.io](https://dashboard.agora.io/en/signup/) and obtain an App ID. Update "app/src/main/res/values/strings.xml" with your App ID.

```
<string name="agora_app_id"><#YOUR APP ID#></string>
```

**Next**
Download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads);
Unzip the downloaded SDK package;
Copy the **.jar** files under **libs** folder to **app/libs** of this sample project;
Copy **arm64-v8a**/**x86**/**armeabi-v7a**" under **libs** folder to **app/src/main/jniLibs** of this sample project
Copy all files under **include** folder to **raw-data-api-java/src/main/cpp/include** of this sample project. 


**Finally**, open the project in Android Studio, connect your Android device, compile and run.

## To compile aar package separately
- raw-data-api-java is a module, which needs to be added to the main project as the dependency
- add below to the setting.gradle:
`include ':raw-data-api-java'`
- add below to the build.gradle: 
```
	dependencies {
	    implementation project(path: ':raw-data-api-java')
	}
```
- the sample project needs to configure the path of the Android NDK, which should be the same Android SDK path configuration

## Code sample
- Implement the callback methods for classes
```
class implements MediaDataAudioObserver, MediaDataVideoObserver
```
- Initialize the object of MediaDataObserverPlugin
```
MediaDataObserverPlugin mediaDataObserverPlugin = MediaDataObserverPlugin.the();
```
- Set callback            
```
MediaPreProcessing.setCallback(mediaDataObserverPlugin);
```
- Add the observer of video callback                  
```
mediaDataObserverPlugin.addVideoObserver(this);
```
- Add the observer of audio callback                  
```
mediaDataObserverPlugin.addAudioObserver(this);
```
- Remove the observer of video callback                      
```
mediaDataCallbackUtil.removeVideoObserver(this);
```
- Remove the observer of audio callback                      
```
mediaDataCallbackUtil.removeAudioObserver(this);
```
- Take local video snapshot                         
```
mediaDataCallbackUtil.saveCaptureVideoSnapshot(path);
```
- Take remote video snapshot                         
```
mediaDataCallbackUtil.saveRenderVideoSnapshot(path);
```

## Developer Environment Requirements
- Android Studio 3.2 +
- Real devices (Nexus 5X or other Android devices)
- Android simulator is NOT recommended

## Connect Us
- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License
The MIT License (MIT).
