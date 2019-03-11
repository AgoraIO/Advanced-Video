# Agora-Custom-Media-Device-Web

*其他语言版本：[简体中文](README.zhCN.md)*

## Introduction

The Agora-Custom-VideoSource-Web Sample App is an open-source demo that helps you get live video chat integrated into your applications with custom video source.

With this Sample App, you can:

* Send captured video from custom video source to live video channel
* Render frames of live video channel with custom video renderer

## Preparing the Developer Environment

* Ensure that your have already installed nodejs >= 8.0.0

## Running the App

1. Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID.
2. Download the Sample App.
3. Fill in the APP ID in the [config.js](src/assets/js/config.js) file of the Sample App:
```
export const APP_ID = <#Your App Id#>
```
4. run `npm install` and `npm run build` to start demo.

## Connect Us

* You can find full API documents at [Document Center](https://docs.agora.io/en/)
* You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).