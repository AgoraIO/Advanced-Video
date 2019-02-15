agora_demo_qt 测试项目
注意需要visual studio 2015的环境编译  还需要额外从agora 的官网下载对应的windows sdk 文件
Agora_native_sdk/dll 用到的dll库文件
Agora_native_sdk/include   头文件
Agora_native_sdk/lib    附加库文件
Agora_native_sdk/videokit 开启视频播放的桥接文件


注意:
视频播放文件的相关接口 只需要调用videokit.h 头文件下的接口就行了
相关的接口如下：
//需要注册videoplaykit的引擎    
void CreateVideoPlayerKitWithRtcEngine(agora::rtc::IRtcEngine *rtcEngine, int sampleRate);    
//设置接口回调 
void setHandler(VideoPlayerKitEventHandler *handler);    
//打开需要播放的文件    
void loadVideo(char * str,bool isAutoPlay);    
//获取当前文件播放的进度    
long getCurrentPosition();    
//跳转到视频文件播放的位置    
void seekTo(int msec);    
//从头开始    
void play();    
//暂停播放    
void pause();    
//恢复播放    
void resume();    
//退出视频播放    
void unload();    
//是否混入麦克风的声音数据    
void setAudioMixing(bool isAudioMix);
//推流
void publish();
//不推流
void unPublish();

相关的调用示例可以参考Demo中的QTVideKit.cpp 文件下相关实现

另外注意一下 QT环境的相关设置
SOURCES +=   
          #引入videokit文件夹下的cpp文件    
          $$PWD/Agora_native_sdk/videokit/ExtendAudioFrameObserver.cpp \    
          $$PWD/Agora_native_sdk/videokit/videoinfocallback.cpp \    
          $$PWD/Agora_native_sdk/videokit/videokit.cpp
          $$PWD/Agora_native_sdk/videokit/ZD3DRender.cpp


  


#添加头文件
INCLUDEPATH += $$PWD/Agora_native_sdk/include \               
                        $$PWD/Agora_native_sdk/videokit

#添加附加库文件 
win32: LIBS += -L$$PWD/Agora_native_sdk/lib/ -lagora_rtc_sdk -lre_sampler -lVideoPlayerKit

