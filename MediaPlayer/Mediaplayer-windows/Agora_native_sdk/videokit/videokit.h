#ifndef VIDEOKIT_H
#define VIDEOKIT_H
#include "videoinfocallback.h"
#include <IAgoraMediaEngine.h>
#include <IAgoraRtcEngine.h>
#include "ExtendAudioFrameObserver.h"
#include <QObject>
#include <memory>
#include <mutex>
#include "SingleTon.h"
#include "ZD3DRender.h"
class VideoPlayerTool;

enum FF_PLAYER_STATE
{
	FF_PLAYER_STATE_PLAY = 0,
	FF_PLAYER_STATE_PAUSE = 1
	
};
enum FF_LOAD_VIDEO_STATE
{
	FF_LOAD_VIDEO_ERROR_UNSORPPORT_FORMATE = 0, 
	FF_LOAD_VIDEO_ERROR_NULL_FILE = 1,
	FF_LOAD_VIDEO_ERROR_UNSORPPORT_ARGUMENT = 2,
	FF_LOAD_VIDEO_ERROR_PREPARED = 3,
};

class VideoPlayerKitEventHandler{
public:
	VideoPlayerKitEventHandler(){}
	virtual ~VideoPlayerKitEventHandler(){

	}
	//回调函数
	virtual void onVideoLoaded(FF_LOAD_VIDEO_STATE state){};
	//执行失败的回调
	virtual void onKitError(){};
	//播放器停止的回调
	virtual void onPlayerStopped(){};
	//播放状态的回调
	virtual void onPlayerSate(FF_PLAYER_STATE playerState){};
};

class VideoKit : public VideoInfoCallBack,public Singleton<VideoKit>
{
	friend Singleton;
public:
	~VideoKit() {
	
	}
	VideoKit()
	{
		duration = 0;
		handler = NULL;
		isSetWindows = false;
	}
	long duration;
	virtual void onVideoDataCallBack(int linesize[], void *data, int w, int h);
	virtual void onAudioDataCallBack(void *data, int len);
	virtual void onAudioInfoCallBack(int sample_rate, int channles, long duation,int width,int height);
	virtual void onVideoStateCallBack(FF_VIDEO_STATE state);
    //需要注册videoplaykit的引擎
    void CreateVideoPlayerKitWithRtcEngine(agora::rtc::IRtcEngine *rtcEngine, int sampleRate);
    //设置接口回调
	void setHandler(VideoPlayerKitEventHandler *handler);
    //打开需要播放的文件
    void loadVideo(char * str,bool isAutoPlay);
    //获取当前文件播放的进度
    long getCurrentPosition();
    //跳转到视频文件播放的位置
    void seekTo(long msec);
    //从头开始
    void play();
    //暂停播放
    void pause();
    //恢复播放
    void resume();
    //退出视频播放
    void unload();
    //是否混入麦克风的声音
    void setAudioMixing(bool isAudioMix);
	//开启视频预览
	void publish();
	//如果之前有开启视频预览请务必在加入频道成功之后 调用该接口关闭视频预览  
	void unpublish();
	//设置需要渲染的窗口句柄
	int setPreviewWindow(void * hwnd);
public:
	void refreshWindows();
private:
	bool isExit = false;
	bool isPlaying = false;
	bool isVieoPublish = true;
    agora::rtc::IRtcEngine *_rtcEngine;
    //timeStamp
     static UINT64 WINAPI GetHighAccuracyTickCount()
     {
         LARGE_INTEGER	liPC;
         LARGE_INTEGER	liPF;

         ::QueryPerformanceCounter(&liPC);
         ::QueryPerformanceFrequency(&liPF);

         return static_cast<UINT64>(liPC.QuadPart * 1000  / liPF.QuadPart);
     }

	VideoPlayerTool *videopool;
    std::mutex m_mutex;
	VideoPlayerKitEventHandler *handler;
	ZD3DRender d3dRender;
	int m_width;
	int m_height;
	void *m_hwnd;
	int pic_width;
	int pic_height;
	bool isSetWindows;
};
#endif // VIDEOKIT_H
