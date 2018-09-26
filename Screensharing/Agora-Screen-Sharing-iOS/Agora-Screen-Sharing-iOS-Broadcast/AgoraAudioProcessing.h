//
//  AGVideoPreProcessing.h
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by Alex Zheng on 7/28/16.
//  Copyright © 2016 Agora.io All rights reserved.
//

#import <CoreMedia/CoreMedia.h>
#import <AgoraRtcEngineKit/AgoraRtcEngineKit.h>

@interface AgoraAudioProcessing : NSObject
+ (void)registerAudioPreprocessing:(AgoraRtcEngineKit*) kit;
+ (void)deregisterAudioPreprocessing:(AgoraRtcEngineKit*) kit;
+ (void)pushAudioAppBuffer:(CMSampleBufferRef)sampleBuffer;
+ (void)pushAudioMicBuffer:(CMSampleBufferRef)sampleBuffer;
@end
