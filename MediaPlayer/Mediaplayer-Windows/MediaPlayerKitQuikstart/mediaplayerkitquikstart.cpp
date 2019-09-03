#include "agorartcengine.h"
#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include "QTVideoKit.h"
using namespace agora::rtc;

class AgoraRtcEngineEvent : public agora::rtc::IRtcEngineEventHandler
{
    AgoraRtcEngine& m_engine;
public:
    AgoraRtcEngineEvent(AgoraRtcEngine& engine)
        :m_engine(engine)
    {}
    virtual void onVideoStopped() override
    {
        emit m_engine.videoStopped();
    }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override
    {
        emit m_engine.joinedChannelSuccess(channel, uid, elapsed);
    }
    virtual void onUserJoined(uid_t uid, int elapsed) override
    {
        emit m_engine.userJoined(uid, elapsed);
    }
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override
    {
        emit m_engine.userOffline(uid, reason);
    }
    virtual void onFirstLocalVideoFrame(int width, int height, int elapsed) override
    {
        emit m_engine.firstLocalVideoFrame(width, height, elapsed);
    }
    virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) override
    {
        emit m_engine.firstRemoteVideoDecoded(uid, width, height, elapsed);
    }
    virtual void onFirstRemoteVideoFrame(uid_t uid, int width, int height, int elapsed) override
    {
        emit m_engine.firstRemoteVideoFrameDrawn(uid, width, height, elapsed);
    }
};

agora::media::IExternalVideoRender* AgoraRtcEngine::createRenderInstance(const agora::media::ExternalVideoRenerContext& context)
{
    if (!context.view)
        return nullptr;
    return nullptr;
}

AgoraRtcEngine::AgoraRtcEngine(QObject *parent) : QObject(parent)
  ,m_rtcEngine(createAgoraRtcEngine())
  ,m_eventHandler(new AgoraRtcEngineEvent(*this))
{
    agora::rtc::RtcEngineContext context;
    context.eventHandler = m_eventHandler.get();
    context.appId = YOUR_APPID;//Specify your APP ID here
    if (*context.appId == '\0')
    {
        QMessageBox::critical(nullptr, tr("Agora QT Demo"),
                                       tr("You must specify APP ID before using the demo"));
    }

    m_rtcEngine->initialize(context);
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(m_rtcEngine.get(),agora:: AGORA_IID_MEDIA_ENGINE);
	RtcEngineParameters ap(*m_rtcEngine);
	ap.setLocalVideoMirrorMode(VIDEO_MIRROR_MODE_DISABLED);
	m_rtcEngine->enableVideo();
	

	//register mediaEngine
	//m_rtcEngine->startPreview();
	QTVideoKit::Get()->createQTVideoPlayerKit(m_rtcEngine.get(), 44100);
}

AgoraRtcEngine::~AgoraRtcEngine()
{
    if(m_rtcEngine.get()) {
        auto re = m_rtcEngine.get();

        m_rtcEngine.release();
        re->release();
    }
}

int AgoraRtcEngine::joinChannel(const QString& key, const QString& channel, int uid)
{
    RtcEngineParameters rep(*m_rtcEngine);
    rep.setLogFile("sdk.log");
    //m_rtcEngine->enableVideo();
    int r = m_rtcEngine->joinChannel(key.toUtf8().data(), channel.toUtf8().data(), nullptr, uid);
	//int r = 0;
    if (!r)
        emit joiningChannel();
    return r;
}

int AgoraRtcEngine::leaveChannel()
{

    int r = m_rtcEngine->leaveChannel();
    if (!r)
        emit leavingChannel();

    return r;
}

int AgoraRtcEngine::muteLocalAudioStream(bool muted)
{
    RtcEngineParameters rep(*m_rtcEngine);
    return rep.muteLocalAudioStream(muted);
}

int AgoraRtcEngine::enableVideo(bool enabled)
{
    return enabled ? m_rtcEngine->enableVideo() : m_rtcEngine->disableVideo();
}

int AgoraRtcEngine::setupLocalVideo(void * view)
{
	
    agora::rtc::view_t v = reinterpret_cast<agora::rtc::view_t>(view);
//    qDebug() << "setup local video " << view;
    VideoCanvas canvas(v, RENDER_MODE_FIT, 0);
    return m_rtcEngine->setupLocalVideo(canvas);
}

int AgoraRtcEngine::setupRemoteVideo(unsigned int uid, void* view)
{
//    qDebug() << "setup remote video " << view << " for user " << uid;
    agora::rtc::view_t v = reinterpret_cast<agora::rtc::view_t>(view);
    VideoCanvas canvas(v, RENDER_MODE_FIT, uid);
    return m_rtcEngine->setupRemoteVideo(canvas);
}

