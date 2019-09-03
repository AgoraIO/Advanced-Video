#include "QTVideoKit.h"
#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>
class QTVideoHandler:public MediaPlayerKitEventHandler
{
public:
	QTVideoHandler(){};
	~QTVideoHandler(){};
	virtual void onPlayerStateChanged(AG_MEDIA_PLAYER_STATE state) {
		printf("playerstate == %d", state);
	}
	virtual void onKitError(AG_KIT_ERROR error) {
		printf("kitError == %d", error);

	}

private:
	 
};
class QTVideoInfoCallback :public MediaInfoCallback
{
public:
	virtual void onAudioTrackInfoCallBack(int index, int sample_rate, int channels, long duration) {
		printf("index == %d,samplerate_rate = %d,channels == %d duration == %d \n",index,sample_rate,channels,duration);
	}
	virtual void onVideoTrackInfoCallback(int index, int width, int height) {
		printf("index == %d width == %d,height == %d \n", index, width, height);
	}
};

void QTVideoKit::loadVideo(QString url,bool isAutoPlay){
    QTVideoHandler *handler = new QTVideoHandler();
	QTVideoInfoCallback *infoCallback = new QTVideoInfoCallback();
	mediaPlayerKit->setEventHandler(handler);
	mediaPlayerKit->setMediaInfoCallback(infoCallback);
	mediaPlayerKit->load(url.toUtf8().data(),true);
	
}
void QTVideoKit::unload(){

	mediaPlayerKit->unload();

}
void QTVideoKit::pause(){

	mediaPlayerKit->pause();
}
void QTVideoKit::seekTo(int msc){
	float  aspt = msc / (float)1000;
	long msec = aspt * (double)mediaPlayerKit->getDuration();
	mediaPlayerKit->seekTo(msec);
}
int QTVideoKit::getCurrentPos(){
	if (mediaPlayerKit == NULL || mediaPlayerKit->getDuration() == 0)
	{
		return 0;
	}
	double ret =(double)mediaPlayerKit->getCurrentPosition();
	int pos = (ret / (double)mediaPlayerKit->getDuration()) * 1000;

    return pos;
}
void QTVideoKit::setAudioMix(bool isAudioMix){
	if (isAudioMix)
	{
		mediaPlayerKit->play();
	}else{
		mediaPlayerKit->stop();
	}

}
void QTVideoKit::publish() {
	mediaPlayerKit->publishAudio();
	mediaPlayerKit->publishVideo();
}
void QTVideoKit::unPublish(){
	mediaPlayerKit->unpublishAudio();
	mediaPlayerKit->unpublishVideo();
}
void QTVideoKit::setPreview(void *hwnd)
{
	mediaPlayerKit->setVideoView(hwnd);
}
void QTVideoKit::createQTVideoPlayerKit(agora::rtc::IRtcEngine *rtcEngine, int sampleRate)
{
	if (rtcEngine) {
		agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
		mediaEngine.queryInterface(rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
		mediaEngine->setExternalVideoSource(true, false);
	}
     //mediaPlayerKit = createMediaPlayerKitWithRtcEngine(rtcEngine,sampleRate);
    mediaPlayerKit = createMediaPlayerKit();
}