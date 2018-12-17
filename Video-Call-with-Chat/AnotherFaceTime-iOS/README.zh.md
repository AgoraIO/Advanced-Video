# Another FaceTime iOS

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成 Agora 视频 SDK 和信令 SDK，实现1对1视频通话。

在这个示例项目中包含了以下功能：

- 登录信令；
- 拨号和呼叫；
- 接听和挂断；
- 静音和解除静音；
- 切换前置摄像头和后置摄像头；

## 运行示例程序

1. 首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 *KeyCenter.swift*。

		static let appId: String = <#Your App Id#>

2. 然后下载 [视频通话/视频直播 SDK](https://docs.agora.io/cn/Video/downloads)，解压后将其中的 `AgoraRtcEngineKit.framework` 复制到本项目的 `AgoraFacetime` 文件夹下。

3. 下载 [信令SDK](https://docs.agora.io/cn/Signaling/downloads)，解压后将其中的 `AgoraSigKit.framework` 复制到本项目的 `AgoraFacetime` 文件夹下。

4. 最后使用 Xcode 打开 AgoraFacetime.xcodeproj，连接 iPhone／iPad 测试设备，设置有效的开发者签名后即可运行。

## 运行环境
* XCode 10.0 +
* iOS 真机设备
* 不支持模拟器

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT).
