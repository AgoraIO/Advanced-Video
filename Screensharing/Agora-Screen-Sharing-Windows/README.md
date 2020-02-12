# Agora-MutilProcess-Education-ShareScreen

*English | [中文](README.zh.md)*

This tutorial shows you how to streaming camera video and screen sharing simultaneously using the Agora sample app.

The small demo contains following features:

- Join/Leave Channel     
- Start/Stop ScreenSharing

## Quick Start

This section shows you how to prepare, and run the sample application.

### Obtain an App ID

To build and run the sample application, get an App ID:
1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App ID** from the Dashboard for later use.


### Install dependencies and integrate the Agora Video SDK

1. Download the Agora Video SDK from Agora.io SDK. Unzip the downloaded SDK package and copy the sdk to the project folder(the old one may be over written).
2. Replace the appid in AgoraObject.h
```
#define APP_ID _T("Your App ID")
```

## Resources

- You can find full API document at [Document Center](https://docs.agora.io/en/Video/API%20Reference/web/index.html)
- You can file bugs about this demo at [issue](https://github.com/agoraio/Advanced-Video)

## License

The MIT License (MIT)
