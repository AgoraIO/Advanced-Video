//
//  VideoKit.h
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2018/11/2.
//  Copyright © 2018 Agora. All rights reserved.
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

@protocol MediaPlayerKitDelegate<NSObject>

/**
 返回播放器状态的回调

 @param state 当前播放器的状态
 */
-(void)onPlayerStateChanged:(AG_MEDIA_PLAYER_STATE) state;

/**
 返回音频信息的回调

 @param index 当前音频流的索引
 @param samplerate 音频采样率
 @param channels 音频声道数
 @param duration 视频总时长
 */
-(void)onAudioTrackInfoCallBack:(int)index samplerate:(int)samplerate channels:(int)channels duration:(long)duration;

/**
 返回视频信息的回调

 @param index 当前视频流的索引
 @param width 当前视频的宽度
 @param height 当前视频的高度
 */
-(void)onVideoTrackInfoCallback:(int)index width:(int)width height:(int)height ;

/**
 返回MediaPlayerKit 错误信息的回调

 @param error 错误回调信息
 */
-(void)onKitError:(AG_KIT_ERROR)error;

@end
@interface MediaPlayerKit : NSObject


/**
 创建MediaPlayerKit单实例

 @return MediaPlayerKit's Instance
 */
+(instancetype)shareInstance;

/**
 MediaplayerKit 的代理方法 设置该代理方法 可获取当前MediaPlayKit的各种信息的回调
 */
@property (strong, nonatomic) id<MediaPlayerKitDelegate> delegate;
/**
 创建播放器引擎
 @note 默认方式创建的播放器引擎 可进行本地视频播放的必要功能
 */
-(void)createMediaPlayerKitWithRtcEngine;

/**
 通过外部传入的AgoraRTCEngine 和 音频采样率 来创建播放器引擎
 @note 传入的AgroaRTCEngine 不能为空 音频采样率 是为了AgroaRTCEngine和播放器引擎互通需要设置的必要参数
 @param rtcEngine AgoraRTCEngine
 @param sampleRate 音频采样率
 */
-(void)createMediaPlayerKitWithRtcEngine:(AgoraRtcEngineKit *_Nullable)rtcEngine withSampleRate:(int)sampleRate;

/**
 设置播放器引擎需要绑定的显示的view

 @param view 传入需要渲染的view
 */
-(void)setVideoView:(NSView *)view;

/**
 打开需要播放器播放的地址 可设置打开地址之后是否自动播放

 @param url 播放的地址
 @param isAutoPlay 打开之后是否自动播放
 */
-(void)load:(NSString *)url isAutoPlay:(bool)isAutoPlay;
/**
 获取当前播放视频的总时长
 @note 这个获取视频总时长的时机最好是收到播放器的状态回调 AG_MEDIA_PLAYER_STATE_READY 之后再获取 在没有收到这个状态回调之前调用这个接口 获取的值为空值
 @return 视频总时长
 */
-(long)getDuration;
/**
 获取当前播放视频的进度
 @note 获取的当前的进度精度为int64 需要自己转换成毫秒
 @return 当前视频播放的进度
 */
-(long)getCurrentPosition;
/**
 设置播放进度 精度为纳秒
 @note 进度的换算方法h根据总时长去换算 long seekToPos = aspt(计算当前视频进度占总时长的比例) * duration
 @param msec 进度
 */
-(void)seekTo:(long)msec;
/**
 开始播放
 */
-(void)play;
/**
 停止播放
 */
-(void)stop;
/**
 恢复播放
 */
-(void)resume;
/**
 卸载当前视频
 */
-(void)unload;
/**
 销毁MediaPlayerKit 实例
 */
-(void)destroy;

/**
 调节本地播放视频的音量
 
 @param volume 调整的播放音量的值范围为0 ~ 400
 */
-(void)adjustPlaybackSignalVolume:(int)volume;
//adjust remoteSignalVolume  0~ 400

/**
 调节远端播放音量
 
 @param volume 调整的播放音量的值范围为0 ~ 400
 */
-(void)adjustPublishSignalVolume:(int)volume;
/**
 调用本接口 可以把本地音频推到远端
 */
-(void)publishAudio;

/**
 调用本接口 停止把本地音频流推到远端
 */
-(void)unpublishAudio;
/**
 调用本接口 把本地视频流推到远端
 */
-(void)publishVideo;
/**
 调用本接口 停止把本地视频流推到远端
 */
-(void)unpublishVideo;
@end

NS_ASSUME_NONNULL_END
