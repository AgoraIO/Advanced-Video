# Agora-Custom-Media-Device-Web

*Read this is other languages：[English](README.md)*

## 简介

本开源示例项目基于 Agora Web SDK videoSource 接口开发，演示了如何使用Agora Web SDK，实现自定义视频采集和渲染。

本示例项目中包含以下的功能：

* 使用自定义采集协议，将视频数据采集并发送到直播频道中。
* 使用自定义渲染协议，将渲染频道内连麦的视频数据。

## 准备开发环境

* 请保证你已经安装了nodejs >= 8.0.0

## 运行示例程序

1. 在 [Agora.io](https://dashboard.agora.io/signin/) 注册账号，并创建自己的测试项目，获取到 App ID。
2. 下载本示例程序。
3. 在示例程序的 [config.js](src/assets/js/config.js) 文件中，填入获得的 App ID:
```
export const APP_ID = <#Your App Id#>
```
4. 执行 `npm install` 安装依赖，执行 `npm run build` 运行示例程序。

## 联系我们

* 详细的 API 文档请参考 [文档中心](https://docs.agora.io/en/)
* 如果发现了示例代码的Bug，欢迎提交 [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## 代码许可

The MIT License (MIT).