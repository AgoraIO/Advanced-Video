#ifndef VIDEOINFOCALLBACK_H
#define VIDEOINFOCALLBACK_H
enum FF_VIDEO_STATE
{
	FF_VIDEO_START = 0,
	FF_VIDEO_ERROR = 1,
	FF_VIDEO_EXIT = 2,
	FF_VIDEO_COMPLETE = 3,
	FF_VIDEO_PREPARE = 4,
	FF_VIDEO_PAUSE = 5,
	FF_VIDEO_PLAY = 6,
	FF_VIDEO_INVALID_ARGUMENT = 7,
    FF_VIDEO_FILE_NULL = 8,
    FF_VIDEO_UNSUPPORT_FORMATE = 9
};

class VideoInfoCallBack{
public:
    VideoInfoCallBack();
	~VideoInfoCallBack(){

	}
    virtual void onVideoDataCallBack(int linesize[],void *data ,int w,int h) = 0;
    virtual void onAudioDataCallBack(void *data,int len) = 0;
    virtual void onAudioInfoCallBack(int sample_rate,int channles,long duation,int width,int height) = 0;
	virtual void onVideoStateCallBack(FF_VIDEO_STATE state) = 0;
};

#endif // VIDEOINFOCALLBACK_H
