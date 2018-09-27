# Open Duo for Android

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成 Agora 视频 SDK 和信令 SDK，实现一个带呼叫功能的演示程序(本程序不带离线推送功能，比如 [GCM](https://developers.google.com/cloud-messaging/)/[APNs](https://developer.apple.com/library/content/documentation/NetworkingInternet/Conceptual/RemoteNotificationsPG/APNSOverview.html) 或其它，如需此功能需要开发者自行去实现)。

在这个示例项目中包含了以下功能：

- 登录信服务器（APP层限制只允许数字登录）；
- 查询呼叫对象是否在线；
- 呼叫对方，加入通话，结束通话；
- 静音和解除静音；
- 切换前置摄像头和后置摄像头；

Agora 视频/信令 SDK 支持 iOS / Android / Web 等多个平台，你可以查看对应各平台的示例项目：

- [OpenDuo-iOS-Objective-C](https://github.com/AgoraIO/OpenDuo-iOS-Objective-C)
- [OpenDuo-Web](https://github.com/AgoraIO/OpenDuo-Web)

## 运行示例程序
**首先** 在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。
将 AppID 填写进 "app/src/main/res/values/strings.xml"

```
<string name="agora_app_id"><#YOUR APP ID#></string>

```

`本项目代码当前适配了视频 SDK 2.1.1 版本以及信令 SDK 1.2.1 版本，如果使用不同版本的 SDK 可能会需要稍微调整代码里的调用代码`

**其次** 在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载视频通话 SDK 和信令 SDK，解压后将其中的 **libs** 文件夹下的 ***.jar** 复制到本项目的 **app/libs** 下，其中的 **libs** 文件夹下的 **arm64-v8a**/**x86**/**armeabi-v7a** 复制到本项目的 **app/src/main/jniLibs** 下。

在本项目的 "app/build.gradle" 文件依赖属性中添加如下依赖关系（此处代码中已添加示例）：

```
compile fileTree(dir: 'libs', include: ['*.jar'])
```

**最后** 用 Android Studio 打开该项目，连上设备，编译并运行。

也可以使用 `Gradle` 直接编译运行。

## 运行环境
- Android Studio 2.0 +
- 真实 Android 设备 (Nexus 5X 或者其它设备)
- 部分模拟器会存在功能缺失或者性能问题，所以推荐使用真机

## 联系我们
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/OpenDuo-Android/issues)

## 代码许可
The MIT License (MIT).
