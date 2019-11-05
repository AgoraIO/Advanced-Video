# Agora Web Remote Control Demo

*English | [中文](README_zh.md)*

This is a web based remote control demo, built on `AgoraRTCSDK` and  `AgoraRTMSDK`.

## How to use

This demo has 2 parts: 

	- The desktop app based on `Electron` 
	- The website app based on `ReactJS`

The desktop app is the controlled end, which will publish its screen-sharing stream and execute control signals from the website app. The website app is the controller end, which will subscribe the remote screen-sharing sharing and sends control signals based on captured mouse/keyboard events.

When using, you need to open the desktop app first and click  `As the controlled device`. After the connection is established, the app will show your current `channel` and `password`. Then open the website app on **another device** and enter the current  `channel` and `password`.

> This demo only supports one to one remote control

## How to build

At first, this demo's code has 2 parts (the desktop and the website), which are the `wrc-desktop` and `wrc-website` folders in the current directory, so you need to build 2 projects. Second, this demo is base on `AgoraRTCSDK` and `AgoraRTMSDK`, so you need to manually update your `APPID` into the code before building the code.

### build wrc-website

- edit `./src/constant.ts` , update the `APPID`

```shell
# install dependencies
yarn

# run website app in localhost (development mode)
yarn start

# build website app (production mode with minification)
yarn build
```

The build results are in the `build` folder.

### build wrc-desktop

- Edit `./src/renderer/constant.ts`, update the `APPID`. If you need, update the `WRC_WEBSITE`. This variable will be used to open the online website app

```shell
# install dependencies
yarn

# run desktop app in development mode 
yarn start

# build and pack desktop app (production mode with minification)
yarn make
```

> because of a bug from `electron-forge`，you must have run `yarn start` at least once before running `yarn make`

