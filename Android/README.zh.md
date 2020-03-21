# Android Samples for Advanced Applications of Agora Video SDK APIs

*[English](README.md) | 中文*

## 模块

### 自定义视频源和自定义视频渲染

如果项目里已经存在其它视频来源（其它采集模块，其它设备等）并进行定制的视频处理，开发者可以使用 MediaIO 接口发送视频到 Agora SDK 进行传输。 这个方式需要开发者自行维护采集、渲染的生命周期，需要一定的技术能力。

Demo 以两个外部视频源为例（屏幕分享和本地视频），为开发者提供了一种自定义视频源的切换、发送和渲染的方案。 当然，不同项目的实现要结合实际情况进行选择。

关于 MediaIO 视频源和渲染接口的使用，请参考[此页](https://docs.agora.io/en/Video/custom_video_android?platform=Android)

常用的视频帧的格式有 i420, NV21 和 texture，此 Demo 主要展示如何维护 OpenGL 环境，并使用 texture 进行视频的采集和渲染。

OpenGL 线程的维护的思路来自 Google 的 [grafika](https://github.com/google/grafika) 项目，有关于这个库的使用请移步该项目主页。

以下的描述文字以及项目代码中，source 专门指代 MediaIO 中的 external source，即 Agora SDK 接收的外部视频源（区别于 Agora SDK 的采集模块）； 而不同种类的外部视频源（相机、屏幕分享和视频文件等）被称为不同的视频输入源 (video input)。


#### 屏幕分享

屏幕分享功能是 Android 5.0 引入的，所以项目的 min sdk level 不应小于21.

通常屏幕分享功能有很多后台运行的场景，所以为了避免分享程序不会被系统强制杀死，需要将其跑在一个 Foreground Service 中。 Android OREO 及以后的版本， 系统强制要求凡是开启的 Foreground Service 都需要向系统注册一个通知，以便于用户知晓应用的运行情况。

#### 视频文件

为了实现简单，demo 在运行时要求使用者拷贝自己的 .mp4 文件到应用下的指定路径。 开发者可以自行开发获取视频文件的方式。

使用 [MediaExtractor](https://developer.android.com/reference/android/media/MediaExtractor) 可以从 .mp4 文件中将视频帧解压出来（通常是以 ByteBuffer 的格式），之后以一定的频率（比如帧的播放时间戳）发送到 [MediaCodec](https://developer.android.com/reference/android/media/MediaCodec) 解码器， 解码的结果会绘制到一个预先设置的 Surface 中（由 texture 生成），继而进行本地渲染或者视频发送。

#### 视频输入的切换

Demo 将从不同的方式获取到的视频抽象为 video input，从外部看，这些 input 最终的输出结果就是一个 texture，视频源可以在任何时候进行切换，且 Agora SDK 对此并无感知。

Foreground Service 中维护了一个 OpenGL 线程，texture 在这个线程里进行处理、发送和渲染（通常指的是本地渲染），这样可以比较方便的维护 OpenGL 环境，几乎不需要考虑多线程问题。

### 使用 Agora SDK 的裸数据插件接口进行音视频包的自定义加密

关于如何使用裸数据接口，请参考文档：[音频裸数据](https://docs.agora.io/en/Video/raw_data_audio_android?platform=Android) 和 [视频裸数据](https://docs.agora.io/en/Video/raw_data_video_android?platform=Android)

包加密是音视频裸数据接口的应用场景之一，开发者可以在数据发送之前对音视频的网络数据包进行自定义处理，并在接收端执行逆向操作以恢复原始数据。

需要注意的是，由于性能等方面的考量，视频只有 C++ 裸数据的接口可以使用；目前仅有音频的裸数据可以使用 Java 接口。在对性能有一定要求的场合，仍然推荐使用 C++ 的接口。

C++ 裸数据接口的代码需要将其编译成以 "libamp-xxx.so" 的形式命名的动态库文件。 Agora RTC 引擎会在插件初始化阶段寻找这些文件进行加载。

在 JCenter 上发布的 sdk 是没有 C++ 裸数据接口的头文件的。虽然 demo 里已经上传了相关文件，但若以后有更新，则必须到 Agora 下载页进行下载和覆盖更新。当然也可以采用直接拷贝 sdk 的方式进行集成。

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
