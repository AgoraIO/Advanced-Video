# Agora Video with ARKit

*Read this in other languages: [中文](README.zh.md)*

This quick start enables you to integrate live video chat functionality from the Agora Video SDK using an iOS ARKit sample application.

This sample application demonstrates the following basic Agora SDK functionality:

- Sending a captured ARFrame image to a live video channel
- Rendering remote user ARSession video frames as SCNNodes


## Prerequisites
- Agora.io developer account
- Xcode 9.0+
- Physical iPhone or iPad device (iOS simulator is not supported)

## Quick Start
This section shows you how to prepare and build the Agora Video with the ARKit sample app.

### Create an Account and Obtain an App ID
To build and run the sample application, first obtain an app ID: 

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the sign-up process, you are redirected to the dashboard.
2. Navigate in the dashboard tree on the left to **Projects** > **Project List**.
3. Copy the app ID that you obtained from the dashboard into a text file. You will use this when you launch the app.

### Update and Run the Sample Application 

1. Open `Agora-Video-With-ARKit.xcodeproj` and edit the `ViewController.swift` file. In the `viewDidLoad` function, update `<#Your AppId#>` with your app ID.

   ``` Swift
   agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: <#Your App Id#>, delegate: self)
   ```

2. Download the [Agora Video SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the `libs/AograRtcEngineKit.framework` file into the `Agora-Video-With-ARKit` folder.

	**Note:** Custom media device protocols are available in the Agora SDK 2.1.0.
		
3. Connect your iPhone／iPad device and run the project. Ensure a valid provisioning profile is applied or your project will not run.

### Using the Sample Application

This sample application requires two devices and works in conjunction with the [OpenLive](https://github.com/AgoraIO/OpenLive-iOS) sample application. 

1. Run the sample application. The device will display a flashing red plane indicator. Move the device until you find a horizontal surface.
 
2. Touch the plane indicator to add a virtual display screen to your AR session. The virtual display screen streams the video from the remote user.

3. On a different device, launch the [OpenLive](https://github.com/AgoraIO/OpenLive-iOS) sample application using the app ID and join the channel `agoraar` as a broadcaster. The virtual display screen from the previous step displays the video broadcast sent from this [OpenLive](https://github.com/AgoraIO/OpenLive-iOS) application.

## Resources
* A detailed code walkthrough for this sample is available in [Steps to Create this Sample](./guide.md).
* Complete API documentation is available at the [Document Center](https://docs.agora.io/en/).

## License
This software is licensed under the MIT License (MIT). [View the license](LICENSE.md).