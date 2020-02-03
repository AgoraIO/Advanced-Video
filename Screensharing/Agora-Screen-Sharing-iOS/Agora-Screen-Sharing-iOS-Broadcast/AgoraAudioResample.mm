//
//  AgoraAudioResample.m
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2019/4/22.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "AgoraAudioResample.h"
#include "external_resampler.h"

static external_resampler* resampler;
int16_t inBuffer[44100];
int inBufferSamples = 0;
int outBufferSamples = 0;

@interface AgoraAudioResample ()
@property (assign, nonatomic) Float64 targetSampleRate;
@property (assign, nonatomic) int  targetChannels;

@end

@implementation AgoraAudioResample
- (instancetype)initWithTargetSampleRate:(Float64)targetSampleRate withTargetChannels:(int)targetChannels
{
    if (self = [super init]) {
        self.targetSampleRate = targetSampleRate;
        self.targetChannels = targetChannels;
    }
    return self;
}

- (void)resamplingBuffer:(CMSampleBufferRef)sampleBuffer
              completion:(void (^)(unsigned char *audioFrame, int frameSize))completion
{
    AudioBufferList inAudioBufferList;
    CMBlockBufferRef blockBuffer;
    OSStatus status = CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer, NULL, &inAudioBufferList, sizeof(inAudioBufferList), NULL, NULL, 0, &blockBuffer);
    if (status != noErr) {
        return;
    }
    
    AudioStreamBasicDescription inASBD = *CMAudioFormatDescriptionGetStreamBasicDescription((CMAudioFormatDescriptionRef)CMSampleBufferGetFormatDescription(sampleBuffer));
    
    AudioBuffer buffer = inAudioBufferList.mBuffers[0];
    
    if (inASBD.mFormatFlags & kAudioFormatFlagIsBigEndian) {
        uint8_t* p = (uint8_t*)buffer.mData;
        for (int i = 0; i < buffer.mDataByteSize; i += 2) {
            uint8_t tmp;
            tmp = p[i];
            p[i] = p[i + 1];
            p[i + 1] = tmp;
        }
    }
    
    int inFs = inASBD.mSampleRate;
    int outFs = self.targetSampleRate;
    if (inFs == outFs && inASBD.mChannelsPerFrame == self.targetChannels) {
        completion((unsigned char *)buffer.mData, buffer.mDataByteSize);
        if (blockBuffer) {
            CFRelease(blockBuffer);
        }
        return;
    }
    
    int inDataSamplesPer10ms = inFs / 100;
    int outDataSamplesPer10ms = (outFs / 100);
    memcpy(inBuffer + inBufferSamples, buffer.mData, buffer.mDataByteSize);
    inBufferSamples += buffer.mDataByteSize / 2;
    
    if (!resampler) {
        resampler = new external_resampler();
    }
    int pPos = 0;
    while (inBufferSamples > inDataSamplesPer10ms) {
        int outDataSizePer10ms = outDataSamplesPer10ms * 2 * self.targetChannels;
        short data[outDataSizePer10ms];
        resampler->do_resample(inBuffer + pPos, inDataSamplesPer10ms, buffer.mNumberChannels, inFs,
                               data, outDataSamplesPer10ms ,self.targetChannels, outFs);
        
        pPos += inDataSamplesPer10ms;
        inBufferSamples -= inDataSamplesPer10ms;
        if (completion) {
            completion((unsigned char *)data, outDataSizePer10ms);
        }
    }
    memmove(inBuffer, inBuffer + pPos, sizeof(int16_t) * inBufferSamples);
    outBufferSamples = 0;
    
    if (blockBuffer) {
        CFRelease(blockBuffer);
    }
}
@end
