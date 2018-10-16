# Agora RTC With CallKit

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何结合 Agora视频SDK 和 CallKit，实现 视频电话功能。

在这个示例项目中包含了以下功能：

- 模拟发起一个视频通话；
- 模拟接听一个视频通话；
- 通话中静音和解除静音。

## 运行示例程序
首先在 [Agora.io](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 ViewController.swift 对应位置。

然后在 [Agora.io SDK](https://www.agora.io/cn/blog/download/) 下载 **视频通话 + 直播 SDK**，解压后将其中的 **libs/AgoraRtcEngineKit.framework** 复制到本项目的 "AgoraRTCWithCallKit" 文件夹下。

最后使用 Xcode 打开 AgoraRTCWithCallKit.xcodeproj，连接 iPhone／iPad 测试设备，设置有效的开发者签名后即可运行。

##使用方法
1. 在输入框输入模拟的号码，键盘的完成键可以收起键盘；
2. 点击屏幕下方的模拟呼叫按钮，将模拟一个呼出的视频通话；
3. 点击屏幕下方的模拟来电按钮，将模拟一个视频通话来电；
4. 两个设备输入同样的号码呼叫，即可视频互通。

## 运行环境
* Xcode 9.0 +
* iOS 10.0 +
* 真机设备，不支持模拟器

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的bug，欢迎提交 [issue](https://github.com/AgoraIO/Agora-RTC-With-CallKit/issues)

## 代码许可

The MIT License (MIT).
