# Agora Video With ARKit

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何结合 Agora视频SDK 和ARKit，实现 AR 场景下的视频连麦直播。

在这个示例项目中包含了以下功能：

- 使用自定义采集协议，将 ARCamera 的视频数据直播到频道中；
- 使用自定义渲染协议，将频道内连麦端的视频数据渲染到 AR 场景中。

## 运行示例程序
首先在 [Agora.io](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 ViewController.swift 对应位置。

然后在 [Agora.io SDK](https://www.agora.io/cn/blog/download/) 下载 **视频通话 + 直播 SDK**，解压后将其中的 **libs/AgoraRtcEngineKit.framework** 复制到本项目的 "Agora-Video-With-ARKit" 文件夹下。

> 自定义采集和自定义渲染协议从 2.1.0 版本开始提供

最后使用 Xcode 打开 Agora-Video-With-ARKit.xcodeproj，连接 iPhone／iPad 测试设备，设置有效的开发者签名后即可运行。

##使用方法
1. 对着一个较大的水平面缓慢移动，直到 ARKit 识别出平面(会有一个红色矩形闪现)；
2. 在识别出的平面任意位置点击，添加虚拟现实屏；
3. 使用同一个 AppId 运行 [OpenLive](https://github.com/AgoraIO/OpenLive-iOS)，以主播身份加入 "agoraar" 频道；
4. 虚拟显示屏上会出现主播的画面。

## 运行环境
* Xcode 9.0 +
* 支持 ARKit 的 iOS 真机设备
* 不支持模拟器

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的bug，欢迎提交 [issue](https://github.com/AgoraIO/Agora-Video-With-ARKit/issues)

## 代码许可

The MIT License (MIT).
