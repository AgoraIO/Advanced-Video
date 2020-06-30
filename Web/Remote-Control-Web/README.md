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

- wrc-desktop and wrc-website are two different projects and need to be built respectively.
- you will need to manually update your APPID into the code before building the code.

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
The build output can be found in the build folder.

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

## Contact Us
- For potential issues, take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT)

