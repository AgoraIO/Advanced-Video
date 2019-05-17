//
//  AgoraAudioResample.h
//  Agora-Screen-Sharing-iOS-Broadcast
//
//  Created by GongYuhua on 2019/4/22.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

@interface AgoraAudioResample : NSObject
- (instancetype)initWithTargetSampleRate:(Float64)targetSampleRate;
- (void)resamplingBuffer:(CMSampleBufferRef)sampleBuffer
              completion:(void (^)(unsigned char *audioFrame, int frameSize))completion;
@end
