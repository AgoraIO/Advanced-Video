//
//  RTCVideoFrameBuffer.h
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2019/7/4.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RTCMacros.h"
#include "i420_buffer.h"


NS_ASSUME_NONNULL_BEGIN
@protocol RTCI420Buffer;

// RTCVideoFrameBuffer is an ObjectiveC version of webrtc::VideoFrameBuffer.
RTC_EXPORT
@protocol RTCVideoFrameBuffer <NSObject>

@property(nonatomic, readonly) int width;
@property(nonatomic, readonly) int height;

- (id<RTCI420Buffer>)toI420;

@end

/** Protocol for RTCVideoFrameBuffers containing YUV planar data. */
@protocol RTCYUVPlanarBuffer <RTCVideoFrameBuffer>

@property(nonatomic, readonly) int chromaWidth;
@property(nonatomic, readonly) int chromaHeight;
@property(nonatomic, readonly) const uint8_t *dataY;
@property(nonatomic, readonly) const uint8_t *dataU;
@property(nonatomic, readonly) const uint8_t *dataV;
@property(nonatomic, readonly) int strideY;
@property(nonatomic, readonly) int strideU;
@property(nonatomic, readonly) int strideV;

- (instancetype)initWithWidth:(int)width
                       height:(int)height
                        dataY:(const uint8_t *)dataY
                        dataU:(const uint8_t *)dataU
                        dataV:(const uint8_t *)dataV;
- (instancetype)initWithWidth:(int)width height:(int)height;
- (instancetype)initWithWidth:(int)width
                       height:(int)height
                      strideY:(int)strideY
                      strideU:(int)strideU
                      strideV:(int)strideV;

@end

/** Extension of the YUV planar data buffer with mutable data access */
@protocol RTCMutableYUVPlanarBuffer <RTCYUVPlanarBuffer>

@property(nonatomic, readonly) uint8_t *mutableDataY;
@property(nonatomic, readonly) uint8_t *mutableDataU;
@property(nonatomic, readonly) uint8_t *mutableDataV;

@end

/** Protocol for RTCYUVPlanarBuffers containing I420 data */
@protocol RTCI420Buffer <RTCYUVPlanarBuffer>
@end

@interface RTCI420Buffer : NSObject<RTCI420Buffer>
{
rtc::scoped_refptr<I420BufferInterface> _i420Buffer;
}
/** Initialize an RTCI420Buffer with its backing I420BufferInterface. */
- (instancetype)initWithFrameBuffer:(rtc::scoped_refptr<I420BufferInterface>)i420Buffer;
- (rtc::scoped_refptr<I420BufferInterface>)nativeI420Buffer;

@end


NS_ASSUME_NONNULL_END
