# Agora-MutilProcess-Education-ShareScreen

*English | [中文](README.zh.md)*

This tutorial shows you how to streaming camera video and screen sharing simultaneously using the Agora sample app.

The small demo contains following features:

- Join/Leave Channel     
- Start/Stop ScreenSharing
- Two Process

## Developer Environment Requirements
* VC2013 or higher
* WIN7 or higher

## Run the sample program
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. define the APP_ID with your App ID.

     #define APP_ID _T("Your App ID")

Next, download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the **sdk** to the "Agora-Screen-Sharing-Windows" folder in project(the old one may be over written).

Finally, Open OpenLive.sln, build the solution and run.

**Note：**

  1. After the program is compiled, if the program "xxx\xxx\xxx\Debug\Language\English.dll" cannot be started when running the program, 
      please select the OpenLive project in the Solution Explorer and right click. In the pop-up menu bar, select "Set as active project" to solve. Then run the program again.
  
  2. The dll library under the sdk/dll file needs to be placed in the corresponding execution path.
  
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
