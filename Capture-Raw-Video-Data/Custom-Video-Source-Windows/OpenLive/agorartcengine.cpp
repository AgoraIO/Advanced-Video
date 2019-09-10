#include "agorartcengine.hpp"
#include "Agora/AgoraBase.h"
#include "obs.h"

using namespace agora::rtc;
using namespace agora;

class AgoraRtcEngineEvent : public agora::rtc::IRtcEngineEventHandler
{
    AgoraRtcEngine& m_engine;
public:
    AgoraRtcEngineEvent(AgoraRtcEngine& engine)
        :m_engine(engine)
    {}
    virtual void onVideoStopped() override
    {
    // emit m_engine.videoStopped();
    }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override
    {
		struct calldata params;
		calldata_init(&params);
		calldata_set_string(&params, "channel", channel);
		calldata_set_int(&params, "uid", uid);
		calldata_set_int(&params, "elapsed", elapsed);
		signal_handler_signal(obs_service_get_signal_handler(m_engine.agoraService), "joinChannelSuccess", &params);
    }

	virtual void onLeaveChannel(const RtcStats& stats) override
	{
		struct calldata params;
		uint8_t stack[256] = { 0 };
		calldata_init_fixed(&params, stack, 256);

		calldata_set_int(&params, "duration", stats.duration);
		calldata_set_int(&params, "txBytes", stats.txBytes);
		calldata_set_int(&params, "rxBytes", stats.rxBytes);
		calldata_set_int(&params, "txKBitRate", stats.txKBitRate);
		calldata_set_int(&params, "rxKBitRate", stats.rxKBitRate);
		calldata_set_int(&params, "cpuAppUsage", stats.cpuAppUsage);
		calldata_set_int(&params, "cpuTotalUsage", stats.cpuTotalUsage);
		calldata_set_int(&params, "rxAudioKBitRate", stats.rxAudioKBitRate);
		calldata_set_int(&params, "txAudioKBitRate", stats.txAudioKBitRate);
		calldata_set_int(&params, "rxVideoKBitRate", stats.rxVideoKBitRate);
		calldata_set_int(&params, "txVideoKBitRate", stats.txVideoKBitRate);
		calldata_set_int(&params, "userCount", stats.userCount);
	}

	virtual void onError(int err, const char* msg) override
	{
		struct calldata params;
		calldata_init(&params);
		calldata_set_int(&params, "err_code", err);
		calldata_set_string(&params, "err_msg", msg);
		signal_handler_signal(obs_service_get_signal_handler(m_engine.agoraService), "onError", &params);
	}

    virtual void onUserJoined(uid_t uid, int elapsed) override
    {
		struct calldata params;
		calldata_init(&params);
		calldata_set_int(&params, "uid", uid);
		calldata_set_int(&params, "elapsed", elapsed);
		signal_handler_signal(obs_service_get_signal_handler(m_engine.agoraService), "userJoined", &params);
    }
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override
    {
		struct calldata params;
		calldata_init(&params);
		calldata_set_int(&params, "uid", uid);
		calldata_set_int(&params, "reason", reason);
		signal_handler_signal(obs_service_get_signal_handler(m_engine.agoraService), "userOffline", &params);
    }

    virtual void onFirstLocalVideoFrame(int width, int height, int elapsed) override
    {
    //   emit m_engine.firstLocalVideoFrame(width, height, elapsed);
    }
    virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) override
	{
		struct calldata params;

		calldata_init(&params);
		calldata_set_int(&params, "uid", uid);
		calldata_set_int(&params, "width", width);
		calldata_set_int(&params, "height", height);
		calldata_set_int(&params, "elapsed", elapsed);
		signal_handler_signal(obs_service_get_signal_handler(m_engine.agoraService), "firstRemoteVideoDecoded", &params);
    }
    virtual void onFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed) override
    {
     //  emit m_engine.firstRemoteVideoFrameDrawn(uid, width, height, elapsed);
    }
};

AgoraRtcEngine* AgoraRtcEngine::m_agoraEngine = nullptr;
AgoraRtcEngine* AgoraRtcEngine::GetInstance()
{
	if (m_agoraEngine == nullptr)
		m_agoraEngine = new AgoraRtcEngine;

	return m_agoraEngine;
}

