# Agora Screen Sharing Android


*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何用 Agora 视频 SDK 实现屏幕共享，默认我们 SDK 是会订阅远端流，只是不显示，但这样就会产生额外的计费
考虑到屏幕共享的引用场景大多数并不需要观看远端的音视频流，因此在本例中 `joinChannel` 前加入如下代码

```
mRtcEngine.muteAllRemoteAudioStreams(true);
mRtcEngine.muteAllRemoteVideoStreams(true);
```
避免接收远端的音视频流数据。


在这个示例项目中包含了以下功能：

- 开始或结束屏幕共享

你可以在这里查看 Agora 视频 SDK 的入门示例项目：[Agora-Android-Tutorial-1to1](https://github.com/AgoraIO/Basic-Video-Call/tree/master/One-to-One-Video/Agora-Android-Tutorial-1to1)

## 运行示例程序
**首先**在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 "app/src/main/res/values/strings.xml"

```
<string name="agora_app_id"><#YOUR APP ID#></string>
```


**然后**是集成 Agora 视频 SDK，集成方式有以下两种：

- 首选集成方式：

在项目对应的模块的 "app/build.gradle" 文件的依赖属性中加入通过 JCenter 自动集成 Agora 视频 SDK 的地址：

```
implementation 'io.agora.rtc:full-sdk:2.4.0'
```
(如果要在自己的应用中集成 Agora 视频 SDK，添加依赖链接地址是最重要的一步。）

- 次选集成方式：

第一步: 在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 **视频通话 + 直播 SDK**，解压后将其中的 libs 文件夹下的 *.jar 复制到本项目的 app/libs 下，其中的 libs 文件夹下的 arm64-v8a/x86/armeabi-v7a 复制到本项目的 app/src/main/jniLibs 下。

第二步: 在本项目的 "app/build.gradle" 文件依赖属性中添加如下依赖关系：

```
implementation fileTree(dir: 'libs', include: ['*.jar'])
```

**最后**用 Android Studio 打开该项目，连上设备，编译并运行。

也可以使用 `Gradle` 直接编译运行。

## 运行环境
- Android Studio 3.1+
- 真实 Android 设备 (Nexus 5X 或者其它设备)
- 部分模拟器会存在功能缺失或者性能问题，所以推荐使用真机

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT).
