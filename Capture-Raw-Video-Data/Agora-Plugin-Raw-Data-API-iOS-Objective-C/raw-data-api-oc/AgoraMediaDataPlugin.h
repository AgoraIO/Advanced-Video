//
//  AgoraMediaDataPlugin.h
//  OpenVideoCall
//
//  Created by CavanSu on 26/02/2018.
//  Copyright © 2018 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AgoraRtcEngineKit/AgoraRtcEngineKit.h>
#import "AgoraVideoRawData.h"
#import "AgoraAudioRawData.h"

@class AgoraMediaDataPlugin;
@protocol AgoraVideoDataPluginDelegate <NSObject>
@optional
- (AgoraVideoRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didCapturedVideoRawData:(AgoraVideoRawData *)videoRawData;
- (AgoraVideoRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willRenderVideoRawData:(AgoraVideoRawData *)videoRawData;
@end

@protocol AgoraAudioDataPluginDelegate <NSObject>
@optional
- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didRecordAudioRawData:(AgoraAudioRawData *)audioRawData;
- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willPlaybackAudioRawData:(AgoraAudioRawData *)audioRawData;
- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willPlaybackBeforeMixingAudioRawData:(AgoraAudioRawData *)audioRawData;
- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didMixedAudioRawData:(AgoraAudioRawData *)audioRawData;
@end

@interface AgoraMediaDataPlugin : NSObject
@property (nonatomic, weak) id<AgoraVideoDataPluginDelegate> videoDelegate;
@property (nonatomic, weak) id<AgoraAudioDataPluginDelegate> audioDelegate;
+ (instancetype)mediaDataPluginWithAgoraKit:(AgoraRtcEngineKit *)agoraKit;
// If called setVideoDelegate:(delegate != nil), after you can call following methods.
- (void)screenShotDidCaptureLocalWithImage:(void (^)(UIImage *image))completion;
- (void)screenShotWillRenderWithUid:(NSUInteger)uid image:(void (^)(UIImage *image))completion;
@end
