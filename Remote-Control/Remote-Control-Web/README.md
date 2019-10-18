# Agora Web Remote Control Demo

这是一个利用 AgoraRTCSDK 和 AgoraRTMSDK 实现浏览器远程控制桌面的一个 Demo，以下简称为 AgoraWRC。

## 使用说明
本 Demo 分为 2 个部分：基于 `Electron` 的桌面端和基于 `Web` 的网页端。其中，桌面端是受控端，负责发布自己的屏幕共享流以及执行来自网页端的控制命令，网页端是控制端，负责订阅桌面端的屏幕共享流，并根据鼠标键盘事件向桌面端发送控制命令。

使用时，需要先打开桌面端的应用程序，选择 `作为受控设备`。待连接建立完成后，界面上会给出当前 AgoraWRC 运行的 `channel` 和 `password`。之后再在 **另一台** 设备上打开网页端的网页，先后输入相应的 `channel` 和 `password` 即可远程控制刚刚的设备。

> 目前只支持一对一的远程控制

## 构建及代码说明
首先，整个代码分为桌面端和网页端 2 个部分，也就是当前目录下的 `wrc-desktop` 和 `wrc-website` 文件夹，我们之后会分开介绍。其次，整个代码是基于 `AgoraRTC` 和 `AgoraRTM` 2 个 SDK，在构建代码之前，需要您手动将您的 `APPID` 同步到代码里。

> 因为考虑到泛用性，Demo 在编写的时候默认您没有开启 [App 动态密钥](https://docs.agora.io/cn/Agora%20Platform/terms?platform=All%20Platforms&#app-certificate), 是直接通过 APPID 加入 channel 的，如果需要启用动态密钥，您可能需要手动修改部分代码

接下来开始正式的代码说明和构建步骤

### 网页端：wrc-website

先看网页端，这是通过 `create-react-app` 生成的项目，使用的技术栈是 `react` + `mobx`，下面介绍一下大致的代码构成:

- `./src/pages` 这里主要存放不同的路由页面 UI，主要就 2 个，主页和远程控制页
- `./src/controller` 这里主要存放 AgoraWRC 的核心控制逻辑，AgoraRTC 和 AgoraRTM 2 个 SDK 主要的操作都在这里，主要是订阅远端流以及捕获鼠标/键盘事件并通过 RTM 广播
- `./src/stores` 这里存放 Mobx Store，用来管理 UI 相关的状态，以及处理一些通过 UI 触发的 action
  - `./src/stores/wrcStore.ts` 这是用来处理和传递 AgoraWRC 相关逻辑的核心 Store，差不多算是 AgoraWRC Controller 和 UI 的中间层
- `./src/types` 因为网页端和桌面端会有信令交换，这里定义了信令的基本格式和类型

如果想要构建网页端，按照以下步骤
- 修改 `./src/constant.ts` 文件，将 APPID 替换成您自己的 APPID
- 执行 `yarn` 安装依赖
- 执行 `yarn start` 开启本地服务
- 执行 `yarn build` 以生产模式构建打包代码，输出在 `build` 目录下

### 桌面端：wrc-desktop

> 桌面端使用的架构是 Electron + Agora Web SDK

桌面端的结构和网页端类型，但是因为 Electron 分为 `Renderer` 和 `Main` 2 个 Process，所以整个结构看起来会复杂一点。桌面端是通过 `electron-forge` 生成的，使用的技术栈依然是 `react` + `mobx`：

- `./src/main.ts` 为 Electron Main Process 的入口文件，处理所有 Main Process 相关的逻辑
- `./src/controller` 这里存放 AgoraWRC 的核心控制逻辑，分为 2 个部分
  - `./src/controller/client.ts` 用于给 Renderer Process 引用，主要是采集发布屏幕共享，以及接收 RTM 的广播消息
  - `./src/controller/backend.ts` 用于给 Main Process 引用，主要是通过 [robotjs](http://robotjs.io) 这个原生模块来执行远端的远程控制命令
- `./src/renderer` 为 Electron Renderer Process 的入口文件，下面的结构和网页端差不多，可以参考网页端的结构来阅读

如果想要构建桌面端，按照以下步骤
- 修改 `./src/renderer/constant.ts` 文件，将 APPID 替换成您自己的 APPID，根据需要，更新 WRC_WEBSITE 的地址
- 执行 `yarn` 安装依赖
- 执行 `yarn start` 以开发模式运行 Electron 应用
- 执行 `yarn make` 以生产模式打包 Electron 应用(在 make 之前需要执行过 start)
