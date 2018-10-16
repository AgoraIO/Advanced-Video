# Agora OpenDuo (Web)

*其他语言版本： [简体中文](README.zh.md)*

The Agora OpenDuo Sample App supports the following platforms:
* [iOS](https://github.com/AgoraIO/OpenDuo-iOS-Objective-C)
* [Android](https://github.com/AgoraIO/OpenDuo-Android)
* [Web](https://github.com/AgoraIO/OpenDuo-Web)

This readme describes the steps and considerations for demonstrating the Agora Web OpenDuo sample app.

## Introduction

Built upon the Agora Video SDK and the Agora Signaling SDK, the Agora Web OpenDuo sample app is an open-source demo that integrates video chat into your Web applications.

This sample app allows you to:
* Login the signaling server
* Call each other
* Join the media channel
* Leave the media channel

## Preparing the Developer Environment

1. Ensure that your device has a camera and sufficient CPU power and memory with one of the following requirements:

  * macOS
  * Windows 7 or later
  * iOS 11 or later
  * Android 4.1 or later
 
 2. Ensure that your device has one of the following browsers:
 
  - Google Chrome 58 or later
  - Firefox 56 or later
  - Opera 45 or later
  - Safari 11 or later
  - QQ browser

## Running the App
1. Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), obtain an App ID, and enable the App Certificate. 
2. Fill in the AppID and the App Certificate in agora.config.js:

          const appid = "YOUR_SIGNALING_APPID";
      
3. Download the Signaling SDK from [Agora.io](https://www.agora.io/en/download/). 
4. Unzip the downloaded SDKs to /static folder, rename Signaling SDK to 'AgoraSig.js'
4. Run npm in the root directory of your project to install the dependency. 
   
          npm install
   
5. Use gulp to build the project.

         npm run build
   
*A “build” directory is generated under **/root** of your project.*

NOTE: Deploy this project on the server and view the page using http/https. Do not double click the corresponding file to view the page. 

## About SignalingToken

When you login the signaling server, you can use SignalingToken provided by the server for authentication purposes. SignalingToken is not used by default. To use SignalingToken, rewrite the following login function in signalingClient.js.

      //... 
      let session = this.signal.login(account,'_no_need_token');
      //... 

## About the Dynamic Key

You can use the Dynamic Key provided by the server for authentication purposes. The Dynamic Key is not used by default.  To enable or disable it, modify the getDynamicKey function in rtc.js. 

* If the Dynamic Key is not enabled:

        getDynamicKey(channelName){
        return new Deferred().resolve(undefined).promise();
        }
        
 * If the Dynamic Key is enabled: 
 
        getDynamicKey(channelName){
          return $.ajax({
              url: 'service url to get your dynamic key'
          })
        }
        
## Contact Us
 
* You can find the API documentation at [Developer Center](https://docs.agora.io/en/).
* You can report issues about this demo at [issue](https://github.com/OpenDuo-Web/issues).

## License

The MIT License (MIT). 













