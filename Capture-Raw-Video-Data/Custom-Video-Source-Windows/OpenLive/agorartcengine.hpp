#pragma once
#include <memory>
#include <Agora/IAgoraRtcEngine.h>
#include <Agora/IAgoraMediaEngine.h>
#include <Agora/IAgoraRtcEngine2.h>
#include <string>
#include "ExtendAudioFrameObserver.hpp"
#include "ExtendVideoFrameObserver.hpp"
#include "obs.h"
//#include "video_render_impl.h"

class QQuickItem;
using namespace agora::rtc;

static const char *agora_signals[] = {
	"void firstRemoteVideoDecoded(ptr service, unsigned int uid, int width, int height, int elapsed)",
	"void userJoined(ptr service, unsigned int uid, int elapsed)",
	"void userOffline(ptr service, unsigned int uid, int reason)",
	"void joinChannelSuccess(ptr output, const char* channel, unsigned int uid, int elapsed)",
	"void onError(int err, const char* msg)",
	"void onLeaveChannel()",
	NULL
};

class AgoraRtcEngine
{
   // Q_OBJECT
public:
    explicit AgoraRtcEngine();
    ~AgoraRtcEngine();
	static AgoraRtcEngine* GetInstance();
	static void ReleaseInstance();

	bool InitEngine(std::string appid);

	BOOL setLogPath(std::string path);
	BOOL setClientRole(CLIENT_ROLE_TYPE role, LPCSTR lpPermissionKey = NULL);
	int  setChannelProfile(CHANNEL_PROFILE_TYPE profile);

	int enableVideo(bool enabled);
	bool enableLocalRender(bool bEnable);
	int muteLocalAudioStream(bool muted);
	int  setLocalVideoMirrorMode(VIDEO_MIRROR_MODE_TYPE mirrorMode);
	void startPreview();
	void stopPreview();

	int joinChannel(const std::string& key, const std::string& channel, unsigned uid);
	int leaveChannel();
	
	bool  keepPreRotation(bool bRotate);
	bool  setVideoProfileEx(int nWidth, int nHeight, int nFrameRate, int nBitRate);
	bool  enableLocalCameara(bool bEnable);

	void* AgoraVideoObserver_Create();
	void  AgoraVideoObserver_Destroy(void* data);
	bool  AgoraVideoObserver_Encode(void* data, struct encoder_frame* frame,
	    struct encoder_packet* packet, bool *receive_packet);

	bool setRecordingAudioFrameParameters(int nSampleRate, int nChannels, int nSamplesPerCall);
	bool setExternalAudioSource(bool bEnabled, int nSampleRate, int nChannels);
	bool enableExtendPlayDevice(bool bEnable);

	void* AgoraAudioObserver_Create();
	void  AgoraAudioObserver_Destroy(void* data);
	bool  AgoraAudioObserver_Encode(void* data, struct encoder_frame* frame,
	    struct encoder_packet* packet, bool *receive_packet);

	int ConfigPublisher(const PublisherConfiguration& config);
	int SetVideoCompositingLayout(const VideoCompositingLayout& sei);
	int ClearVideoCompositingLayout();

	int AddPublishStreamUrl(const char *url, bool transcodingEnabled);
	int RemovePublishStreamUrl(const char *url);
	//added by wanhaiping
	int SetLivevideoTrans(int countUser, int localuid, int remoteuid);
	int SetLiveTranscoding(const LiveTranscoding &transcoding);

	int enableLoopbackRecording(bool enabled);

	int adjustRecordingSignalVolume(int volume);
	int adjustPlaybackSignalVolume(int volume);

	int EnableWebSdkInteroperability(bool enabled);
	//…Ë±∏
	int getRecordingDeviceVolume();
	int getPalyoutDeviceVolume();
	int setRecordingDeviceVolume(int volume);
	int setPalyoutDeviceVolume(int volume);
	int testMicrophone(bool start, int interval);
	int testSpeaker(bool start);

 void agora_blog(int log_level, char *info);

	obs_service_t* agoraService;
	bool bInit = false;
	int setupRemoteVideo(unsigned int uid, void* view);
    agora::rtc::IRtcEngine* getRtcEngine() {return m_rtcEngine;}

	int agora_fps = 15;
	int agora_out_cx = 640;
	int agora_out_cy = 360;
	int agora_video_bitrate = 500;

 void joinedChannelSuccess(const char* channel, unsigned int uid, int elapsed);	
	int audioChannel = 2;
	int sampleRate = 44100; 
//public slots:
private:
    friend class AgoraRtcEngineEvent;
private:
   // std::unique_ptr<agora::rtc::IRtcEngine> m_rtcEngine;
    agora::rtc::IRtcEngine* m_rtcEngine;
    std::unique_ptr<agora::rtc::IRtcEngineEventHandler> m_eventHandler;
	static char* m_appid;
	static AgoraRtcEngine* m_agoraEngine;
	
	std::unique_ptr<CExtendVideoFrameObserver> m_videoObserver;
	std::unique_ptr<CExtendAudioFrameObserver> m_audioObserver;
};

