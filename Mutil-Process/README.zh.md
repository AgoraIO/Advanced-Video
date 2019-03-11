# Agora-MutilProcess-Education-ShareScreen
> 该demo 演示了如何用AgoraSDK推双流的方法.


**主调进程：是正常的直播推流程序**

**processShareScreen 是共享屏幕进程**

**主调进程会向共享进程发送 一些基本指令： 基本数据指令，开始推流，停止推流**



## 在共享进程里面基本逻辑：

###默认调用的api
enablevideo();

disableAudio();

muteAllRemoteVideoStream(true);

muteAllRemoteAudioStream(true);

###基本默认数据
同步一些基本的数据包括appid,秘钥，频道，uid等。

###开始推流：
join,startscreenShare.

###停止推流：
stopScreenShare.leavechannel.

**建议 processShareScreen 不要常驻内存，需要用到的时候就开启该进程，停止屏幕共享，就关掉该进程**


