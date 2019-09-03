#import "RTCVideoFrame.h"
#import "RTCVideoFrameBuffer.h"

@implementation RTCVideoFrame {
    RTCVideoRotation _rotation;
    int64_t _timeStampNs;
}

@synthesize buffer = _buffer;
@synthesize timeStamp;

- (int)width {
    return _buffer.width;
}

- (int)height {
    return _buffer.height;
}

- (RTCVideoRotation)rotation {
    return _rotation;
}

- (int64_t)timeStampNs {
    return _timeStampNs;
}

- (RTCVideoFrame *)newI420VideoFrame {
    return [[RTCVideoFrame alloc] initWithBuffer:[_buffer toI420]
                                        rotation:_rotation
                                     timeStampNs:_timeStampNs];
}


- (instancetype)initWithBuffer:(id<RTCVideoFrameBuffer>)buffer
                      rotation:(RTCVideoRotation)rotation
                   timeStampNs:(int64_t)timeStampNs {
    if (self = [super init]) {
        _buffer = buffer;
        _rotation = rotation;
        _timeStampNs = timeStampNs;
    }
    return self;
}
@end
