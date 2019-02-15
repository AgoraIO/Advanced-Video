#include "QTVideoKit.h"
#include "videokit.h"
class QTVideoHandler:public VideoPlayerKitEventHandler
{
public:
	QTVideoHandler(){};
	~QTVideoHandler(){};

	virtual void onVideoLoaded(FF_LOAD_VIDEO_STATE state){

    };
    virtual void onKitError(){

    };
    virtual void onPlayerStopped(){

    };
    virtual void onPlayerSate(FF_PLAYER_STATE playerState){

    };
private:
	 
};

void QTVideoKit::loadVideo(QString url,bool isAutoPlay){
    QTVideoHandler *handler = new QTVideoHandler();

	VideoKit::Get()->setHandler(handler);
	VideoKit::Get()->loadVideo(url.toUtf8().data(),isAutoPlay);
	delete handler;
}
void QTVideoKit::unload(){

	VideoKit::Get()->unload();
}
void QTVideoKit::pause(){

	VideoKit::Get()->pause();
}
void QTVideoKit::seekTo(int msc){
	float  aspt = msc / (float)1000;
	long msec = aspt * VideoKit::Get()->duration;
    VideoKit::Get()->seekTo(msec);
}
int QTVideoKit::getCurrentPos(){
	double ret =(double) VideoKit::Get()->getCurrentPosition();
	int pos = (ret / (double)VideoKit::Get()->duration ) * 1000;

    return pos;
}
void QTVideoKit::setAudioMix(bool isAudioMix){
	VideoKit::Get()->setAudioMixing(isAudioMix);

}
void QTVideoKit::publish() {
	VideoKit::Get()->publish();
}
void QTVideoKit::unPublish(){
	VideoKit::Get()->unpublish();
}
void QTVideoKit::setPreview(void *hwnd)
{
	VideoKit::Get()->setPreviewWindow(hwnd);
}