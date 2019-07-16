# Open Live macOS for Swift

*Read this in other languages: [English](README.en.md)*

这个开源示例项目演示了如何快速集成Agora视频SDK，实现多人视频连麦直播。

在这个示例项目中包含了以下功能：

- 加入通话和离开通话；
- 主播和观众模式切换；
- 静音和解除静音；
- 选择分辨率、码率和帧率；

你也可以在这里查看入门版的示例项目：[Agora-macOS-Tutorial-Swift-1to1](https://github.com/AgoraIO/Agora-macOS-Tutorial-Swift-1to1)

Agora视频SDK支持 iOS / Android / Windows / macOS 等多个平台，你可以查看对应各平台的示例项目：

- [OpenLive-iOS](https://github.com/AgoraIO/OpenLive-iOS)
- [OpenLive-Android](https://github.com/AgoraIO/OpenLive-Android)
- [OpenLive-Windows](https://github.com/AgoraIO/OpenLive-Windows)

## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 KeyCenter.swift

```
static let AppId: String = "Your App ID"
```

然后在 [Agora.io SDK](https://www.agora.io/cn/blog/download/) 下载 **视频通话 + 直播 SDK**，解压后将其中的 **libs/AgoraRtcEngineKit.framework** 复制到本项目的 “OpenLive” 文件夹下。

最后使用 XCode 打开 OpenLive.xcodeproj，设置有效的开发者签名后即可运行。

## 运行环境
* XCode 8.0 +

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的bug，欢迎提交 [issue](https://github.com/AgoraIO/Agora-macOS-Tutorial-Swift-1to1/issues)

## 代码许可

The MIT License (MIT).mac