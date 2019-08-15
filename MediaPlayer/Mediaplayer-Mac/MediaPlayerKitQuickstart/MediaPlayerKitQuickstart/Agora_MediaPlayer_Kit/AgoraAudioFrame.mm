//
//  AgoraAudioFrame.cpp
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2018/3/9.
//  Copyright © 2018 agora. All rights reserved.
//

#import "AgoraAudioFrame.h"
#include <stdio.h>
#include <iostream>
#include <queue>
#include <AgoraRtcEngineKit/IAgoraRtcEngine.h>
#include <AgoraRtcEngineKit/IAgoraMediaEngine.h>

#import <mach/mach_time.h>
#import "AudioCircularBuffer.h"
#import <MediaPlayerKit/external_resampler.h>
using namespace AgoraRTC;
static scoped_ptr<AudioCircularBuffer<char>> agoraAudioBuf(new AudioCircularBuffer<char>(2048,true));

//static scoped_ptr<AudioCircularBuffer<char>> earBackBuf(new AudioCircularBuffer<char>(2048,true));
static external_resampler *resample = new external_resampler();
static external_resampler *capture_resample =  new external_resampler();
static NSObject *lock = [[NSObject alloc] init];
static NSObject *threadLockCapture = [[NSObject alloc] init];
using namespace std;

static float *stereoBuffer;
//static float *effectBuffer;

static float audio_sonSum = 1;
static float audio_voiceSum = 1;

class AgoraAudioFrameObserver:public agora::media::IAudioFrameObserver
{
    
public:
    
    int sampleRate = 0;
    int sampleRate_play = 0;
    int isOpenAudioEffect;
    float voiceSum;
    float songSum;
    std::mutex mtx;
public:
#pragma mark- <C++ Capture>
    // push audio data to special buffer(Array byteBuffer)
    // bytesLength = date length
    void pushExternalData(void* data, long bytesLength)
    {
        @synchronized(threadLockCapture) {
            mtx.lock();
            char *buf = (char *)data;
            agoraAudioBuf->Push(buf, (int)bytesLength);
            mtx.unlock();
        }
        
        
    }
    virtual bool onRecordAudioFrame(AudioFrame& audioFrame) override
    {
        @synchronized(threadLockCapture) {
            
            int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
            int16_t *tmpBuf = (int16_t *)malloc(sizeof(int16_t)*bytes);
            memcpy(tmpBuf, audioFrame.buffer, bytes);
            
            if (agoraAudioBuf->mAvailSamples < bytes) {
                memcpy(audioFrame.buffer, tmpBuf, sizeof(int16_t)*bytes);
                free(tmpBuf);
                return true;
            }
            
            mtx.lock();
            
            int mv_samplerate = (int)[AgoraAudioFrame shareInstance].re_sampleRate;
            int mv_channels = (int)[AgoraAudioFrame shareInstance].re_channels;
            
            int mv_size = mv_samplerate * 0.01 * 2 * mv_channels;
            int ret = agoraAudioBuf->mAvailSamples - mv_size;
            if(ret < 0){
                mtx.unlock();
                return true;
            }
            char *data = (char *)malloc(sizeof(char)*mv_size);
            agoraAudioBuf->Pop(data, mv_size);
            int16_t *buf = (int16_t *)malloc(bytes);
            resample->do_resample((int16_t *)data, mv_samplerate * 0.01 , mv_channels,mv_samplerate, buf, audioFrame.samples, audioFrame.channels, (int)[AgoraAudioFrame shareInstance].sampleRate);
            int16_t* p16 = (int16_t*) buf;
            int16_t *audioBuf = (int16_t *)malloc(sizeof(int16_t)*bytes);
            memcpy(audioBuf, tmpBuf, bytes);
            for (int i = 0; i < bytes / 2; ++i) {
                int tmp = p16[i] * audio_sonSum;
                audioBuf[i] = audioBuf[i] * audio_voiceSum;
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
            memcpy(audioFrame.buffer, audioBuf,sizeof(int16_t) * bytes);
            free(audioBuf);
            free(tmpBuf);
            free(data);
            free(p16);
            mtx.unlock();
            
        }
        return true;
        
    }
    virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame) override{
        
        return true;
    }
    virtual  bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) override {
        
        return true;
    }
    virtual bool onMixedAudioFrame(AudioFrame& audioFrame) override {
        
        
        return true; }
    
};
@interface AgoraAudioFrame()
{
    AgoraRtcEngineKit *_rtcEngine;
}
@end
static AgoraAudioFrameObserver* s_audioFrameObserver;

@implementation AgoraAudioFrame

+ (instancetype)shareInstance{
    
    static dispatch_once_t once;
    static AgoraAudioFrame *sharedInstance;
    dispatch_once(&once, ^{
        if (sharedInstance == NULL) {
            
            sharedInstance = [[self alloc] init];
        }
    });
    return sharedInstance;
    
}

-(void)registerEngineKit:(AgoraRtcEngineKit *)rtcEngine
{
    _rtcEngine = rtcEngine;
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)rtcEngine.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {
        s_audioFrameObserver = new AgoraAudioFrameObserver();
        s_audioFrameObserver->sampleRate = (int)self.sampleRate;
        s_audioFrameObserver->sampleRate_play = (int)self.sampleRate;
        mediaEngine->registerAudioFrameObserver(s_audioFrameObserver);
        
    }
    
}
-(void)pushAudioSource:(void *)data byteLength:(long)bytesLength{
    s_audioFrameObserver->pushExternalData(data, bytesLength);
}

-(void)setIsAudience:(BOOL)isAudience
{
    _isAudience = isAudience;
    
    
}
-(void)destroyAudioBuf{
    
    agoraAudioBuf.reset(new AudioCircularBuffer<char>(2048,true));
}
-(void)setSongNum:(float)songNum
{
    _songNum = songNum;
    audio_sonSum = songNum;
}
-(void)setVoiceNum:(float)voiceNum
{
    _voiceNum = voiceNum;
    audio_voiceSum = voiceNum;
}
-(void)destroy{
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)_rtcEngine.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {
        mediaEngine->registerAudioFrameObserver(NULL);
    }
    agoraAudioBuf.release();
}

- (void)dealloc
{
    delete resample;
    delete capture_resample;
}
@end
