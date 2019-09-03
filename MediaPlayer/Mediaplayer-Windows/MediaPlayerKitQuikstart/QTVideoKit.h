#pragma once
#include "MediaPlayerKit.h"
#include <IAgoraRtcEngine.h>
#include <QObject>
#include  "SingleTon.h"
#include <mutex>
class QTVideoKit:public QObject,public Singleton<QTVideoKit>
{
	Q_OBJECT
	friend Singleton;
	QTVideoKit(){

	}
	 ~QTVideoKit()
	{

    }
public:
	void createQTVideoPlayerKit(agora::rtc::IRtcEngine *rtcEngine, int sampleRate);
    void loadVideo(QString url,bool isAutoPlay);
    void pause();
	void unload();
    void seekTo(int msc);
    int  getCurrentPos();
    void setAudioMix(bool isAudioMix);
	void publish();
	void unPublish();
	void setPreview(void *hwnd);
private:
	std::mutex m_mutex;
	MediaPlayerKit *mediaPlayerKit;
};

