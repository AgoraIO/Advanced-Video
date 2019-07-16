//
//  VideoKit.h
//  OpenLive
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

@protocol AGLiveKitDelegate<NSObject>
-(void)onPlayerStateChanged:(AG_MEDIA_PLAYER_STATE) state;
-(void)onAudioTrackInfoCallBack:(int)index samplerate:(int)samplerate channels:(int)channels duration:(long)duration index:(int)index;
-(void)onVideoTrackInfoCallback:(int)index width:(int)width height:(int)height ;
-(void)onKitError:(AG_KIT_ERROR)error;

@end
@interface MediaPlayerKit : NSObject

+(instancetype)shareInstance;
@property (strong, nonatomic) id<AGLiveKitDelegate> delegate;
// create mediaPlayerkit
-(void)createMediaPlayerKitWithRtcEngine;
// create mediaPlayerkit with samplerate
-(void)createMediaPlayerKitWithRtcEngine:(AgoraRtcEngineKit *_Nullable)rtcEngine withSampleRate:(int)sampleRate;
//set render view
-(void)setVideoView:(NSView *)view;
//load url and set if need to autoplay
-(void)load:(NSString *)url isAutoPlay:(bool)isAutoPlay;
//get duration of this video
-(long)getDuation;
//get current position of this video
-(long)getCurrentPosition;
//seek to the pos of the video
-(void)seekTo:(int)msec;
// play
-(void)play;
// stop
-(void)stop;
// resume the video
-(void)resume;
// unload the video
-(void)unload;
// destroy
-(void)destroy;
//adjust localSignalVolume  0 ~ 400
-(void)adjustPlaybackSignalVolume:(int)volume;
//adjust remoteSignalVolume  0~ 400
-(void)adjustPublishSignalVolume:(int)volume;
//set  publishAudio
-(void)publishAudio;
//set dont't  publishAudio
-(void)unpublishAudio;
//set  publishVideo
-(void)publishVideo;
//set don't publishVideo
-(void)unpublishVideo;
@end

NS_ASSUME_NONNULL_END
