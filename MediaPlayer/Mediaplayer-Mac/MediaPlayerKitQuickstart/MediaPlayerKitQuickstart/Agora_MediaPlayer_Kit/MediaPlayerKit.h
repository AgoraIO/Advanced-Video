//
//  MediaPlayerKit.h
//
//  Created by zhanxiaochao on 2018/11/2.
//  Copyright © 2018 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
@class AgoraRtcEngineKit;
NS_ASSUME_NONNULL_BEGIN
/**
 Current state of MediaPlayer.
 */
typedef enum AG_MEDIA_PLAYER_STATE
{
    /** MediaPlayer is not initialized. */
    AG_MEDIA_PLAYER_STATE_UNINITIALIZED = 0,
    /** MediaPlayer is ready for play. */
    AG_MEDIA_PLAYER_STATE_READY = 1,
    /** MediaPlayer is playing video. */
    AG_MEDIA_PLAYER_STATE_PLAY = 2,
    /** The video is paused. */
    AG_MEDIA_PLAYER_STATE_PAUSE = 3,
    /** The video is stopped. */
    AG_MEDIA_PLAYER_STATE_STOP = 4,
    /** The video is buffering. */
    AG_MEDIA_PLAYER_STATE_BUFFERING = 5,
    /** The video playback is complete. */
    AG_MEDIA_PLAYER_STATE_COMPLETE = 6,
    
}AG_MEDIA_PLAYER_STATE;

/**
 MediaPlayer playback error codes.
 */
typedef enum AG_KIT_ERROR
{
    /** Used the unsupported argument. */
    AG_SOURCE_LOADED_ERROR_UNSUPPORTED_ARGUMENT = 1,
    /** The data is corrupted. */
    AG_SOURCE_LOADED_ERROR_CORRUPTED_DATA = 2,
    /** Used the unsupported format. */
    AG_SOURCE_LOADED_ERROR_UNSUPPORTED_FORMAT = 3,
    /** No available audio track. */
    AG_SOURCE_LOADED_ERROR_NO_AVAILABLE_TRACK = 4,
    /** No available network. */
    AG_PLAY_ERROR_NETWORK_UNAVAILABLE = 7,
    
    
}AG_KIT_ERROR;

/**
 The MediaPlayerKitDelegate protocol enables callbacks to your app.
 
 The SDK uses delegate callbacks in the MediaPlayerKitDelegate protocol
 to report runtime events to the app.
 */
@protocol MediaPlayerKitDelegate<NSObject>

/**
 Reports the current state of MediaPlayer.
 
 @param state Currents state of MediaPlayer, see AG_MEDIA_PLAYER_STATE
 */
-(void)onPlayerStateChanged:(AG_MEDIA_PLAYER_STATE) state;

/**
 Reports the audio track information.
 
 @param index The index of a current audio stream, representing the number
 of the audio streams.
 @param samplerate The audio sample rate (Hz).
 @param channels The number of channels (mono or dual).
 @param duration The duration (ms) of the video.
 */
-(void)onAudioTrackInfoCallback:(int)index samplerate:(int)samplerate channels:(int)channels duration:(long)duration;

/**
 Reports the video track information.
 
 @param index The index of a current video stream, representing the number of
 the video streams.
 @param width The video width (pixel).
 @param height The video height (pixel).
 */
-(void)onVideoTrackInfoCallback:(int)index width:(int)width height:(int)height ;

/**
 Reports the MediaPlayer playback error codes.
 
 @param error The playback error codes, see AG_KIT_ERROR
 */
-(void)onKitError:(AG_KIT_ERROR)error;

@end

/**
 The MediaPlayerKit class, providing API methods for apps to easily play
 videos.
 */
@interface MediaPlayerKit : NSObject


/**
 Initializes a MediaPlayerKit object.
 
 Call this method to initialize the service before using MediaPlayerKit.
 
 @return An object of the MediaPlayerKit class.
 */
+(instancetype)shareInstance;

/**
 Sets and retrieves the MediaPlayerKitDelegate.
 
 The MediaPlayerKitDelegate reports to the app on MediaPlayerKitWithRtcEngine
 runtime events.
 
 All methods defined in the delegate are optional implementation methods.
 */
@property (weak, nonatomic) id<MediaPlayerKitDelegate> delegate;

/**
 Creates the MediaPlayerKit engine by default.
 
 @note You can use this method to create the engine when you use the MediaPlayer as a local media player.
 */
