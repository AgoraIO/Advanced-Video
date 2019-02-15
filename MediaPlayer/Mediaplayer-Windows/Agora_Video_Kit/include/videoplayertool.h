


#ifndef VIDEOPLAYERTOOL_H
#define VIDEOPLAYERTOOL_H

#include <iostream>

enum VIDEO_STATE
{
	VIDEO_START = 0,
	VIDEO_ERROR = 1,
	VIDEO_EXIT = 2,
	VIDEO_COMPLETE = 3,
	VIDEO_PREPARE = 4,
	VIDEO_PAUSE = 5,
	VIDEO_PLAY = 6,
	VIDEO_INVALID_ARGUMENT = 7,
	VIDEO_FILE_NULL = 8,
	VIDEO_UNSUPPORT_FORMATE = 9
};

typedef void(*video_callback)(int linsize[], void *data, int w, int h);
typedef void(*audio_callBack)(void *data, int bytes_len);
typedef void(*audioinfo_callback)(int sample_rate, int channels, long duration,int width,int height);
typedef void(*videostate_callback)(VIDEO_STATE state);

class  VideoPlayerTool
{
public:
    ~VideoPlayerTool(){

    }
    VideoPlayerTool();

	void setWinID(void *data);
    //playOrPause
    void video_play_pause();
    //quit
    void video_quit();
    //receive video positon
    int  getVideoCurrentPos();
    //set video timestamp
    void video_seek(long msec);
    //video quik or slow
    void video_seek_step();
    //load video
    void load_video(char *url,bool isAutoPlay);
	void setVideoCallBack(video_callback fn);
	void setAudioCallBack(audio_callBack fn);
	void setAudioInfoCallBack(audioinfo_callback fn);
	void setVideoStateCallBack(videostate_callback fn);
};

#endif // VIDEOPLAYERTOOL_H
