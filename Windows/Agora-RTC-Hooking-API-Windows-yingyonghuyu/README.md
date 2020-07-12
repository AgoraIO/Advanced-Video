# Agora RTC Hooking API Windows

The current demo is mainly written for heroes

With this sample Code you can:

- join/leave channel

- Listen to /play remote audio stream

- switch Native audio capture devices

- switch local audio player devices

- select local different audio player hook

## Developer Environment Requirements

- visual studio 2013

- Window 7 or Higher

- Agora Native SDK 2.9.3

## Run the sample program
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Then define the configuration file AgroaHook.ini

[BaseInfo]

AppId=Your App ID

AppCertEnable=0

AppCertificateId=

ChannelName=baluoteliz

[AgoraHookInstance]

LoginUid=111


Next, download the **Agora Video SDK** from [Agora.io SDK](https://docs.agora.io/en/Agora%20Platform/downloads). Unzip the downloaded SDK package and copy the files under **libs\include**, to the project folder **Agora-RTC-Hooking-API-Windows-yingyonghuyu\sdk\include** （create if not exist）. Then copy the **dll** file and **lib** file under **libs\x86**, to **sdk\dll** and **sdk\lib** respectively.Then, you need to download Dll of the Hook Module and place the relevant folder on the HookSDK.

Finally, Open AgoraHooking.sln, build the solution and run.

**Note：**

  1. After the program is compiled, if the program "xxx\xxx\xxx\Debug\Language\English.dll" cannot be started when running the program, 
      please select the AgoraHooking project in the Solution Explorer and right click. In the pop-up menu bar, select "Set as active project" to solve. Then run the program again.
  
  2. The dll library under the sdk/dll file needs to be placed in the corresponding execution path.
  
  Tips: The relevant dll library has been configured for you in this case tutorial. If you want to use the interface provided by agora for related development, you need to put the dll library into the corresponding execution path as prompted above.

## Contact us

- For potential issues, take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Advanced-Video/issues)


## License

The MIT License (MIT).











     


