# AgoraPluginRawDataAPI for Android

*Read this in other languages: [English](README.en.md)*

这个示例项目演示了如何使用 Agora 裸数据接口，实现获取音视频数据流的功能。

在这个示例项目中包含了以下功能：

- 获取本地视频采集数据，其他用户视频数据；
- 获取本地音频采集数据，其他用户音频数据；
- 本地视频和远端视频截图功能

## 运行示例程序
**首先**在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 "app/src/main/res/values/strings.xml"

```
<string name="agora_app_id"><#YOUR APP ID#></string>
```

**然后**是集成 Agora SDK：
 第一步: 在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 **视频通话 + 直播 SDK**，解压后将其中的 **libs** 文件夹下的 ***.jar** 复制到本项目的 **app/libs** 下，其中的 **libs** 文件夹下的 **arm64-v8a**/**x86**/**armeabi-v7a** 复制到本项目的 **app/src/main/jniLibs** 下，其中的**include**文件夹下的所有文件复制到本项目的
**raw-data-api-java/src/main/cpp/include**下。

 第二步: 在本项目的 "app/build.gradle" 文件依赖属性中添加如下依赖关系：

```
compile fileTree(dir: 'libs', include: ['*.jar'])
```

**最后**用 Android Studio 打开该项目，连上设备，编译并运行。


## 单独编译 aar 包
- 本程序作为一个Module程序，需要添加到主项目中作为依赖项目运行
- setting.gradle 中添加  include ':raw-data-api-java'
- 主项目 build.gradle 下添加 
	dependencies {
	    compile project(path: ':raw-data-api-java')
	}
- 项目需要配置 NDK 路径，同Android SDK路径配置


## 代码示例
- 在需要接收回调的类，实现回调方法       class implements MediaDataAudioObserver, MediaDataVideoObserver
- 初始化 MediaDataObserverPlugin 对象  MediaDataObserverPlugin mediaDataObserverPlugin = MediaDataObserverPlugin.the();
- 设置数据回调                         MediaPreProcessing.setCallback(mediaDataObserverPlugin);
- 添加视频数据回调监听                  mediaDataObserverPlugin.addVideoObserver(this);
- 添加音频数据回调监听                  mediaDataObserverPlugin.addAudioObserver(this);
- 移除视频回调监听                      mediaDataCallbackUtil.removeVideoObserver(this);
- 移除音频回调监听                      mediaDataCallbackUtil.removeAudioObserver(this);
- 本地视频截图                         mediaDataCallbackUtil.saveCaptureVideoShot(path);
- 远端视频截图                         mediaDataCallbackUtil.saveRenderVideoShot(path);

## 运行环境
- Android Studio 2.0 +
- 真实 Android 设备 (Nexus 5X 或者其它设备)
- 部分模拟器会存在功能缺失或者性能问题，所以推荐使用真机

## 联系我们
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/Agora-Plugin-Raw-Data-API-Android-Java/issues)

## 代码许可
The MIT License (MIT).