void AgoraRtcEngine::ReleaseInstance()
{
	if (nullptr != m_agoraEngine){
		delete m_agoraEngine;
		m_agoraEngine = nullptr;
	}
}

AgoraRtcEngine::AgoraRtcEngine()
	: m_rtcEngine(createAgoraRtcEngine())
    , m_eventHandler(new AgoraRtcEngineEvent(*this))
{
}

AgoraRtcEngine::~AgoraRtcEngine()
{
    if(m_rtcEngine) {
        m_rtcEngine->release();
       // auto re = m_rtcEngine;
       // m_rtcEngine.release();
       // re->release();
    }
}

void AgoraRtcEngine::agora_blog(int log_level, char* info)
{
    blog(LOG_INFO, info);
}
bool AgoraRtcEngine::InitEngine(std::string appid)
{
	agora::rtc::RtcEngineContext context;
	context.eventHandler = m_eventHandler.get();
	context.appId = appid.c_str();
	struct calldata params;
	calldata_init(&params);

	if (*context.appId == '\0'){
		calldata_set_int(&params, "error", -1);//
		signal_handler_signal(obs_service_get_signal_handler(agoraService), "initRtcEngineFailed", &params);
		return false;
	}
	if (0 != m_rtcEngine->initialize(context)){
		calldata_set_int(&params, "error", -2);
		signal_handler_signal(obs_service_get_signal_handler(agoraService), "initRtcEngineFailed", &params);
		return false;
	}
	return true;
}

BOOL AgoraRtcEngine::setLogPath(std::string path)
{
	RtcEngineParameters rep(*m_rtcEngine);
	int ret = rep.setLogFile(path.c_str());
	return ret == 0 ? TRUE : FALSE;
}

BOOL AgoraRtcEngine::setClientRole(CLIENT_ROLE_TYPE role, LPCSTR lpPermissionKey)
{
	//wanhaiping_voice
	int nRet = m_rtcEngine->setClientRole(role);


	//m_nRoleType = role;

	return nRet == 0 ? TRUE : FALSE;
}

int AgoraRtcEngine::setChannelProfile(CHANNEL_PROFILE_TYPE profile)
{
	return m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
}

int AgoraRtcEngine::setLocalVideoMirrorMode(VIDEO_MIRROR_MODE_TYPE mirrorMode)
{
	RtcEngineParameters rep(*m_rtcEngine);
	return rep.setLocalVideoMirrorMode(mirrorMode);
}

bool AgoraRtcEngine::enableLocalRender(bool bEnable)
{
	int ret = -1;
	AParameter apm(*m_rtcEngine);

	if (bEnable)
		ret = apm->setParameters("{\"che.video.local.render\":true}");
	else
		ret = apm->setParameters("{\"che.video.local.render\":false}");
	apm.release();
	return ret == 0 ? true : false;
}

void AgoraRtcEngine::startPreview()
{
	m_rtcEngine->startPreview();
}

void AgoraRtcEngine::stopPreview()
{
	m_rtcEngine->stopPreview();
}

void* AgoraRtcEngine::AgoraVideoObserver_Create()
{
	agora::util::AutoPtr< agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);

	if (mediaEngine.get() == nullptr)
		return nullptr;

	m_videoObserver.reset(new CExtendVideoFrameObserver);
	if (mediaEngine->registerVideoFrameObserver(m_videoObserver.get()))
	{
		mediaEngine->release();
		return nullptr;
	}

	mediaEngine->release();
	return m_videoObserver.release();
}

void AgoraRtcEngine::AgoraVideoObserver_Destroy(void* data)
{
	CExtendVideoFrameObserver* videoObserver = static_cast<CExtendVideoFrameObserver*>(data);
	if (videoObserver != nullptr)
	{
		delete videoObserver;
		videoObserver = nullptr;
	}

	agora::util::AutoPtr< agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);

	if (mediaEngine.get() == nullptr)
		return;

	mediaEngine->registerVideoFrameObserver(nullptr);
	mediaEngine->release();
}

bool  AgoraRtcEngine::AgoraVideoObserver_Encode(void* data, struct encoder_frame* frame, struct encoder_packet* packet, bool *receive_packet)
{
	return true;
}

