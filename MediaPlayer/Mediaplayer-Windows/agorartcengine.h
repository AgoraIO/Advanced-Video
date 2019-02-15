#ifndef AGORARTCENGINE_H
#define AGORARTCENGINE_H
#include <memory>
#include <QString>
#include <QVariant>
#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>
#include "Agora_Video_Kit/videokit/videokit.h"

class QQuickItem;

class AgoraRtcEngine : public QObject, public agora::media::IExternalVideoRenderFactory
{
    Q_OBJECT
public:
    explicit AgoraRtcEngine(QObject *parent = 0);
    ~AgoraRtcEngine();
    virtual agora::media::IExternalVideoRender* createRenderInstance(const agora::media::ExternalVideoRenerContext& context) override;
    Q_INVOKABLE int joinChannel(const QString& key, const QString& channel, int uid);
    Q_INVOKABLE int leaveChannel();
    Q_INVOKABLE int muteLocalAudioStream(bool muted);
    Q_INVOKABLE int enableVideo(bool enabled);
    Q_INVOKABLE QVariantMap getRecordingDeviceList();
    Q_INVOKABLE QVariantMap getPlayoutDeviceList();
    Q_INVOKABLE QVariantMap getVideoDeviceList();
    Q_INVOKABLE int setRecordingDevice(const QString& guid);
    Q_INVOKABLE int setPlayoutDevice(const QString& guid);
    Q_INVOKABLE int setVideoDevice(const QString& guid);
    Q_INVOKABLE int getRecordingDeviceVolume();
    Q_INVOKABLE int getPalyoutDeviceVolume();
    Q_INVOKABLE int setRecordingDeviceVolume(int volume);
    Q_INVOKABLE int setPalyoutDeviceVolume(int volume);
    Q_INVOKABLE int testMicrophone(bool start, int interval);
    Q_INVOKABLE int testSpeaker(bool start);
    Q_INVOKABLE int testCamera(bool start, void* view);
     int setupLocalVideo(void* view);
     int setupRemoteVideo(unsigned int uid, void* view);
    agora::rtc::IRtcEngine* getRtcEngine() {return m_rtcEngine.get();}
signals:
    void joiningChannel();
    void leavingChannel();
    void videoStopped();
    void joinedChannelSuccess(const QString& channel, unsigned int uid, int elapsed);
    void userJoined(unsigned int uid, int elapsed);
    void userOffline(unsigned int uid, int reason);
    void firstLocalVideoFrame(int width, int height, int elapsed);
    void firstRemoteVideoDecoded(unsigned int uid, int width, int height, int elapsed);
    void firstRemoteVideoFrameDrawn(unsigned int uid, int width, int height, int elapsed);
public slots:
private:
    friend class AgoraRtcEngineEvent;
private:
    std::unique_ptr<agora::rtc::IRtcEngine> m_rtcEngine;
    std::unique_ptr<agora::rtc::IRtcEngineEventHandler> m_eventHandler;
    VideoKit videoKit;
};

#endif // AGORARTCENGINE_H
