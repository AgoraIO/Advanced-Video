//
//  VideoKit.m
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2018/11/2.
//  Copyright © 2018 Agora. All rights reserved.
//
#import "AgoraAudioFrame.h"
#import "MediaPlayerKit.h"
#import <MediaPlayerKit/AgoraFFplayKit.h>
#import "RTCMTLNSVideoView.h"
#import "RTCVideoFrame.h"
#import "RTCVideoFrameBuffer.h"

#define fftime_to_milliseconds(ts) (av_rescale(ts, 1000, AV_TIME_BASE))
#define milliseconds_to_fftime(ms) (av_rescale(ms, AV_TIME_BASE, 1000))

@interface MediaPlayerKit()<AgoraVideoSourceProtocol>
{
    AgoraRtcEngineKit *_rtcEngine;
    bool isVideoPublish;
    bool isAudioPublish;
    bool _isAutoPlay;
    
}
@property(nonatomic,assign)long duration;
@property(nonatomic,strong)NSView <RTCVideoRenderer> *render;

@end
@implementation MediaPlayerKit
@synthesize consumer;
static MediaPlayerKit *videoKit = nil;
static dispatch_once_t once_t;
+(instancetype)shareInstance{
    
    dispatch_once(&once_t, ^{
        if (videoKit == NULL) {
            videoKit = [[self alloc] init];
        }
    });
    return videoKit;
    
}
-(void)setVideoView:(NSView *)view
{
    _render = [[RTCMTLNSVideoView alloc] initWithFrame:view.bounds];
    [view addSubview:_render positioned:NSWindowBelow relativeTo:view];
    //add view's constraints
    NSDictionary *views = NSDictionaryOfVariableBindings(_render);
    
    NSArray *constraintsHorizontal =
    [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[_render]-0-|"
                                            options:0
                                            metrics:nil
                                              views:views];
    [view addConstraints:constraintsHorizontal];
    
    NSArray *constraintsVertical =
    [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[_render]-0-|"
                                            options:0
                                            metrics:nil
                                              views:views];
    [view addConstraints:constraintsVertical];
    
}
-(void)createMediaPlayerKitWithRtcEngine
{
    
    [self initMediaPlayer:false];
    
}
-(void)createMediaPlayerKitWithRtcEngine:(AgoraRtcEngineKit *_Nullable)rtcEngine withSampleRate:(int)sampleRate{
    _rtcEngine = rtcEngine;
    [AgoraAudioFrame shareInstance].sampleRate = sampleRate;
    [[AgoraAudioFrame shareInstance] registerEngineKit:_rtcEngine];
    [rtcEngine setParameters:@"{\"che.video.keep_prerotation\":false}"];
    [rtcEngine setParameters:@"{\"che.video.local.camera_index\":1025}"];
    [rtcEngine setRecordingAudioFrameParametersWithSampleRate:sampleRate channel:2 mode:AgoraAudioRawFrameOperationModeReadWrite samplesPerCall:sampleRate * 0.01 * 2];
    [rtcEngine setPlaybackAudioFrameParametersWithSampleRate:sampleRate channel:2 mode:AgoraAudioRawFrameOperationModeReadWrite samplesPerCall:sampleRate * 0.01 * 2];
    [rtcEngine setLocalVideoMirrorMode:AgoraVideoMirrorModeDisabled];
    [rtcEngine setVideoSource:self];
    [rtcEngine setVideoResolution:CGSizeMake(848, 480) andFrameRate:15 bitrate:1200];
    [self initMediaPlayer:true];
    
}
-(void)initMediaPlayer:(bool)isHasRTCEngine{
    
    __weak typeof(self) weakSelf = self;
    //视频数据的推送
    [AgoraFFplayKit shareInstance].yuvDataCallBack = ^(int * _Nullable linsize, void * _Nonnull data, int w, int h) {
        if (linsize[0] !=  w) {
            uint8_t *tmpBuf = (uint8_t *)malloc(w * h * 3/2);
            
            for (int  i = 0; i < h; i++) {
                memcpy(tmpBuf + i * w, (uint8_t *)data + linsize[0] * i, w );
            }
            for (int i = 0 ; i < h /2 ; i++) {
                memcpy(tmpBuf + w * h + w/2  * i,(uint8_t *)data + linsize[0] * h +  linsize[0]/2 * i, w / 2);
            }
            for (int i = 0 ; i < h / 2 ; i ++) {
                
                memcpy(tmpBuf +  w * h * 5 /4 + w /2 * i,(uint8_t *)data + linsize[0] * h * 5 /4 + linsize[0]/2 * i, w/2);
                
            }
            //self render
            RTCI420Buffer *i420buf = [[RTCI420Buffer alloc] initWithWidth:w height:h dataY:tmpBuf dataU:tmpBuf + w * h dataV:tmpBuf +  w * h * 5 /4];
            RTCVideoFrame *videoFrame = [[RTCVideoFrame alloc] initWithBuffer:i420buf rotation:RTCVideoRotation_0 timeStampNs:CACurrentMediaTime()*1000];
            [_render renderFrame:videoFrame];
            if (isHasRTCEngine && isVideoPublish) {
                [self.consumer consumeRawData:data withTimestamp:CMTimeMake(CACurrentMediaTime()*1000, 1000) format:AgoraVideoPixelFormatI420 size:CGSizeMake(linsize[0], h) rotation:AgoraVideoRotationNone];
            }
            free(tmpBuf);
        }else{
            
            //self render
            RTCI420Buffer *i420buf = [[RTCI420Buffer alloc] initWithWidth:w height:h dataY:(uint8_t *)data  dataU:(uint8_t *)data  + w * h dataV:(uint8_t *)data +  w * h * 5 /4];
            RTCVideoFrame *videoFrame = [[RTCVideoFrame alloc] initWithBuffer:i420buf rotation:RTCVideoRotation_0 timeStampNs:CACurrentMediaTime()*1000];
            [_render renderFrame:videoFrame];
            if (isHasRTCEngine && isVideoPublish) {
                [self.consumer consumeRawData:data withTimestamp:CMTimeMake(CACurrentMediaTime()*1000, 1000) format:AgoraVideoPixelFormatI420 size:CGSizeMake(linsize[0], h) rotation:AgoraVideoRotationNone];
            }
            
        }
        
    };
    //音频数据的推送
    if (isHasRTCEngine) {
        [AgoraFFplayKit shareInstance].audioDataCallBack = ^(void * _Nonnull data, int length) {
            
            if (!isAudioPublish || data == NULL) {
                return;
            }
            [[AgoraAudioFrame shareInstance] pushAudioSource:data byteLength:length];
        };
    }
    [AgoraFFplayKit shareInstance].videoInfoDataCallBack = ^(int index, long duation, int sample_rate, int channels) {
        
        [AgoraAudioFrame shareInstance].re_sampleRate = sample_rate;
        [AgoraAudioFrame shareInstance].re_channels = channels;
        weakSelf.duration = duation ;
        if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(onAudioTrackInfoCallBack:samplerate:channels:duration:)]) {
            [weakSelf.delegate onAudioTrackInfoCallBack:index samplerate:sample_rate channels:channels duration:duation/1000];
        }
        
    };
    [AgoraFFplayKit shareInstance].onVideoTrackCallBack = ^(int index, int width, int height) {
        
        if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(onVideoTrackInfoCallback:width:height:)]) {
            [weakSelf.delegate onVideoTrackInfoCallback:index width:width height:height];
        }
    };
    [AgoraFFplayKit shareInstance].onKitError = ^(FF_KIT_ERROR error) {
        if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(onKitError:)]) {
            [weakSelf.delegate onKitError:(AG_KIT_ERROR)error];
        }
    };
    [AgoraFFplayKit shareInstance].onPlayerStateChanged = ^(FF_MEDIA_PLAYER_STATE state) {
        if (weakSelf.delegate && [weakSelf.delegate respondsToSelector:@selector(onPlayerStateChanged:)]) {
            [weakSelf.delegate onPlayerStateChanged:(AG_MEDIA_PLAYER_STATE)state];
        }
    };
    
}

