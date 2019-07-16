//
//  RTCVideoFrame.h
//  OpenLive
//
//  Created by zhanxiaochao on 2019/7/3.
//  Copyright © 2019年 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RTCMacros.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, RTCVideoRotation) {
    RTCVideoRotation_0 = 0,
    RTCVideoRotation_90 = 90,
    RTCVideoRotation_180 = 180,
    RTCVideoRotation_270 = 270,
};
@protocol RTCVideoFrameBuffer;

// RTCVideoFrame is an ObjectiveC version of webrtc::VideoFrame.
RTC_EXPORT

@interface RTCVideoFrame : NSObject

/** Width without rotation applied. */
@property(nonatomic, readonly) int width;

/** Height without rotation applied. */
@property(nonatomic, readonly) int height;
@property(nonatomic, readonly) RTCVideoRotation rotation;

/** Timestamp in nanoseconds. */
@property(nonatomic, readonly) int64_t timeStampNs;

/** Timestamp 90 kHz. */
@property(nonatomic, assign) int32_t timeStamp;

@property(nonatomic, readonly) id<RTCVideoFrameBuffer> buffer;

- (instancetype)init NS_UNAVAILABLE;
- (instancetype) new NS_UNAVAILABLE;
/** Initialize an RTCVideoFrame from a frame buffer, rotation, and timestamp.
 */
- (instancetype)initWithBuffer:(id<RTCVideoFrameBuffer>)frameBuffer
                      rotation:(RTCVideoRotation)rotation
                   timeStampNs:(int64_t)timeStampNs;

- (RTCVideoFrame *)newI420VideoFrame;
@end
NS_ASSUME_NONNULL_END