void* AgoraRtcEngine::AgoraAudioObserver_Create()
{
	agora::util::AutoPtr< agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);

	if (mediaEngine.get() == nullptr)
		return nullptr;

	std::unique_ptr<CExtendAudioFrameObserver> audioObserver(new CExtendAudioFrameObserver);

	if (mediaEngine->registerAudioFrameObserver(audioObserver.get()))
	{
		mediaEngine->release();
		return nullptr;
	}

	mediaEngine->release();
	return audioObserver.release();
}

void AgoraRtcEngine::AgoraAudioObserver_Destroy(void* data)
{
	CExtendAudioFrameObserver* audioObserver = static_cast<CExtendAudioFrameObserver*>(data);

	agora::util::AutoPtr< agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);

	if (mediaEngine.get() == nullptr)
		return;

	mediaEngine->registerAudioFrameObserver(0);
	mediaEngine->release();

	if (audioObserver != nullptr)
	{
		delete audioObserver;
		audioObserver = nullptr;
	}
}

bool AgoraRtcEngine::AgoraAudioObserver_Encode(void* data, struct encoder_frame* frame,
    struct encoder_packet* packet, bool *receive_packet)
{
	CExtendAudioFrameObserver* audioObserver = static_cast<CExtendAudioFrameObserver*>(data);

	if (audioObserver == nullptr)
		return false;

	//resample
	return true;
}

bool AgoraRtcEngine::enableExtendPlayDevice(bool bEnable)
{
	int ret = 0;

	AParameter apm(*m_rtcEngine);

	if (bEnable)
		ret = apm->setParameters("{\"che.audio.external_render\":true}");
	else
		ret = apm->setParameters("{\"che.audio.external_render\":false}");
	apm->release();
	return ret == 0 ? TRUE : FALSE;
}

bool AgoraRtcEngine::setExternalAudioSource(bool bEnabled, int nSampleRate, int nChannels)
{
	RtcEngineParameters rep(m_rtcEngine);

	int nRet = rep.setExternalAudioSource(bEnabled, nSampleRate, nChannels);

	return nRet == 0 ? true : false;
}

bool AgoraRtcEngine::setRecordingAudioFrameParameters(int nSampleRate, int nChannels, int nSamplesPerCall)
{
	RtcEngineParameters rep(m_rtcEngine);
	int ret = rep.setRecordingAudioFrameParameters(nSampleRate, nChannels,  RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, nSamplesPerCall);
	return ret == 0 ? true : false;
}

int AgoraRtcEngine::joinChannel(const std::string& key, const std::string& channel, unsigned int uid)
{
	
    int r = m_rtcEngine->joinChannel(key.data(), channel.data(), nullptr, uid);
//     if (!r)
//         emit joiningChannel();
    return r;
}

int AgoraRtcEngine::leaveChannel()
{
    int r = m_rtcEngine->leaveChannel();
//     if (!r)
//         emit leavingChannel();
    return r;
}

// int AgoraRtcEngine::muteLocalAudioStream(bool muted)
// {
//     RtcEngineParameters rep(*m_rtcEngine);
//     return rep.muteLocalAudioStream(muted);
// }

int AgoraRtcEngine::enableVideo(bool enabled)
{
    return enabled ? m_rtcEngine->enableVideo() : m_rtcEngine->disableVideo();
}

// int AgoraRtcEngine::setupLocalVideo(QWidget* view)
// {
// 	agora::rtc::view_t v = NULL;
// 	if (view != NULL)
// 	    v = reinterpret_cast<agora::rtc::view_t>(view->winId());
// 
// 	VideoCanvas canvas(v, RENDER_MODE_FIT, 0);
//     return m_rtcEngine->setupLocalVideo(canvas);
// }

int AgoraRtcEngine::setupRemoteVideo(unsigned int uid, void* view)
{
	agora::rtc::view_t v = reinterpret_cast<agora::rtc::view_t>(view);
	VideoCanvas canvas;// (v, RENDER_MODE_FIT, uid);
	canvas.view = v;
	canvas.renderMode = RENDER_MODE_FIT;
	canvas.uid = uid;
    return m_rtcEngine->setupRemoteVideo(canvas);
}