-(void)createMediaPlayerKitWithRtcEngine;

/**
 Creates the MediaPlayerKit engine.
 
 Creates the MediaPlayerKit engine with the AgoraRtcEngine and the audio
 sample rate.
 
 @note
 - You can use this method to create the engine when you want to publish the video played by MediaPlayer to remote users.
 - The parameter passed into AgoraRtcEngine (`rtcEngine`) cannot be NIL.
 
 @param rtcEngine AgoraRTCEngine
 @param sampleRate The audio sample rate (Hz)
 */
-(void)createMediaPlayerKitWithRtcEngine:(AgoraRtcEngineKit *_Nullable)rtcEngine withSampleRate:(int)sampleRate;

/**
 Sets the playback video view and configures the playback video display
 settings on the device.
 
 The app calls this method to bind each video window (`view`) of the playback
 video streams and configures the playback video display settings.
 
 - If you use MediaPlayer as a local media player, call this method after initialization.
 - If you want to publish the playback video to remote users, call this method after
 initialization and before joining the channel.
 
 When you destroy a MediaPlayerKit instance, the binding here is not valid.
 
 @param view The playback video view and display settings.
 */
-(void)setVideoView:(NSView *)view;

/**
 Loads a specified video into the memory.
 
 @note Call this method after joining the channel when you want to publish
 the video to remote users.
 
 @param url The URL address or the local file path of a specified video.
 @param isAutoPlay Sets whether to play the video automatically after loading: *YES: Play the video automatically
 * NO: Does not play the video automatically
 */
-(void)load:(NSString *)url isAutoPlay:(bool)isAutoPlay;
/**
 Retrieves the duration (ms) of the current video.
 
 @warning You can not retrieves the duration of the real-time video streams.
 
 You can retrieve the duration of the current video only when you have received the AG_MEDIA_PLAYER_STATE_READY state code,
 otherwise the duration is NIL.
 
 @return * &ge; 0: The video duration, if the method call is successful
 * < 0: Failure
 */
-(long)getDuration;
/**
 Retrieves the playback position (ms) of the current video.
 
 @warning You can not retrieves the duration of the real-time video streams.
 
 @return * &ge; 0: The video position, if the method call is successful
 * < 0: Failure
 */
-(long)getCurrentPosition;
/**
 Sets the playback position (ms) of the current video.
 
 This method sets the playback position (ms) of the current video to a
 different starting position (the default starting position is the beginning).
 
 @param msec The playback starting position (ms) of the current video.
 */
-(void)seekTo:(long)msec;
/**
 Plays a specified video.
 */
-(void)play;
/**
 Stops playing a specified video.
 */
-(void)stop;
/**
 Pauses a specified video.
 */
-(void)pause;
/**
 Resumes playing a specified video.
 */
-(void)resume;
/**
 Releases a specified loaded video from the memory.
 */
-(void)unload;
/**
 Destroys a MediaPlayerKit instance.
 */
-(void)destroy;
/**
 Adjusts the local playback volume.
 
 @param volume The playback volume. The value ranges between 0 and 400.
 
 @note
 - The value of `volume` is not measured in decibels but in 1. Agora defines
 original volume to 100.
 - 400: (Maximum) Four times the original volume with signal clipping protection.
 - 0: Mute
 */
-(void)adjustPlaybackSignalVolume:(int)volume;

/**
 Adjusts the video volume for publishing to remote users.
 
 @note Call this method after joining the channel when you want to publish
 the video to remote users.
 
 @param volume The playback volume. The value ranges between 0 and 400.
 
 @note
 - The value of `volume` is not measured in decibels but in 1. Agora defines
 original volume to 100.
 - 400: (Maximum) Four times the original volume with signal clipping protection.
 - 0: Mute
 */
-(void)adjustPublishSignalVolume:(int)volume;
/**
 Publishes the playback audio to remote users.
 
 This method publishes the playback audio to remote users in the channel.
 */
-(void)publishAudio;

/**
 Stops publishing the playback audio to remote users.
 
 This method stops publishing the playback audio to remote users in the channel.
 */
-(void)unpublishAudio;
/**
 Publishes the playback video to remote users.
 
 This method publishes the playback video to remote users in the channel.
 */
-(void)publishVideo;
/**
 Stops publishing the playback video to remote users.
 
 This method stops publishing the playback video to remote users in the channel.
 */
-(void)unpublishVideo;
@end

NS_ASSUME_NONNULL_END
