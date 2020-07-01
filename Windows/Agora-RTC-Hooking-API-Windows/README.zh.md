# Agora RTC Hooking API Windows

这个demo主要用来截取音乐播放器的声音，输入声网进行传输。

示例具有以下功能:

- 加入/离开频道

- 播放远端音频流

- 切换本地音频采集设备 

- 切换本地音频播放设备

- 选择不同的音乐播放器hook

## 开发环境

- visual studio 2013

- Window 7 or Higher

- Agora Native SDK 2.9.3

## Run the sample program

1. 首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 App ID。然后定义配置文件AgroaHook.ini

        [BaseInfo]
        AppId=Your App ID
        AppCertEnable=0
        AppCertificateId=
        ChannelName=Your Specified Channel Name

        [AgoraHookInstance]
        LoginUid=111


2. 然后在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 **视频通话 + 直播 SDK**，.解压后将其中的 **sdk** 复制到本项目目录下（并覆盖原有旧目录）"Agora-RTC-Hooking-API-Windows"文件夹。

3. 然后， 需要下载Hook sdk，并把相关文件拷贝到 HookSDK目录下面

4. 最后使用 VC++2013 打开 AgoraHooking.sln，编译整个解决方案即可运行

**注意:**

  1. 程序编译后，在运行程序时如若出现：无法启动程序"xxx\xxx\xxx\Debug\Language\English.dll"的错误提示，
      请在解决方案资源管理器中选中AgoraHooking项目，并右击，在弹出的菜单栏中选择 "设为启动项目"，即可解决。之后重新运行程序即可。
  2. sdk/dll 文件下的 dll 库需要放在相应的执行路径下。

## 联系我们

- 如果你遇到了困难，可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)


## License

The MIT License (MIT).











     