int AgoraRtcEngine::ConfigPublisher(const PublisherConfiguration& config)
{
	return m_rtcEngine->configPublisher(config);
}

int AgoraRtcEngine::SetVideoCompositingLayout(const VideoCompositingLayout& sei)
{
	return m_rtcEngine->setVideoCompositingLayout(sei);
}

int AgoraRtcEngine::ClearVideoCompositingLayout()
{
	return m_rtcEngine->clearVideoCompositingLayout();
}

bool AgoraRtcEngine::keepPreRotation(bool bRotate)
{
	int ret = -1;
	AParameter apm(m_rtcEngine);
	if (bRotate)
		ret = apm->setParameters("{\"che.video.keep_prerotation\":true}");
	else
		ret = apm->setParameters("{\"che.video.keep_prerotation\":false}");
	apm.release();
	return ret == 0;
}

bool AgoraRtcEngine::setVideoProfileEx(int nWidth, int nHeight, int nFrameRate, int nBitRate)
{
	IRtcEngine2 *lpRtcEngine2 = (IRtcEngine2 *)m_rtcEngine;
	int nRet = lpRtcEngine2->setVideoProfileEx(nWidth, nHeight, nFrameRate, nBitRate);

	return nRet == 0 ? true : false;
}

bool AgoraRtcEngine::enableLocalCameara(bool bEnable)
{
	AParameter apm(*m_rtcEngine);
	int ret = -1;
	if (!apm.get()) return false;

	if (!bEnable)
		ret = apm->setParameters("{\"che.video.local.camera_index\":1024}");
	else
		ret = apm->setParameters("{\"che.video.local.camera_index\":0}");

	apm.release();
	return ret == 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//agora device
///////////////////////////////////////////////////////////////////////////////////////
// QVariantMap AgoraRtcEngine::getRecordingDeviceList()
// {
//     QVariantMap devices;
//     QVariantList names, guids;
//     AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
//     if (!audioDeviceManager)
//         return devices;
// 
//     agora::util::AutoPtr<IAudioDeviceCollection> spCollection(audioDeviceManager->enumerateRecordingDevices());
//     if (!spCollection)
//         return devices;
// 	char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
//     int count = spCollection->getCount();
//     if (count > 0)
//     {
//         for (int i = 0; i < count; i++)
//         {
//             if (!spCollection->getDevice(i, name, guid))
//             {
//                 names.push_back(name);
//                 guids.push_back(guid);
//             }
//         }
//         devices.insert("name", names);
//         devices.insert("guid", guids);
//         devices.insert("length", names.length());
//     }
//     return devices;
// }

// QVariantMap AgoraRtcEngine::getPlayoutDeviceList()
// {
//     QVariantMap devices;
//     QVariantList names, guids;
//     AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
//     if (!audioDeviceManager)
//         return devices;
// 
//     agora::util::AutoPtr<IAudioDeviceCollection> spCollection(audioDeviceManager->enumeratePlaybackDevices());
//     if (!spCollection)
//         return devices;
// 	char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
//     int count = spCollection->getCount();
//     if (count > 0)
//     {
//         for (int i = 0; i < count; i++)
//         {
//             if (!spCollection->getDevice(i, name, guid))
//             {
//                 names.push_back(name);
//                 guids.push_back(guid);
//             }
//         }
//         devices.insert("name", names);
//         devices.insert("guid", guids);
//         devices.insert("length", names.length());
//     }
//     return devices;
// }
// 
// QVariantMap AgoraRtcEngine::getVideoDeviceList()
// {
//     QVariantMap devices;
//     QVariantList names, guids;
//     AVideoDeviceManager videoDeviceManager(m_rtcEngine.get());
//     if (!videoDeviceManager)
//         return devices;
// 
//     agora::util::AutoPtr<IVideoDeviceCollection> spCollection(videoDeviceManager->enumerateVideoDevices());
//     if (!spCollection)
//         return devices;
// 	char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
//     int count = spCollection->getCount();
//     if (count > 0)
//     {
//         for (int i = 0; i < count; i++)
//         {
//             if (!spCollection->getDevice(i, name, guid))
//             {
//                 names.push_back(name);
//                 guids.push_back(guid);
//             }
//         }
//         devices.insert("name", names);
//         devices.insert("guid", guids);
//         devices.insert("length", names.length());
//     }
//     return devices;
// }
// 
// int AgoraRtcEngine::setRecordingDevice(const QString& guid)
// {
//     if (guid.isEmpty())
//         return -1;
//     AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
//     if (!audioDeviceManager)
//         return -1;
//     return audioDeviceManager->setRecordingDevice(guid.toUtf8().data());
// }
// 
// int AgoraRtcEngine::setPlayoutDevice(const QString& guid)
// {
//     if (guid.isEmpty())
//         return -1;
//     AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
//     if (!audioDeviceManager)
//         return -1;
//     return audioDeviceManager->setPlaybackDevice(guid.toUtf8().data());
// }
// 
// int AgoraRtcEngine::setVideoDevice(const QString& guid)
// {
//     if (guid.isEmpty())
//         return -1;
//     AVideoDeviceManager videoDeviceManager(m_rtcEngine.get());
//     if (!videoDeviceManager)
//         return -1;
//     return videoDeviceManager->setDevice(guid.toUtf8().data());
// }

int AgoraRtcEngine::getRecordingDeviceVolume()
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine);
    if (!audioDeviceManager)
        return 0;
    int vol = 0;
    if (audioDeviceManager->getRecordingDeviceVolume(&vol) == 0)
        return vol;
    return 0;
}

