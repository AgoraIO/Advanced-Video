# Android Samples for Advanced Applications of Agora Video SDK APIs

*English | [中文](README.zh.md)*

## Modules

### Custom Video Sources

By default, Agora SDK starts camera preview and handles all the video processing. Instead, it also provides MediaIO interfaces that consume custom video sources. It requires developers to maintain the whole capture/rendering life cycles themselves.

Two kinds of custom video source (screenshare and media file) are implemented as examples of how to input frames to Agora SDK pipelines. Developers are free to choose any ways that best fit their scenarios.

Refer to [this page](https://docs.agora.io/en/Video/custom_video_android?platform=Android) for the details of using MediaIO interfaces.

Video frames are usually in the formats of I420, NV21 or texture, and in some circumstances, it is necessary for developers to create their own OpenGL threads. The idea of maintaining OpenGL contexts in this sample comes from [grafika](https://github.com/google/grafika) project. Refer to the page for further information if you are not familiar or interested to know more.

In the following paragraphs and the source code, custom video sources are defined as `external`,  while we consider the video captured by SDK video module as the `internal` source. One external video source may come from different types of `Input`,  like screen share or .mp4 files as mentioned above.

#### Screen Share

Screen Sharing is one of the features introduced in Android 5.0, the project minimum SDK level should not be less than 21.

In most cases, screen share applications are required to stay running in the background. To prevent from being killed unexpectedly by the system, MediaProjection is created and started in a foreground service. Remember to send a system notification for the foreground service, particularly on Android Oreo and above.

#### Local Videos

This project **does not** provide a .mp4 file by default, users need to provide one and copy to application private folder as `storage/emulated/0/Android/data/io.agora.advancedvideo.switchvideoinput/files/Movies/localvideo.mp4`. 

Video frame data (stored in ByteBuffer) is extracted from .mp4 files using [MediaExtractor](https://developer.android.com/reference/android/media/MediaExtractor), then the frame data is sent to a decoder [MediaCodec](https://developer.android.com/reference/android/media/MediaCodec) at the pace of the frame's presentation time.

The decoded frames are rendered to an output Surface which is created from a pre-configured texture, then the texture can be used to draw local preview and be fed to SDK.


#### <h2 id="switching"> Switching between different video inputs </h2>

Only one video input instance is created in charge of generating video frames, and the frames from one input can be switched to another at any time and SDK is not aware.

Video frames are sent in a service. The video input continues to send video even if the app goes to background until stopped. 

An OpenGL worker thread is managed by the service. All video inputs have the same output Surface, and frame data is obtained from the Surface periodically.

### Audio & Video Customized Packet Encryption Using Raw-data Plugin Interface

Go to [Raw Audio Data](https://docs.agora.io/en/Video/raw_data_audio_android?platform=Android) and [Raw Video Data](https://docs.agora.io/en/Video/raw_data_video_android?platform=Android) for the descriptions of using audio & video raw data. 

Packet encryption is one of the common use cases of audio & video raw data interface. Developers can choose to perform any transformation of raw data on the sending side, and do the inverse to recover data on the receiving side.

Note that this project only provides C++ raw data interface. There are also Java layer interfaces before, but the video interfaces have been removed from earlier SDK releases for performance concerns. The Java audio interfaces are still usable, but C++ interfaces are recommended for the same reason.

The compiled .so library's name should start with a pre-defined prefix like **"libapm-xxx.so"**, RTC engine will search in the library directory .so files of names of this form as Agora plugins and load the libraries during the plugin manager initialization phase.

Plugins use header files which exist in the downloaded SDK zip archives but not a part of JCenter (or maven) dependency. The headers are submitted along with the project right now, but there are chances that developers need to upgrade the header files for future SDK versions.


### Push Interfaces

Besides MediaIO interface, push interface is also an Agora SDK feature that helps consume external video sources. For more details, please refer to [Push Interface](https://docs.agora.io/en/Interactive%20Broadcast/custom_video_android?platform=Android#customize-video-source).

In this demo, we focus on how push interface is used and ignore details like background running and threading. If you are not certain how to implement, refer to the [external switching demo](#switching).

## Prerequisites

- Android Studio 3.3 or above
- Android device (e.g. Nexus 5X). A real device is recommended because some simulators have missing functionality or lack the performance necessary to run the sample.

## Quick Start

This section shows you how to prepare, build, and run the sample application.

### Obtain an App ID

To build and run the sample application, get an App ID:
1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App ID** from the Dashboard for later use.
4. Generate a temp **Access Token** (valid for 24 hours) from dashboard page with given channel name, save for later use.

5. Locate the file **lib-component/src/main/res/values/strings_config.xml** and replace <#YOUR APP ID#> with the App ID in the dashboard.

  ```xml
  <string name="agora_app_id"><#YOUR APP ID#></string>
  <!-- Obtain a temp Access Token at https://dashboard.agora.io -->
  <!-- You will need to deploy your own token server for production release -->
  <!-- Leave this value empty if Security keys/Token is not enabled for your project -->
  <string name="agora_access_token"><#YOUR TOKEN#></string>
  ```

### Integrate the Agora Video SDK

The SDK must be integrated into the sample project before it can opened and built. There are two methods for integrating the Agora Video SDK into the sample project. The first method uses JCenter to automatically integrate the SDK files. The second method requires you to manually copy the SDK files to the project.

#### Method 1 - Integrate the SDK Automatically Using JCenter (Recommended)

1. Clone this repository.
2. Open **lib-component/build.gradle** and add the following line to the `dependencies` list:

  ```
  ...
  dependencies {
      ...
      implementation 'io.agora.rtc:full-sdk:3.0.0'
  }
  ```

#### Method 2 - Manually copy the SDK files

1. Download the Agora Video SDK from [Agora.io SDK](https://www.agora.io/en/download/).
2. Unzip the downloaded SDK package.
3. Copy the following files from from the **libs** folder of the downloaded SDK package:

Copy from SDK|Copy to Project Folder
---|---
.jar file|**/lib-component/libs** folder
**arm64-v8a** folder|**/lib-component/src/main/jniLibs** folder
**x86** folder|**/lib-component/src/main/jniLibs** folder
**armeabi-v7a** folder|**/lib-component/src/main/jniLibs** folder

    

### Run the Application

Open project with Android Studio, connect your Android device, build and run.
      
Or use `Gradle` to build and run.


## Contract Us

- For potential issues, take a look at our [FAQ](https://docs.agora.io/cn/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Advanced-Video/issues)


## Licence
The MIT License (MIT).
