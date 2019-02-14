#include "ExtendAudioFrameObserver.h"
#include "external_resampler.h"
#include "AudioCircularBuffer.h"
#include "scoped_ptr.h"
#include <stdio.h>
using namespace AgoraRTC;
static scoped_ptr<AudioCircularBuffer<char>> agoraAudioBuf(new AudioCircularBuffer<char>(2048, true));
static external_resampler *resample = new external_resampler();
static int audioMixVolume = 1;
CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{

}
CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
	
	delete resample;
	agoraAudioBuf.release();
}
bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame){

    int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
    int16_t *tmpBuf = (int16_t *)malloc(sizeof(int16_t)*bytes);
    memcpy(tmpBuf, audioFrame.buffer, bytes);

    if (agoraAudioBuf->mAvailSamples < bytes) {
        memcpy(audioFrame.buffer, tmpBuf, sizeof(int16_t)*bytes);
        free(tmpBuf);
        return true;
    }
    mtx.lock();
    int mv_samplerate = CExtendAudioFrameObserver::Get()->re_sample_rate == 11025 ? 48000 : CExtendAudioFrameObserver::Get()->re_sample_rate;
    int mv_channels = CExtendAudioFrameObserver::Get()->re_channels > 2 ? 2 : CExtendAudioFrameObserver::Get()->re_channels;
	if (CExtendAudioFrameObserver::Get()->re_sample_rate == 11025 && CExtendAudioFrameObserver::Get()->re_channels == 1)
	{
		mv_channels = 2;
	}
    int mv_size = mv_samplerate * 0.01 * 2 * mv_channels;
    int ret = agoraAudioBuf->mAvailSamples - mv_size;
    if (ret < 0){
        memcpy(audioFrame.buffer, tmpBuf, sizeof(int16_t)*bytes);
        free(tmpBuf);
        mtx.unlock();
        return true;
    }
    char *data = (char *)malloc(sizeof(char)*mv_size);
    //FILE *fp = NULL;
    agoraAudioBuf->Pop(data, mv_size);
    //fp=fopen("C:/Users/zhanxiaochao/Desktop/out.pcm","ab+");

  //  if(fp){
  //      fwrite(data,1,mv_size,fp);
		//fclose(fp);
  //  }
    int16_t *buf = (int16_t *)malloc(bytes);
	resample->do_resample((int16_t *)data, mv_samplerate * 0.01, mv_channels, mv_samplerate, buf, audioFrame.samples, audioFrame.channels, this->sample_rate);
    int16_t* p16 = (int16_t*)buf;
    int16_t *audioBuf = (int16_t *)malloc(sizeof(int16_t)*bytes);
    memcpy(audioBuf, tmpBuf, bytes);
    for (int i = 0; i < bytes / 2; ++i) {
        int tmp = p16[i] * 1;
        audioBuf[i] = audioBuf[i] * audioMixVolume;
        tmp += audioBuf[i];

        if (tmp > 32767) {
            audioBuf[i] = 32767;
        }
        else if (tmp < -32768) {
            audioBuf[i] = -32768;
        }
        else {
            audioBuf[i] = tmp;
        }
    }
    memcpy(audioFrame.buffer, audioBuf, sizeof(int16_t) * bytes);
    free(audioBuf);
    free(tmpBuf);
    free(data);
    free(p16);
    mtx.unlock();
    return true;
}
bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame){
    return true;
}
bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame){

    return true;
}
bool  CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame){
    return true;
}
void CExtendAudioFrameObserver::pushAudioData(void *data,int len){
    mtx.lock();
    agoraAudioBuf->Push((char *)data, len);
    mtx.unlock();
}
void CExtendAudioFrameObserver::reset(){
    agoraAudioBuf.release();
    agoraAudioBuf.reset(new AudioCircularBuffer<char>(2048, true));
}
void CExtendAudioFrameObserver::setAudioMixing(bool isAudioMix){
    if(isAudioMix){
        audioMixVolume = 0;
    }else{
        audioMixVolume = 1;
    }
}
