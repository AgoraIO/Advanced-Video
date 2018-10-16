# ARD-Agora-RTC-Hooking-API-Windows

The demo mainly explains how to use HOOK to do double-window operation on window pc

With this sample Code you can:

- join/leave channel

- show local /remote video stream

- Listen to /play remote audio stream

- switch Native video capture divices

- switch Native audio capture devices

- switch local audio player devices

- select local different audio player hook

##Running Environment

First of all ,you need to download the Dll of the Hook Module and place the relevant folder on the HookSDK, then download the media sdk ,put the sdk file on the AgoraSDK,and finally define the configuration file AgroaHook.ini

[BaseInfo]

AppId=

AppCertEnable=0

AppCertificateId=

ChannelName=baluoteliz

[AgoraHookInstance]

LoginUid=111


Alsl a conguration file DebugMode.ini. this is related to Hook Infomation positoning ,the default is automatically generated as follows

[DebugMode]

DebugMode=1

SaveDumpPcm=0


HookSDK will produce a log file PlayerHookV6_1.log in the ./AgoraHookLog directory during runtime. the current sdamplecode specifies that mediaSDKLog exists in the ./logger directory

##Development Environment

- visual studio 2013

- Window 7 or Higher








     


