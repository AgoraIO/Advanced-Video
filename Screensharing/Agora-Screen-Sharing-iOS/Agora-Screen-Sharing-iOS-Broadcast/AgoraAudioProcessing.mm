//
//  AGVideoPreProcessing.m
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by Alex Zheng on 7/28/16.
//  Copyright © 2016 Agora.io All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AgoraAudioProcessing.h"
#import "AgoraAudioCriticalSection.h"

#import <AgoraRtcEngineKit/IAgoraRtcEngine.h>
#import <AgoraRtcEngineKit/IAgoraMediaEngine.h>
#import <string.h>

static const int kAudioBufferPoolSize = 500000;
static  unsigned char mRecordingAudioAppPool[kAudioBufferPoolSize];
static  unsigned char mRecordingAudioMicPool[kAudioBufferPoolSize];
static int mRecordingAppBufferBytes = 0;
static int mRecordingMicBufferBytes = 0;
static CriticalSectionWrapper *CritSect = CriticalSectionWrapper::CreateCriticalSection();

void pushAudioAppFrame(unsigned char *inAudioFrame, int frameSize)
{
    CriticalSectionScoped lock(CritSect);
    
    int remainedSize = kAudioBufferPoolSize - mRecordingAppBufferBytes;
    if (remainedSize >= frameSize) {
        memcpy(mRecordingAudioAppPool+mRecordingAppBufferBytes, inAudioFrame, frameSize);
    } else {
        mRecordingAppBufferBytes = 0;
        memcpy(mRecordingAudioAppPool+mRecordingAppBufferBytes, inAudioFrame, frameSize);
    }
    
    mRecordingAppBufferBytes += frameSize;
}

void pushAudioMicFrame(unsigned char *inAudioFrame, int frameSize)
{
    CriticalSectionScoped lock(CritSect);
    
    int remainedSize = kAudioBufferPoolSize - mRecordingMicBufferBytes;
    if (remainedSize >= frameSize) {
        memcpy(mRecordingAudioMicPool+mRecordingMicBufferBytes, inAudioFrame, frameSize);
    } else {
        mRecordingMicBufferBytes = 0;
        memcpy(mRecordingAudioMicPool+mRecordingMicBufferBytes, inAudioFrame, frameSize);
    }
    
    mRecordingMicBufferBytes += frameSize;
}

class AgoraAudioFrameObserver : public agora::media::IAudioFrameObserver
{
    public:
    virtual bool onRecordAudioFrame(AudioFrame& audioFrame) override
    {
        CriticalSectionScoped lock(CritSect);
        
        int bytes = audioFrame.samples * audioFrame.channels * audioFrame.bytesPerSample;
        
        if (mRecordingAppBufferBytes < bytes && mRecordingMicBufferBytes < bytes) {
            return false;
        }
        
        short *mixedBuffer = (short *)malloc(bytes);
        
        if (mRecordingAppBufferBytes >= bytes) {
            memcpy(mixedBuffer, mRecordingAudioAppPool, bytes);
            mRecordingAppBufferBytes -= bytes;
            memcpy(mRecordingAudioAppPool, mRecordingAudioAppPool+bytes, mRecordingAppBufferBytes);
            
            if (mRecordingMicBufferBytes >= bytes) {
                short *micBuffer = (short *)mRecordingAudioMicPool;
                for (int i = 0; i < bytes / 2; ++i) {
                    
                    int number = mixedBuffer[i] / 4;
                    number += micBuffer[i];
                    
                    if (number > 32767) {
                        number = 32767;
                    } else if (number < -32768) {
                        number = -32768;
                    }
                    
                    mixedBuffer[i] = number;
                }
                
                mRecordingMicBufferBytes -= bytes;
                memcpy(mRecordingAudioMicPool, mRecordingAudioMicPool+bytes, mRecordingMicBufferBytes);
            }
        } else if (mRecordingMicBufferBytes >= bytes) {
            memcpy(mixedBuffer, mRecordingAudioMicPool, bytes);
            mRecordingMicBufferBytes -= bytes;
            memcpy(mRecordingAudioMicPool, mRecordingAudioMicPool+bytes, mRecordingMicBufferBytes);
        }
        
        memcpy(audioFrame.buffer, mixedBuffer, bytes);
        free(mixedBuffer);
        
        return true;
    }
    
    virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame) override {
        return true;
    }
    
    virtual bool onMixedAudioFrame(AudioFrame& audioFrame) override {
        return true;
    }
    
    virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) override {
        return true;
    }
};

static AgoraAudioFrameObserver s_audioFrameObserver;

@implementation AgoraAudioProcessing
+ (void)registerAudioPreprocessing: (AgoraRtcEngineKit*) kit
{
    if (!kit) {
        return;
    }
    
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)kit.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {
        mediaEngine->registerAudioFrameObserver(&s_audioFrameObserver);
    }
}
    
+ (void)deregisterAudioPreprocessing:(AgoraRtcEngineKit*)kit
{
    if (!kit) {
        return;
    }
    
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)kit.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {
        mediaEngine->registerAudioFrameObserver(NULL);
    }
}

+ (void)pushAudioAppBuffer:(CMSampleBufferRef)sampleBuffer
{
    AudioBufferList inAudioBufferList;
    CMBlockBufferRef blockBuffer = nil;
    OSStatus status = CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, NULL, &inAudioBufferList, sizeof(inAudioBufferList), NULL, NULL, 0, &blockBuffer);
    if (status != noErr) {
        return;
    }
    
    AudioBuffer buffer = inAudioBufferList.mBuffers[0];
    uint8_t* p = (uint8_t*)buffer.mData;
    
    for (int i = 0; i < buffer.mDataByteSize; i += 2) {
        uint8_t tmp;
        tmp = p[i];
        p[i] = p[i + 1];
        p[i + 1] = tmp;
    }
    pushAudioAppFrame(p, buffer.mDataByteSize);
    
    if (blockBuffer) {
        CFRelease(blockBuffer);
    }
}
    
+ (void)pushAudioMicBuffer:(CMSampleBufferRef)sampleBuffer
{
    AudioBufferList inAaudioBufferList;
    CMBlockBufferRef blockBuffer = nil;
    OSStatus status = CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, NULL, &inAaudioBufferList, sizeof(inAaudioBufferList), NULL, NULL, 0, &blockBuffer);
    if (status != noErr) {
        return;
    }
    
    AudioBuffer buffer = inAaudioBufferList.mBuffers[0];
    uint8_t* p = (uint8_t*)buffer.mData;
    
    pushAudioMicFrame(p, buffer.mDataByteSize);
    
    if (blockBuffer) {
        CFRelease(blockBuffer);
    }
}
@end
