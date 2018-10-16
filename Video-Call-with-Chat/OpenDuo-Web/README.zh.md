# Agora Open Duo (WEB)

*README in [English](readme_en.md).*

这个开源示例项目演示了如何快速集成 Agora 信令 SDK 实现一个简单的视频电话应用。

在这个示例项目中包含了以下功能：

- 登录信令服务器
- 键入对方姓名，进行视频电话
- 接收对方的视频电话，进行视频电话
- 注销


## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。
然后选择测试项目里的编辑，App Certificate 点击启用，根据操作拿到App Certificate。
将 AppID 填写进 agora.config.js 中的两个常量中

``` javascript
const appid = "YOUR_AGORA_APPID";
```

将您获得的信令SDK文件'AgoraSig-*.js' 重命名为 'AgoraSig.js' 置于'/static'目录下。在项目根目录使用npm安装项目依赖，并使用webpack打包出发布文件  

``` bash
# install dependency
npm install
# generate dist
npm run build
```
根目录下会生成build目录，注意**请勿直接使用浏览器将html当作静态文件使用文件协议打开，必须使用http／https协议**，也就是说请部署服务器或使用 Python simpleHTTPServer 模块。

### 关于Token
在登录信令服务器时可提供一个参数token，一般由服务器计算提供作为身份凭证，默认不使用，如需使用，请重写signalingClient.js中的login函数  

``` javascript
//... 
let session = this.signal.login(account,'_no_need_token');
//... 
```
### 关于动态key
在使用视频通讯服务时可提供动态key，一般由服务器计算提供作为身份凭证，默认不适用，如需使用，请修改rtc.js中的getDynamicKey函数
``` javascript
getDynamicKey(channelName){
    // if dynamic not enabled
    return new Deferred().resolve(undefined).promise();

    // if dynamic key enabled
    // return $.ajax({
    //     url: 'service url to get your dynamic key'
    // })
}
```


## 联系我们
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/Agora-Android-Tutorial-1to1/issues)

## 代码许可
The MIT License (MIT).
