//
//  VideoKit.h
//  OpenLive
//
//  Created by zhanxiaochao on 2018/11/2.
//  Copyright © 2018年 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
@class AgoraRtcEngineKit;
NS_ASSUME_NONNULL_BEGIN

typedef enum AG_MEDIA_PLAYER_STATE

{
    
    AG_MEDIA_PLAYER_STATE_UNINITIALIZED = 0,
    
    AG_MEDIA_PLAYER_STATE_READY = 1,
    
    AG_MEDIA_PLAYER_STATE_PLAY = 2,
    
    AG_MEDIA_PLAYER_STATE_PAUSE = 3,
    
    AG_MEDIA_PLAYER_STATE_STOP = 4,
    
    AG_MEDIA_PLAYER_STATE_BUFFERING = 5,
    
    AG_MEDIA_PLAYER_STATE_COMPLETE = 6,
    
}AG_MEDIA_PLAYER_STATE;

typedef enum AG_KIT_ERROR

{
    
    AG_SOURCE_LOADED_ERROR_UNSUPPORTED_ARGUMENT = 1,
    
    AG_SOURCE_LOADED_ERROR_CORRUPTED_DATA = 2,
    
    AG_SOURCE_LOADED_ERROR_UNSUPPORTED_FORMAT = 3,
    
    AG_SOURCE_LOADED_ERROR_NO_AVAILABLE_TRACK = 4,
    
    AG_PLAY_ERROR_NETWORK_UNAVAILABLE = 7,

    
}AG_KIT_ERROR;

@protocol AGLiveKitDelegate<NSObject>
-(void)onPlayerStateChanged:(AG_MEDIA_PLAYER_STATE) state;
-(void)onAudioTrackInfoCallBack:(int)index samplerate:(int)samplerate channels:(int)channels duration:(long)duration index:(int)index;
-(void)onVideoTrackInfoCallback:(int)index width:(int)width height:(int)height ;
-(void)onKitError:(AG_KIT_ERROR)error;

@end
@interface MediaPlayerKit : NSObject

+(instancetype)shareInstance;
//@property (strong, nonatomic) void (^getduation)(long duation);
@property (strong, nonatomic) id<AGLiveKitDelegate> delegate;
// 创建 VideoPlayerKit 指定引擎 设置音频采样率
-(void)createMediaPlayerKitWithRtcEngine;
-(void)createMediaPlayerKitWithRtcEngine:(AgoraRtcEngineKit *_Nullable)rtcEngine withSampleRate:(int)sampleRate;

//传入渲染view
-(void)setVideoView:(NSView *)view;

// 加载播放的文件/可以是链接
-(void)load:(NSString *)url isAutoPlay:(bool)isAutoPlay;
// 获取视频时长
-(long)getDuation;
// 获取当前的视频位置
-(long)getCurrentPosition;
// 拖动到视频播放的位置
-(void)seekTo:(int)msec;
// 视频的播放/暂停
-(void)play;
//
-(void)stop;

-(void)resume;
// 卸载视频
-(void)unload;
// 销毁 VideoPlayerKit
-(void)destroy;
//是否混入麦克风
//-(void)setAudioMixing:(bool)isAudioMixing;
//调节本地视频音量 0~ 400
-(void)adjustPlaybackSignalVolume:(int)volume;
//调节远端视频音量 0~ 400
-(void)adjustPublishSignalVolume:(int)volume;
//是否把音频流推到远端
-(void)publishAudio;
-(void)unpublishAudio;
-(void)publishVideo;
-(void)unpublishVideo;
@end

NS_ASSUME_NONNULL_END
