# Agora Media Source Windows

*其他语言版本： [简体中文](README.zh.md)*

The Agora Media Source Windows Sample App is an open-source demo that how to use IVideoFrameObserver::onCaptureVideoFrame

With this sample app, you can:

- Join / leave channel
- Mute / unmute audio
- Enable / disable video
- Change camera
- Send message to channel
- Setup resolution, frame rate and bit rate
- Enable encryption
- Enable / disable black and white filter
- Enable / disable external video & audio stream capturing and audio stream playing

This demo is written in **C++**

A tutorial demo can be found here: [Agora-Windows-Tutorial-1to1](https://github.com/AgoraIO/Basic-Video-Call/tree/master/One-to-One-Video/Agora-Windows-Tutorial-1to1)


##Prepare Dshow and libyuv

Download [dependencies](https://github.com/AgoraIO/Advanced-Audio/releases/download/DShow/ThirdParty.zip) .
Unzip Thirdparty.zip, copy Dshow and libyuv folders to solution directory.

## Developer Environment Requirements
* VS2017 or higher
* Win8.1 or higher


## Run the sample program
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. define the APP_ID with your App ID.

     #define APP_ID _T("Your App ID")

Next, download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the **sdk** to the "AgoraMediaSource" folder in project(the old one may be over written).

Finally, Open AgoraMediaSource.sln, build the solution and run.

**Note：**

  1. After the program is compiled, if the program "xxx\xxx\xxx\Debug\Language\English.dll" cannot be started when running the program, 
      please select the AgoraMediaSource project in the Solution Explorer and right click. In the pop-up menu bar, select "Set as active project" to solve. Then run the program again.
  
  2. The dll library under the sdk/dll file needs to be placed in the corresponding execution path.
  
**Tips:**

The relevant dll library has been configured for you in this case tutorial. If you want to use the interface provided by agora for related development, you need to put the dll library into the corresponding execution path as prompted above.

## Contact us
- For potential issues, you may take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Would like to see how Agora SDK is used in more complicated real use case? Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase)
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API document at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Developer Forum](hhttps://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).
