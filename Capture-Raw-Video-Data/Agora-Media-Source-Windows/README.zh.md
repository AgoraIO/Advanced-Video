# Agora Media Source Windows

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成 Agora 视频 SDK，实现多人视频通话。

在这个示例项目中包含了以下功能：

- 加入通话和离开通话；
- 静音和解除静音；
- 关闭摄像头和打开摄像头；
- 选择摄像头；
- 发送频道内消息；
- 选择分辨率、码率和帧率；
- 设置加密模式；
- 开启和关闭黑白滤镜；
- 开启音视频流自采集及音频流自渲染

本开源项目使用 **C++** 语言

你也可以在这里查看入门版的示例项目：[Agora-Windows-Tutorial-1to1](https://github.com/AgoraIO/Basic-Video-Call/tree/master/One-to-One-Video/Agora-Windows-Tutorial-1to1)


## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 内容替换至 APP_ID 宏定义中

```
#define APP_ID _T("Your App ID")
```

然后在 [Agora.io SDK](https://docs.agora.io/cn/Agora%20Platform/downloads) 下载 **视频通话 + 直播 SDK**，解压后将其中的 **sdk** 复制到本项目的 "AgoraMediaSource” 文件夹下。

最后使用 VC++2013 打开 AgoraMediaSource.sln，编译整个解决方案

Note:
  1. 程序编译后，在运行程序时如若出现：无法启动程序 "xxx\xxx\xxx\Debug\Language\English.dll" 的错误提示，
      请在解决方案资源管理器中选中 AgoraMediaSource 项目，并右击，在弹出的菜单栏中选择 "设为启动项目"，即可解决。之后重新运行程序即可。
  
  2. sdk/dll 文件下的 dll 库需要放在相应的执行路径下。
  
  温馨提示：此案例教程中已经为您配置好相关的 dll 库，若用户想自己使用 agora 提供的接口进行相关开发，则需按上述提示将 dll 库放入相应的执行路径下。

## 运行环境
* VC++2013 或更高版本
* WIN7 或更高版本

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方 Q 群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 注意

对于 master 分支：

视频裸数据回调的使用，需要 IAgoraRtcEngine2.h。

## 代码许可

The MIT License (MIT).