int AgoraRtcEngine::getPalyoutDeviceVolume()
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine);
    if (!audioDeviceManager)
        return 0;
    int vol = 0;
    if (audioDeviceManager->getPlaybackDeviceVolume(&vol) == 0)
        return vol;
    return 0;
}

int AgoraRtcEngine::setRecordingDeviceVolume(int volume)
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine);
    if (!audioDeviceManager)
        return -1;
    return audioDeviceManager->setRecordingDeviceVolume(volume);
}

int AgoraRtcEngine::setPalyoutDeviceVolume(int volume)
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine);
    if (!audioDeviceManager)
        return -1;
    return audioDeviceManager->setPlaybackDeviceVolume(volume);
}

int AgoraRtcEngine::testMicrophone(bool start, int interval)
{
    agora::rtc::AAudioDeviceManager dm(m_rtcEngine);
    if (!dm)
        return -1;
    if (start)
        return dm->startRecordingDeviceTest(interval);
    else
        return dm->stopRecordingDeviceTest();
}

int AgoraRtcEngine::testSpeaker(bool start)
{
    agora::rtc::AAudioDeviceManager dm(m_rtcEngine);
    if (!dm)
        return -1;
    if (start)
        return dm->startPlaybackDeviceTest("audio_sample.wav");
    else
        return dm->stopPlaybackDeviceTest();
}
// 
// int AgoraRtcEngine::testCamera(bool start, QWidget* view)
// {
//     agora::rtc::AVideoDeviceManager dm(m_rtcEngine);
//     if (!dm)
//         return -1;
// 
//     if (start)
//     {
// 		agora::rtc::view_t v = reinterpret_cast<agora::rtc::view_t>(view->winId());
//         return dm->startDeviceTest(v);
//     }
//     else
//         return dm->stopDeviceTest();
// }

void AgoraRtcEngine::joinedChannelSuccess(const char* channel, unsigned int uid, int elapsed)
{
	
}

int AgoraRtcEngine::AddPublishStreamUrl(const char *url, bool transcodingEnabled)
{
	return m_rtcEngine->addPublishStreamUrl(url, transcodingEnabled);
}
int AgoraRtcEngine::RemovePublishStreamUrl(const char *url)
{
	return m_rtcEngine->removePublishStreamUrl(url);
}



//added by wanhaiping
#define  LIVE_TRANSCODING_WIDTH     360
#define  LIVE_TRANSCODING_HEIGHT    640