-(void)load:(NSString *)url isAutoPlay:(bool)isAutoPlay{
    
    @synchronized(self){
        isVideoPublish = false;
        isAudioPublish = false;
        _isAutoPlay = isAutoPlay;
        [self performSelector:@selector(loadVideo:) onThread:[NSThread mainThread] withObject:url waitUntilDone:NO];
        
    }
}
-(void)loadVideo:(NSString *)url
{
    [[AgoraFFplayKit shareInstance] loadVideo:url isAutoPlay:_isAutoPlay];
}

-(long)getCurrentPosition{
    
    return [[AgoraFFplayKit shareInstance] getCurrentPosition] / 1000;
}

-(void)seekTo:(long) msec{
    
    @synchronized(self){
        
        [[AgoraFFplayKit shareInstance] seekTo:msec * 1000];
        
    }
}

-(void)play{
    
    @synchronized(self){
        [[AgoraAudioFrame shareInstance] destroyAudioBuf];
        [[AgoraFFplayKit shareInstance] play];
    }
}

-(void)resume{
    
    @synchronized(self){
        [[AgoraAudioFrame shareInstance] destroyAudioBuf];
        [[AgoraFFplayKit shareInstance] pause];
    }
}
-(void)unload{
    
    @synchronized(self){
        
        [[AgoraAudioFrame shareInstance] destroyAudioBuf];
        [[AgoraFFplayKit shareInstance] unload];
        
    }
}
-(void)stop{
    
    @synchronized(self){
        
        [[AgoraFFplayKit shareInstance] pause];
    }
    
}
-(void)destroy{
    
    @synchronized (self) {
        
        videoKit = nil;
        once_t = 0;
        
    }
    
}


- (AgoraVideoBufferType)bufferType {
    
    return AgoraVideoBufferTypeRawData;
}

- (void)shouldDispose {
    
}

- (BOOL)shouldInitialize {
    
    return true;
    
}

- (void)shouldStart {
    
}

- (void)shouldStop {
    
}

-(void)setAudioMixing:(bool)isAudioMixing{
    
    @synchronized(self){
        if (isAudioMixing) {
            [AgoraAudioFrame shareInstance].voiceNum = 0;
        }else{
            [AgoraAudioFrame shareInstance].voiceNum = 1;
        }
    }
    
}

-(void)adjustPlaybackSignalVolume:(int)volume{
    
    @synchronized(self){
        
        [[AgoraFFplayKit shareInstance] adjustPlaybackSignalVolume:volume];
    }
    
}

-(void)adjustPublishSignalVolume:(int)volume{
    
    @synchronized(self){
        
        [AgoraAudioFrame shareInstance].songNum = volume / 100;
        
    }
}


-(long)getDuation{
    
    @synchronized(self){
        
        return self.duration;
        
    }
}
-(void)publishAudio{
    
    @synchronized(self){
        
        isAudioPublish  = true;
        
    }
}
-(void)unpublishAudio{
    @synchronized(self){
        
        isAudioPublish  = false;
        
    }
}
-(void)publishVideo{
    @synchronized(self){
        
        isVideoPublish  = true;
        
    }
}
-(void)unpublishVideo{
    
    @synchronized(self){
        
        isVideoPublish  = false;
        
    }
}

@end

