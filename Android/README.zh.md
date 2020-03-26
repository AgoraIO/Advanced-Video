# Android Samples for Advanced Applications of Agora Video SDK APIs

*[English](README.md) | 中文*

## 模块

### 自定义视频源

默认 Agora SDK 会启动视频模块进行的采集和后续的处理。同时 SDK 也提供了 MediaIO 接口，可以将第三方的视频源传给 Agora SDK 进行传输。 这就需要开发者具备一定的技术能力，自行维护采集和渲染的生命周期。

Demo 包含两个外部视频源的示例代码（屏幕分享和本地视频），提供一个发送自定义视频源的参考方案。不同项目要结合实际情况选择实现方案。

关于 MediaIO 视频源接口的使用，请参考[此页](https://docs.agora.io/cn/Interactive%20Broadcast/custom_video_android?platform=Android)

通常视频帧的格式为 i420, NV21 或 texture。很多情况下开发者需要创建和管理 OpenGL 线程。Demo 维护 OpenGL 线程维护的思路来自 [grafika](https://github.com/google/grafika) 项目，如果您不熟悉或者想深入了解这个项目请移步其主页。

以下的段落以及项目代码中，我们将第三方视频源定义为外部视频源（即 `external`），而由 Agora SDK 视频模块采集的视频可视为内部源； 而不同的外部视频源（比如上述屏幕分享和视频文件）称为视频输入源 `video input`。

#### 屏幕分享

屏幕分享功能是 Android 5.0 引入的，所以项目的 min sdk level 不应小于21。

屏幕分享功能大多要求能够后台运行，为了避免分享程序不会被系统强杀，需要将其跑在一个 foreground service 中。 Android Oreo 及后续版本要求同时向系统注册一个通知，以便于用户知晓应用的运行情况。

#### 视频文件

Demo 不提供 .mp4 视频文件，使用者应自行拷贝所需的视频文件到应用私有路径下并重命名：`storage/emulated/0/Android/data/io.agora.advancedvideo.switchvideoinput/files/Movies/localvideo.mp4`

使用 [MediaExtractor](https://developer.android.com/reference/android/media/MediaExtractor) 从 .mp4 文件中将视频帧解压出来（保存到 ByteBuffer），以一定的频率（比如帧的播放时间戳）发送到 [MediaCodec](https://developer.android.com/reference/android/media/MediaCodec) 解码器， 解码的结果被绘制到一个预先设置的 Surface 中（由 texture 生成），继而进行本地渲染或者视频发送。

#### <h4 id="switching"> 视频输入源的切换 </h4>

同一时间只有一个视频输入源在工作，视频输入源可以随时以 SDK 无感知的方式进行切换。 视频帧在 foreground service 的 OpenGL 线程里进行处理、发送和渲染（通常指的是本地渲染），因此可以进行后台的采集和发送。 

### 使用 Agora SDK 的裸数据插件接口进行视频包的自定义加密

关于裸数据接口的使用，请参考文档：[音频裸数据](https://docs.agora.io/cn/Video/raw_data_audio_android?platform=Android) 和 [视频裸数据](https://docs.agora.io/cn/Video/raw_data_video_android?platform=Android)

包加密是视频裸数据接口的应用场景之一，开发者可以在数据发送之前对音视频的网络数据包进行自定义处理，并在接收端执行逆向操作以恢复原始数据。

需要注意的是，由于性能等方面的考量，视频只有 C++ 裸数据的接口可以使用；目前仅有音频的裸数据可以使用 Java 接口。在对性能有一定要求的场合，仍然推荐使用 C++ 的接口。

C++ 裸数据接口的代码需要编译成以 **"libamp-xxx.so"** 的形式命名的动态库文件。 Agora RTC 引擎会在插件初始化阶段寻找这些文件并加载。

在 JCenter（maven） 上发布的 sdk 是没有 C++ 裸数据接口的头文件的。当前 demo 里已经上传了相关头文件，但后续的 SDK 版本可能对其进行修改，到时开发者需自行到 Agora 下载页进行下载和覆盖更新。

### Push 接口

除了 MediaIO interface, push 接口也同样用来发送第三方视频源. 更多细节请参考 [Push Interface](https://docs.agora.io/cn/Interactive%20Broadcast/custom_video_android?platform=Android#%E8%87%AA%E5%AE%9A%E4%B9%89%E8%A7%86%E9%A2%91%E9%87%87%E9%9B%86).


在这个 demo 中，我们关注的是 push 接口的实现，忽略了如后台运行和异步多线程等。如果想了解这些细节，请参考 [视频输入源的切换](#switching)

## 运行示例程序

这个段落主要讲解了如何编译和运行实例程序。

### 创建Agora账号并获取AppId

在编译和启动实例程序前，您需要首先获取一个可用的App ID:
1. 在[agora.io](https://dashboard.agora.io/signin/)创建一个开发者账号
2. 前往后台页面，点击左部导航栏的 **项目 > 项目列表** 菜单
3. 复制后台的 **App ID** 并备注，稍后启动应用时会用到它
4. 在项目页面生成临时 **Access Token** (24小时内有效)并备注，注意生成的Token只能适用于对应的频道名。

5. 将 AppID 填写进 "lib-component/src/main/res/values/strings_config.xml"
  ```
  <string name="private_app_id"><#YOUR APP ID#></string>
  <!-- 临时Token 可以在 https://dashboard.agora.io 获取 -->
  <!-- 在正式上线生产环境前，你必须部署你自己的Token服务器 -->
  <!-- 如果你的项目没有打开安全证书，下面的值可以直接留空 -->
  <string name="agora_access_token"><#YOUR TOKEN#></string>
  ```

### 集成 Agora 视频 SDK

*Demo已默认集成JCenter的方式，可以直接编译运行。*

集成方式有以下两种：
  - 通过JCenter集成：
    - 在项目对应的模块的 `lib-component/build.gradle` 文件的依赖属性中加入通过 JCenter 自动集成 Agora 视频 SDK 的地址：
      ```
      implementation 'io.agora.rtc:full-sdk:3.0.0'
      ```
  - 手动集成：
    - 在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 **视频通话 + 直播 SDK**并解压，按以下对应关系将 **libs** 目录的内容复制到项目内。
      
      SDK目录|项目目录
      ---|---
      .jar file|**/lib-component/libs** folder
      **arm64-v8a** folder|**/lib-component/src/main/jniLibs** folder
      **x86** folder|**/lib-component/src/main/jniLibs** folder
      **armeabi-v7a** folder|**/lib-component/src/main/jniLibs** folder
    - 若需要需要使用C++头文件，可以在解压SDK后将其中的 **libs**/**include** 文件夹下的 ***.h** 复制到本项目的 **lib-component**/**src**/**main**/**cpp**/**agora** 下。


### 启动应用程序

用 Android Studio 打开该项目，连上设备，编译并运行。

也可以使用 `Gradle` 直接编译运行。


## 联系我们

- 如果你遇到了困难，可以先参阅[常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考[官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考[官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看[社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT)