int AgoraRtcEngine::SetLivevideoTrans(int countUser, int localuid, int remoteuid)
{
	int user_count = countUser;
	if (user_count<= 0 || countUser > 2)
		return 0;
	
	LiveTranscoding config;
	config.audioSampleRate = AUDIO_SAMPLE_RATE_48000;
	config.audioChannels = /*liveTranscoding.audioChannels*/ 2;
	
	config.videoGop = 30;
	config.userCount = countUser;
	config.watermark = nullptr;
	config.videoCodecProfile = VIDEO_CODEC_PROFILE_HIGH;
	config.transcodingUsers = new TranscodingUser[config.userCount];
	config.lowLatency = true;

	
	if (countUser == 1)
	{
		config.width = LIVE_TRANSCODING_WIDTH;
		config.height = LIVE_TRANSCODING_HEIGHT;
		config.videoFramerate = 15;
		config.videoBitrate = 900;

		config.transcodingUsers[0].x = 0;
		config.transcodingUsers[0].y = 0;
		config.transcodingUsers[0].width = LIVE_TRANSCODING_WIDTH;
		config.transcodingUsers[0].height = LIVE_TRANSCODING_HEIGHT;
		config.transcodingUsers[0].zOrder = 1;
		config.transcodingUsers[0].uid = localuid;

	}
	else if (countUser == 2)
	{


		/*config.width = LIVE_TRANSCODING_WIDTH*2;
		config.height = LIVE_TRANSCODING_HEIGHT*2;
		config.videoFramerate = 15;
		config.videoBitrate = 1500;
		config.backgroundColor = 0x48073D;


		config.transcodingUsers[0].x = 0;
		config.transcodingUsers[0].y = (LIVE_TRANSCODING_HEIGHT) /2;
		config.transcodingUsers[0].width = LIVE_TRANSCODING_WIDTH;
		config.transcodingUsers[0].height = LIVE_TRANSCODING_HEIGHT;
		config.transcodingUsers[0].zOrder = 1;
		config.transcodingUsers[0].uid = localuid;
		

		config.transcodingUsers[1].x = LIVE_TRANSCODING_WIDTH;
		config.transcodingUsers[1].y = (LIVE_TRANSCODING_HEIGHT) / 2;
		config.transcodingUsers[1].width = LIVE_TRANSCODING_WIDTH;
		config.transcodingUsers[1].height = LIVE_TRANSCODING_HEIGHT;
		config.transcodingUsers[1].zOrder = 1;
		config.transcodingUsers[1].uid = remoteuid;*/

		config.width = 640;
		config.height = 480;
		config.videoFramerate = 15;
		config.videoBitrate = 1500;
		config.backgroundColor = 0x48073D;


		config.transcodingUsers[0].x = 0;
		config.transcodingUsers[0].y = 0;
		config.transcodingUsers[0].width = 320;
		config.transcodingUsers[0].height = 480;
		config.transcodingUsers[0].zOrder = 1;
		config.transcodingUsers[0].uid = localuid;


		config.transcodingUsers[1].x = 320;
		config.transcodingUsers[1].y = 0;
		config.transcodingUsers[1].width = 320;
		config.transcodingUsers[1].height = 480;
		config.transcodingUsers[1].zOrder = 1;
		config.transcodingUsers[1].uid = remoteuid;
		
		  
	}

	config.backgroundColor = 0x48073D;
	int ret = SetLiveTranscoding(config);

	if (config.transcodingUsers)
	{
		delete[] config.transcodingUsers;
	}

	return 0;
}


int AgoraRtcEngine::SetLiveTranscoding(const LiveTranscoding &transcoding)
{
	return m_rtcEngine->setLiveTranscoding(transcoding);
}


int AgoraRtcEngine::EnableWebSdkInteroperability(bool enabled)
{
	RtcEngineParameters rep(*m_rtcEngine);
	return rep.enableWebSdkInteroperability(true);
}


//wanhaiping_voice
int AgoraRtcEngine::enableLoopbackRecording(bool enabled)
{
	RtcEngineParameters rep(*m_rtcEngine);
	return rep.enableLoopbackRecording(true);
}


int AgoraRtcEngine::adjustRecordingSignalVolume(int volume)
{
	RtcEngineParameters rep(*m_rtcEngine);
	return  rep.adjustRecordingSignalVolume(volume);
}


int AgoraRtcEngine::adjustPlaybackSignalVolume(int volume)
{
	RtcEngineParameters rep(*m_rtcEngine);
	return  rep.adjustPlaybackSignalVolume(volume);
}
