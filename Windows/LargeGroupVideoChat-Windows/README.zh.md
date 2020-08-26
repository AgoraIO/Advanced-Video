# Large Group Video Chat

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成Agora视频SDK实现多人视频连麦（17人）。

在这个示例项目中包含了以下功能：

- 加入通话和离开通话；
- 主播和观众模式切换；
- 静音和解除静音；
- 切换摄像头；
- 选择分辨率、码率和帧率；
- 包括本地用户在内，一共17人；

本开源项目使用 **C++** 语言

## 运行环境
* VC++ 2013(或更高版本)
* Windows 7(或更高版本)


## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 App ID。将 APP_ID宏定义内容改为刚才申请的 App ID

    
    #define APP_ID _T("Your App ID")
    

如果你不想修改代码部分,你可以在在Debug/Release下创建一个AppId.ini文件.修改appId的值为刚才申请的App ID

    [AppID]
    AppID=xxxxxxxxxxxxxxxxxxx


在 [Agora.io SDK](https://www.agora.io/cn/blog/download/) 下载 **视频通话 + 直播 SDK**，解压后将 **libs\include** 中的文件，复制到 **LargeGroupVideoChat-Windows** 下的 **sdk\include** 文件夹下（如无该文件夹需新建）。并将 **libs\x86** 下的 **dll** 和 **lib** 文件分别复制到 **sdk\dll** 和 **sdk\lib** 文件夹下。

最后使用 VC++2013 打开 OpenLive.sln，编译整个解决方案即可运行

**注意:**

  1. 程序编译后，在运行程序时如若出现：无法启动程序"xxx\xxx\xxx\Debug\Language\English.dll"的错误提示，
      请在解决方案资源管理器中选中OpenLive 项目，并右击，在弹出的菜单栏中选择 "设为启动项目"，即可解决。之后重新运行程序即可。
  2. 本开源项目在 debug 模式下运行可能会出现崩溃，请在 release 模式下运行。


## 联系我们

- 如果你遇到了困难，可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)


## 代码许可

The MIT License (MIT).
