# Mediaplayer-Mac
这个开源项目演示了如何利用 Agora 视频 SDK 和 Agora_MediaPlayer_Kit 插件来实现视频播放内容共享。

## 运行示例程序
首先在 [Agora.io](https://dashboard.agora.io/) 注册账号，并创建自己的测试项目，获取到 AppID，并将 AppID 填写进 KeyCenter.swift  对应位置


```
static let AppId: String = "YOUR_APPID";
```


然后在 [Agora.io SDK](https://docs.agora.io/cn/Agora%20Platform/downloads) 下载**视频通话/视频直播 SDK**，解压后将其中的 **AgoraRtcEngine.framework**  复制到libs 文件夹下。

还需要下载  [MediaPlayerKit]( https://download.agora.io/sdk/release/Agora_MediaPlayerKit_for_macOS_v1_0_0.zip) 同样将解压后的 MediaPlayerKit.framework 文件夹复制到 libs 文件夹下。


最后打开MediaPlayerKitQuikstart.xcodeproj 编译运行即可


## 注意

## 使用方法
1. 填入频道号加入频道；
2. 点击 open 按钮, 选择需要播放的文件；
3. 观众端使用另一台设备，同样打开本示例程序，加入到相同的房间, 即可欣赏主播端播放的视频内容；

## 运行环境
* Xcode 9.0  编译环境
* Mac OS 10.12  以上版本

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT).
