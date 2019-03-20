# Mediaplayer-Windows
这个开源项目演示了如何利用Agora视频SDK 和 Agora_Video_Kit插件 来实现视频播放内容共享

# 运行示例程序
首先在 [Agora.io](https://dashboard.agora.io/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 agorartcengine.cpp 对应位置(在context.appId = APP_ID 处用你申请到的AppID 替换APP_ID)。

然后在 [Agora.io SDK](https://docs.agora.io/cn/Agora%20Platform/downloads) 下载 **视频通话/视频直播 SDK**，解压后将其中的 **sdk** 文件夹复制到 AgoraVideoCall.sln文件同级目录下。

还需要下载  [Agora_Video_Kit](https://github.com/AgoraIO/Advanced-Video/releases/download/MediaPlayer1.0/Agora_Video_Kit_1.0.zip) 同样将解压后的Agora_Video_Kit文件夹复制到AgoraVideoCall.sln文件同级目录下。

最后打开AgoraVideoCall.sln 文件编译运行即可

### 注意 Agora_Video_Kit/dll文件下的dll库和sdk/dll文件下的dll库需要放在相应的执行路径下

## 使用方法
1. 填入频道号加入频道；
2. 点击open按钮 选择需要播放的文件；
3. 观众端使用另一台设备，同样打开本示例程序，加入到相同的房间 即可欣赏主播端播放的视频内容；

## 运行环境
* Visual studio 2015 编译环境
* Qt5.6.2 以上版本

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题，你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题，可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持，你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT).
