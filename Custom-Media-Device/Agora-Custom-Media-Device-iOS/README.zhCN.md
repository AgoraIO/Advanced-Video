# Agora Custom Media Device

*Read this in other languages: [English](README.md)*

## 简介

本开源示例项目基于 Agora Native SDK 中新增的 MediaIO 接口开发，演示了如何使用 Agora 视频 SDK，实现自定义视频采集和渲染。

本示例项目中包含以下功能：

* 使用自定义采集协议，将视频数据发送到直播频道中；
* 使用自定义渲染协议，渲染频道内连麦端的视频数据。

## 准备开发环境

* 请保证你的 Xcode 版本在 Xcode 9.0 及以上
* iOS 真机设备
* 由于部分模拟机有功能缺失或性能问题，Agora 推荐使用真机

## 运行示例程序

1. 在 [Agora.io](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 App ID。
2. 下载本页示例程序。
3. 在示例程序的 *KeyCenter.swift* 文件中，填入获得的 App ID：

   ```
   static let AppId: String = <#Your App Id#>
   ```

4. 点击 [Agora.io SDK](https://docs.agora.io/cn/Agora%20Platform/downloads) 下载 **视频通话 + 直播 SDK**，然后解压。
   > 自定义采集和自定义渲染协议从 2.1.0 版本开始提供，因此请下载 v2.1.0 及之后版本的 SDK。
5. 将 SDK 中 **libs** 文件夹下的 **AgoraRtcEngineKit.framework** 复制到本示例项目的 **Agora-Custom-Media-Device** 文件夹下。
6. 使用 Xcode 打开 **Agora-Custom-Media-Device.xcodeproj**，连接 iPhone／iPad 测试设备，设置有效的开发者签名后即可运行。

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方 QQ 群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 Bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT).
