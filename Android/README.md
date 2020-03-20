# Android Samples for Advanced Applications of Agora Video SDK APIs

*English | [中文](README.zh.md)*

This demo will give examples of how to use Agora Video SDK combined with Android framework APIs, as well as potential scenarios that applies audio & video modules.

## Configuration

## Modules

### Customized Video Sources and Video Rendering

Agora Video SDK provides MediaIO interfaces which can be used to define videos from sources other than captured by Agora SDK itself. This facilitates developers when they have their own video sources available (from other devices, for example), but it needs controlling of the whole life cycles of video sources.

This project exemplifies two kinds of external video sources (compared to videos captured by Agora SDK) and how to push them into the pipeline of Agora SDK APIs. Besides, it shows the way that you may choose to render videos.

Refer to [this page](https://docs.agora.io/en/Video/custom_video_android?platform=Android) for the explanation of how to use MediaIO sources and rendering interfaces.

The video frames are usually in the forms of I420, NV21 or texture. Here we take textures as an example of how to maintain the OpenGL context appropriately for both video sources and rendering. 

There is another reference project [grafika](https://github.com/google/grafika) from Google where this project's idea of maintaining OpenGLES contexts comes from.

To held distinguish between MediaIO video sources and the videos from different sources, we define the video frames from screen sharing or local videos as inputs. 

#### Screen Share

Screen Sharing is one of the features introduced in API 21, so the minimum level should be equal to or higher than that.

In order to keep the screen sharing running properly even in the Launcher or other Apps, and to insure it is not killed by the system unexpectedly, the media projection is created and started in another foreground Service. Do not forget to show a notification, especially when the target is Android Oreo or later.

#### Local Videos

To keep the code simple, the demo requires the users to copy an .mp4 video file to the app-owned video folder. Developers can implement in any way of finding a video file.

Video frame data (stored in ByteBuffer) is extracted from video tracks of packaged media files (.mp4 files) using [MediaExtractor](https://developer.android.com/reference/android/media/MediaExtractor), then the frame data is sent to a decoder [MediaCodec](https://developer.android.com/reference/android/media/MediaCodec) at the pace of the frame's presentation time.

To keep consistent, results of the decoder are received by an output Surface which is created from a texture, then we can update and draw to any SurfaceView or TextureView using that texture directly (usually as local preview).

#### Augmented Reality
Not implemented yet.


#### Switching between different video inputs

We have provided a sample of switching between different inputs of external videos. Special care about the differences between "video inputs" and "video sources", where "sources" refers to Agora RTC MediaIO interface "IVideoSource" which means the transmission entry of Agora API, while "inputs" are defined as where the video frames are generated (mp4 or screens, etc.). 

Only one video input instance is created in charge of generating video frames, and the frames are obtained from one input at one time but can be switched to another input at any time.

To keep consistent, video frames are sent in a Service. So it continues to send video even if the app goes to background until stopped. An OpenGL worker thread is managed by the Service and the thread is used throughout the application to avoid OpenGL problems. All video inputs will have the same output Surface created in this thread, and frame data is updated from the Surface periodically. The intervals are obtained from the specific video inputs.

### Audio & Video Customized Packet Encryption Using Raw-data Plugin Interface

Go to [Raw Audio Data](https://docs.agora.io/en/Video/raw_data_audio_android?platform=Android) and [Raw Video Data](https://docs.agora.io/en/Video/raw_data_video_android?platform=Android) for the descriptions of using audio & video raw data. 

Packet encryption is one of the specific applications of audio & video raw data interface. Developers can choose to perform any transformation of raw data on the sending side, and do the inverse to recover data on the receiving side.

Note that this project only provides C++ raw data interface. There are also Java layer interfaces before, but the video interfaces have been removed from earlier SDK releases for performance concerns. The Java audio interfaces are still usable, but C++ interfaces are recommended for the same reason.

The App's compiled library name should start with a pre-defined prefix like **"libapm-xxx.so"**, RTC engine will search in the library directory .so files of names of this form as Agora plugins and load the libraries during the plugin manager initialization phase.

Plugins use header files which are in the SDK downloaded zip files but not a part of JCenter dependency. The headers are submitted along with the project right now, but if Agora SDK dependency needs to be upgraded, the headers might as well. Once necessary, we suggest that developers download the latest SDK version and replace those headers. Or rather, download and copy the whole SDK instead of introducing from JCenter.  


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
