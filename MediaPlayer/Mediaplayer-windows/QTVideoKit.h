#pragma once
#include "Agora_native_sdk/videokit/videokit.h"
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
};

