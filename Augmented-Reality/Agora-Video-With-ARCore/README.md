# Agora Video with ARCore

*Read this in other languages: [中文](README.zh.md)*

This tutorial shows you how to create an Android app with live video chat using Google's ARCore platform and the Agora Video SDK.

With this sample app, you can:

- Send a captured image from ARCore to the live video channel.
- Render video frames of a remote user in an ARCore session.

## Prerequisites

* Android Studio 3.0.1 or above.
* Android SDK Platform version 7.0 (API level 24) or above.
* A physical [Android device with ARCore support](https://developers.google.com/ar/discover/), for example, a Nexus 5X. A real device is recommended because some simulators have missing functionality or lack the performance necessary to run the sample.

## Quick Start
This section shows you how to prepare, build, and run the sample application.

### Create an Account and Obtain an App ID
To build and run the sample application, first obtain an app ID: 

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the sign-up process, you are redirected to the dashboard.
2. Navigate in the dashboard tree on the left to **Projects** > **Project List**.
3. Locate the file `app/src/main/res/values/strings.xml` and replace <#YOUR APP ID#> with the app ID in the dashboard.

```xml
<string name="private_broadcasting_app_id"><#YOUR APP ID#></string>
```

### Integrate the Agora Video SDK into the Sample Project
To use the Agora Video SDK, you must first integrate the Agora Video SDK into the sample project. 

There are two methods for integrating the Agora Video SDK into the sample project:
- Method 1 uses JCenter to automatically integrate the SDK files.
- In Method 2, you manually copy the SDK files to the project.

**Note:** Custom media device protocols are available in the Agora Video SDK 2.1.0 and above.

#### Method 1. Integrate the SDK Automatically Using JCenter (Recommended)

1. Clone the repository.
2. Open **app/build.gradle** and add the following line to the `dependencies` list:

```
...
dependencies {
    ...
    compile 'io.agora.rtc:full-sdk:2.1.0' 
}
```

#### Method 2. Manually Copy the SDK Files

1. Clone the repository.
2. Download the Agora Video SDK from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads).
3. Unzip the downloaded SDK package.
4. Copy the `agora-rtc-sdk.jar` file from the **libs** folder of the downloaded SDK package to the **/apps/libs** folder of the sample application.
5. Copy the `.so` files from the **armeabi-v7a** folder of the downloaded SDK package to the **/app/src/main/jniLibs/armeabi-v7a** folder of the sample application.


### Using the Sample Application

This sample application works in conjunction with the [OpenLive](https://github.com/AgoraIO/OpenLive-Android) sample application. Note that it requires **two** devices.

1. Run the sample application in Android Studio. Move the first device until you find a horizontal surface.
 
2. Touch the plane indicator to add a virtual display screen to your AR session. The virtual display screen streams the video from the remote user.

3. On the second device, launch the [OpenLive](https://github.com/AgoraIO/OpenLive-Android) sample application using the same app ID used in this project, and join the channel `arcore` as a broadcaster.

	The virtual display screen from step 2 displays the video broadcast sent from the remote user's [OpenLive](https://github.com/AgoraIO/OpenLive-Android) application.

## Resources
* A detailed code walkthrough for this sample is available in [Steps to Create this Sample](./guide.md).
* Find full [API documentation at the Document Center](https://docs.agora.io/en/).
* [File bugs about this sample](https://github.com/AgoraIO/Advanced-Video/issues).


## License
This software is under the MIT License (MIT). [View the license](LICENSE.md).
