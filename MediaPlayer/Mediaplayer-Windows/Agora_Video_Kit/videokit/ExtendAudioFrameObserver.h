#pragma once
#include <IAgoraMediaEngine.h>
#include <iostream>
#include <mutex>
using namespace std;
class CExtendAudioFrameObserver:public agora::media::IAudioFrameObserver
{
public:

	CExtendAudioFrameObserver();
	~CExtendAudioFrameObserver();
	static CExtendAudioFrameObserver  *Get(){
		static CExtendAudioFrameObserver ar;
		return &ar;
	}
	virtual bool onRecordAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame);
	virtual bool onMixedAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame);
public:
	int sample_rate;
	int re_sample_rate;
	int re_channels;
	void pushAudioData(void *data, int len);
	void reset();
    void setAudioMixing(bool isAudioMix);
	std::mutex mtx;

};