QVariantMap AgoraRtcEngine::getRecordingDeviceList()
{
    QVariantMap devices;
    QVariantList names, guids;
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return devices;

    agora::util::AutoPtr<IAudioDeviceCollection> spCollection(audioDeviceManager->enumerateRecordingDevices());
    if (!spCollection)
        return devices;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (!spCollection->getDevice(i, name, guid))
            {
                qDebug()<<"name:"<<name<<",guid:"<<guid<<".len="<<names.length()<<endl;
                names.push_back(name);
                guids.push_back(guid);
                qDebug()<<"len="<<names.length()<<endl;
            }
        }
    }
    return devices;
}

QVariantMap AgoraRtcEngine::getPlayoutDeviceList()
{
    QVariantMap devices;
    QVariantList names, guids;
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return devices;

    agora::util::AutoPtr<IAudioDeviceCollection> spCollection(audioDeviceManager->enumeratePlaybackDevices());
    if (!spCollection)
        return devices;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (!spCollection->getDevice(i, name, guid))
            {
                names.push_back(name);
                guids.push_back(guid);
            }
        }
    }
    return devices;
}

QVariantMap AgoraRtcEngine::getVideoDeviceList()
{
    QVariantMap devices;
    QVariantList names, guids;
    AVideoDeviceManager videoDeviceManager(m_rtcEngine.get());
    if (!videoDeviceManager)
        return devices;

    agora::util::AutoPtr<IVideoDeviceCollection> spCollection(videoDeviceManager->enumerateVideoDevices());
    if (!spCollection)
        return devices;
    char name[MAX_DEVICE_ID_LENGTH], guid[MAX_DEVICE_ID_LENGTH];
    int count = spCollection->getCount();
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (!spCollection->getDevice(i, name, guid))
            {
                names.push_back(name);
                guids.push_back(guid);
            }
        }
    }
    return devices;
}

int AgoraRtcEngine::setRecordingDevice(const QString& guid)
{
    if (guid.isEmpty())
        return -1;
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return -1;
    return audioDeviceManager->setRecordingDevice(guid.toUtf8().data());
}

int AgoraRtcEngine::setPlayoutDevice(const QString& guid)
{
    if (guid.isEmpty())
        return -1;
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return -1;
    return audioDeviceManager->setPlaybackDevice(guid.toUtf8().data());
}

int AgoraRtcEngine::setVideoDevice(const QString& guid)
{
    if (guid.isEmpty())
        return -1;
    AVideoDeviceManager videoDeviceManager(m_rtcEngine.get());
    if (!videoDeviceManager)
        return -1;
    return videoDeviceManager->setDevice(guid.toUtf8().data());
}

int AgoraRtcEngine::getRecordingDeviceVolume()
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return 0;
    int vol = 0;
    if (audioDeviceManager->getRecordingDeviceVolume(&vol) == 0)
        return vol;
    return 0;
}

int AgoraRtcEngine::getPalyoutDeviceVolume()
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return 0;
    int vol = 0;
    if (audioDeviceManager->getPlaybackDeviceVolume(&vol) == 0)
        return vol;
    return 0;
}

int AgoraRtcEngine::setRecordingDeviceVolume(int volume)
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return -1;
    return audioDeviceManager->setRecordingDeviceVolume(volume);
}

int AgoraRtcEngine::setPalyoutDeviceVolume(int volume)
{
    AAudioDeviceManager audioDeviceManager(m_rtcEngine.get());
    if (!audioDeviceManager)
        return -1;
    return audioDeviceManager->setPlaybackDeviceVolume(volume);
}

int AgoraRtcEngine::testMicrophone(bool start, int interval)
{
    agora::rtc::AAudioDeviceManager dm(m_rtcEngine.get());
    if (!dm)
        return -1;
    if (start)
        return dm->startRecordingDeviceTest(interval);
    else
        return dm->stopRecordingDeviceTest();
}

int AgoraRtcEngine::testSpeaker(bool start)
{
    agora::rtc::AAudioDeviceManager dm(m_rtcEngine.get());
    if (!dm)
        return -1;
    if (start)
        return dm->startPlaybackDeviceTest("audio_sample.wav");
    else
        return dm->stopPlaybackDeviceTest();
}

int AgoraRtcEngine::testCamera(bool start, void* view)
{
    agora::rtc::AVideoDeviceManager dm(m_rtcEngine.get());
    if (!dm)
        return -1;

    if (start)
    {
        agora::rtc::view_t v = reinterpret_cast<agora::rtc::view_t>(view);
        return dm->startDeviceTest(v);
    }
    else
        return dm->stopDeviceTest();
}
