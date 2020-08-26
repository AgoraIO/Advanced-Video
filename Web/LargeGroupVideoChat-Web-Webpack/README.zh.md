# LargeGroupVideoChat-Web - Webpack

*[English](README.md) | 中文*

这个开源示例项目演示了如何快速集成 Agora 视频 SDK。

在这个示例项目中包含了以下功能：

- 加入/离开频道
- 选择媒体设备
- 分享观众链接
- 通过观众链接以观众身份加入频道

## 环境准备

- nodejs LTS
- web浏览器

## 运行示例程序

这个段落主要讲解了如何编译和运行示例程序。

### 创建Agora账号并获取AppId

在编译和启动实例程序前，您需要首先获取一个可用的App ID:
1. 在[agora.io](https://dashboard.agora.io/signin/)创建一个开发者账号
2. 前往后台页面，点击左部导航栏的 **项目 > 项目列表** 菜单
3. 复制后台的 **App ID** 并备注，稍后启动应用时会用到它
4. 在项目页面生成临时 **Access Token** (24小时内有效)并备注，注意生成的Token只能适用于对应的频道名。


### 集成 Agora 视频 SDK

1. 在Terminal中，在您的项目根目录输入`install`命令以安装项目依赖
    ```shell
    # install dependencies
    npm install
    ```
2. 输入`run dev`命令以启动Web程序
    ```shell
    # serve with hot reload at localhost:8080
    npm run dev
    ```
    输入`npm run build`会压缩静态资源文件，可作为生产环境打包发布。
    ``` bash
    # build for production with minification
    npm run build
    ```
3. 你的浏览器默认会打开示例应用程序。
    **注意** 如果没有自动打开，请在浏览器里手动输入URL `http://localhost:8080`。

## 联系我们

- 如果你遇到了困难，可以先参阅 [常见问题](https://docs.agora.io/cn/faq)
- 如果你想了解更多官方示例，可以参考 [官方SDK示例](https://github.com/AgoraIO)
- 如果你想了解声网SDK在复杂场景下的应用，可以参考 [官方场景案例](https://github.com/AgoraIO-usecase)
- 如果你想了解声网的一些社区开发者维护的项目，可以查看 [社区](https://github.com/AgoraIO-Community)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 若遇到问题需要开发者帮助，你可以到 [开发者社区](https://rtcdeveloper.com/) 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)


## 代码许可

The MIT License (MIT)
