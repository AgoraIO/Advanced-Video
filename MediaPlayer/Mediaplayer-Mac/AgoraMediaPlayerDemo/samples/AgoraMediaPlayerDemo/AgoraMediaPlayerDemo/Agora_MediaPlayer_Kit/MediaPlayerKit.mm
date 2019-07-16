//
//  VideoKit.m
//  OpenLive
//
//  Created by zhanxiaochao on 2018/11/2.
//  Copyright © 2018年 Agora. All rights reserved.
//
#import "AgoraAudioFrame.h"
#import "MediaPlayerKit.h"
#import "AgoraPlayerKit/AgroaFFplayKit.h"
#import "RTCMTLNSVideoView.h"
#import "RTCVideoFrame.h"
#import "RTCVideoFrameBuffer.h"

@interface MediaPlayerKit()<AgoraVideoSourceProtocol>
{
    AgoraRtcEngineKit *_rtcEngine;
    bool isVideoPublish;
   
}
@property(nonatomic,assign)long duration;
@property(nonatomic,strong)NSView <RTCVideoRenderer> *render;

@end
@implementation MediaPlayerKit
@synthesize consumer;

+(instancetype)shareInstance{
    
    static dispatch_once_t once_t;
    static MediaPlayerKit *videoKit;
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
    //添加视频的约束
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
    __weak typeof(self) weakSelf = self;
    [AgroaFFplayKit shareInstance].videoInfoDataCallBack = ^(long duration, int sample_rate, int channels) {
        [AgoraAudioFrame shareInstance].re_sampleRate = sample_rate;
        [AgoraAudioFrame shareInstance].re_channels = channels;
        //        if (weakSelf.getduation) {
        //            weakSelf.getduation(duration);
        //        }
        weakSelf.duration = duration ;
    };
    
    
    //视频数据的推送
    [AgroaFFplayKit shareInstance].yuvDataCallBack = ^(int * _Nullable linsize, void * _Nonnull data, int w, int h) {
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
            free(tmpBuf);
        }else{
            //self render
            RTCI420Buffer *i420buf = [[RTCI420Buffer alloc] initWithWidth:w height:h dataY:(uint8_t *)data  dataU:(uint8_t *)data  + w * h dataV:(uint8_t *)data +  w * h * 5 /4];
            RTCVideoFrame *videoFrame = [[RTCVideoFrame alloc] initWithBuffer:i420buf rotation:RTCVideoRotation_0 timeStampNs:CACurrentMediaTime()*1000];
            [_render renderFrame:videoFrame];
            
        }
        
    };
    //音频数据的推送
    [AgroaFFplayKit shareInstance].audioDataCallBack = ^(void * _Nonnull data, int length) {
        
        if (!isVideoPublish || data == NULL) {
            return;
        }
        [[AgoraAudioFrame shareInstance] pushAudioSource:data byteLength:length];
    };
    
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
    
    //设置videoSource的代理
    [rtcEngine setVideoSource:self];
    //加入房间
    __weak typeof(self) weakSelf = self;
    [rtcEngine setVideoResolution:CGSizeMake(848, 480) andFrameRate:15 bitrate:1200];
    [AgroaFFplayKit shareInstance].videoInfoDataCallBack = ^(long duration, int sample_rate, int channels) {
        [AgoraAudioFrame shareInstance].re_sampleRate = sample_rate;
        [AgoraAudioFrame shareInstance].re_channels = channels;
//        if (weakSelf.getduation) {
//            weakSelf.getduation(duration);
//        }
         weakSelf.duration = duration ;
    };
    
    
    //视频数据的推送
    [AgroaFFplayKit shareInstance].yuvDataCallBack = ^(int * _Nullable linsize, void * _Nonnull data, int w, int h) {
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
            [self.consumer consumeRawData:tmpBuf withTimestamp:CMTimeMake(CACurrentMediaTime()*1000, 1000) format:AgoraVideoPixelFormatI420 size:CGSizeMake(w, h) rotation:AgoraVideoRotationNone];
            free(tmpBuf);
        }else{
          //self render
          RTCI420Buffer *i420buf = [[RTCI420Buffer alloc] initWithWidth:w height:h dataY:(uint8_t *)data  dataU:(uint8_t *)data  + w * h dataV:(uint8_t *)data +  w * h * 5 /4];
          RTCVideoFrame *videoFrame = [[RTCVideoFrame alloc] initWithBuffer:i420buf rotation:RTCVideoRotation_0 timeStampNs:CACurrentMediaTime()*1000];
          [_render renderFrame:videoFrame];
            
          [self.consumer consumeRawData:data withTimestamp:CMTimeMake(CACurrentMediaTime()*1000, 1000) format:AgoraVideoPixelFormatI420 size:CGSizeMake(linsize[0], h) rotation:AgoraVideoRotationNone];
        }
        
    };
    //音频数据的推送
    [AgroaFFplayKit shareInstance].audioDataCallBack = ^(void * _Nonnull data, int length) {
        
        if (!isVideoPublish || data == NULL) {
            return;
        }
        [[AgoraAudioFrame shareInstance] pushAudioSource:data byteLength:length];
    };
}

// 加载播放的文件/可以是链接
-(void)load:(NSString *)url isAutoPlay:(bool)isAutoPlay{
    
    @synchronized(self){
    isVideoPublish = isAutoPlay;
    [[AgroaFFplayKit shareInstance] loadVideo:url isAutoPlay:true];
    
    }
}
// 获取视频时长
//-(int)getDuation{
//
//    return 0;
//}
// 获取当前的视频位置
-(long)getCurrentPosition{
    
    return [[AgroaFFplayKit shareInstance] getCurrentPosition];
}

// 拖动到视频播放的位置
-(void)seekTo:(int) msec{
    
    @synchronized(self){
        
        [[AgroaFFplayKit shareInstance] seekTo:msec];
        
    }
}

// 视频的播放/暂停
-(void)play{
    
    @synchronized(self){
        isVideoPublish = true;
        [[AgoraAudioFrame shareInstance] destroyAudioBuf];
        [[AgroaFFplayKit shareInstance] play];
    }
}

-(void)resume{
    
    @synchronized(self){
        isVideoPublish = false;
        [[AgoraAudioFrame shareInstance] destroyAudioBuf];
        [[AgroaFFplayKit shareInstance] pause];
    }
}

// 卸载视频
-(void)unload{
    
    @synchronized(self){
        
        [[AgoraAudioFrame shareInstance] destroyAudioBuf];
    }
}



// 销毁 VideoPlayerKit
-(void)destroy{
    
    @synchronized (self) {
        
        [[AgroaFFplayKit shareInstance] unload];

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
//是否混入麦克风的音量
-(void)setAudioMixing:(bool)isAudioMixing{
    
    @synchronized(self){
        if (isAudioMixing) {
            [AgoraAudioFrame shareInstance].voiceNum = 0;
        }else{
            [AgoraAudioFrame shareInstance].voiceNum = 1;
        }
    }
    
}
//调节本地视频音量
-(void)adjustPlaybackSignalVolume:(int)volume{
    
    @synchronized(self){
        
        [[AgroaFFplayKit shareInstance] adjustPlaybackSignalVolume:volume];
    }
    
}
//调节远端视频音量
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

    isVideoPublish  = true;
    
    }
}
-(void)unpublishAudio{
    @synchronized(self){

    isVideoPublish  = false;
        
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
