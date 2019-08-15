//
//  AgoraMediaRawData.m
//  OpenVideoCall
//
//  Created by CavanSu on 26/02/2018.
//  Copyright © 2018 Agora. All rights reserved.
//

#import "AgoraMediaDataPlugin.h"
#import <AgoraRtcEngineKit/IAgoraRtcEngine.h>
#import <AgoraRtcEngineKit/IAgoraMediaEngine.h>

typedef NS_ENUM(int, ObserverType) {
    ObserverTypeVideo,
    ObserverTypeAudio
};

typedef void (^imageBlock)(UIImage *image);

@interface AgoraMediaDataPlugin ()
@property (nonatomic, assign) NSUInteger screenShotUid;
@property (nonatomic, weak) AgoraRtcEngineKit *agoraKit;
@property (nonatomic, weak) imageBlock imageBlock;
- (void)yuvToUIImageWithVideoRawData:(AgoraVideoRawData *)data;
@end


class AgoraVideoFrameObserver : public agora::media::IVideoFrameObserver
{
public:
    AgoraMediaDataPlugin *mediaDataPlugin;
    BOOL getOneDidCaptureVideoFrame = false;
    BOOL getOneWillRenderVideoFrame = false;
    unsigned int videoFrameUid = -1;
    
    AgoraVideoRawData* getVideoRawDataWithVideoFrame(VideoFrame& videoFrame)
    {
        AgoraVideoRawData *data = [[AgoraVideoRawData alloc] init];
        data.width = videoFrame.width;
        data.height = videoFrame.height;
        data.yStride = videoFrame.yStride;
        data.uStride = videoFrame.uStride;
        data.vStride = videoFrame.vStride;
        data.rotation = videoFrame.rotation;
        data.renderTimeMs = videoFrame.renderTimeMs;
        data.yBuffer = (char *)videoFrame.yBuffer;
        data.uBuffer = (char *)videoFrame.uBuffer;
        data.vBuffer = (char *)videoFrame.vBuffer;
        return data;
    }
    
    void modifiedVideoFrameWithNewVideoRawData(VideoFrame& videoFrame, AgoraVideoRawData *videoRawData)
    {
        videoFrame.width = videoRawData.width;
        videoFrame.height = videoRawData.height;
        videoFrame.yStride = videoRawData.yStride;
        videoFrame.uStride = videoRawData.uStride;
        videoFrame.vStride = videoRawData.vStride;
        videoFrame.rotation = videoRawData.rotation;
        videoFrame.renderTimeMs = videoRawData.renderTimeMs;
    }
    
    virtual bool onCaptureVideoFrame(VideoFrame& videoFrame) override
    {
        if (!mediaDataPlugin) return true;
        @synchronized(mediaDataPlugin) {
            @autoreleasepool {
                AgoraVideoRawData *newData = nil;
                if ([mediaDataPlugin.videoDelegate respondsToSelector:@selector(mediaDataPlugin:didCapturedVideoRawData:)]) {
                    AgoraVideoRawData *data = getVideoRawDataWithVideoFrame(videoFrame);
                    newData = [mediaDataPlugin.videoDelegate mediaDataPlugin:mediaDataPlugin didCapturedVideoRawData:data];
                    modifiedVideoFrameWithNewVideoRawData(videoFrame, newData);
                    
                    // ScreenShot
                    if (getOneDidCaptureVideoFrame) {
                        getOneDidCaptureVideoFrame = false;
                        [mediaDataPlugin yuvToUIImageWithVideoRawData:newData];
                    }
                }
            }
            return true;
        }
    }
    
    virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame) override
    {
        if (!mediaDataPlugin) return true;
        @synchronized(mediaDataPlugin) {
            @autoreleasepool {
                AgoraVideoRawData *newData = nil;
                if ([mediaDataPlugin.videoDelegate respondsToSelector:@selector(mediaDataPlugin:willRenderVideoRawData:)]) {
                    AgoraVideoRawData *data = getVideoRawDataWithVideoFrame(videoFrame);
                    newData = [mediaDataPlugin.videoDelegate mediaDataPlugin:mediaDataPlugin willRenderVideoRawData:data];
                    modifiedVideoFrameWithNewVideoRawData(videoFrame, newData);
                    
                    // ScreenShot
                    if (getOneWillRenderVideoFrame && videoFrameUid == uid) {
                        getOneWillRenderVideoFrame = false;
                        videoFrameUid = -1;
                        [mediaDataPlugin yuvToUIImageWithVideoRawData:newData];
                    }
                }
            }
            return true;
        }
    }
};

class AgoraAudioFrameObserver : public agora::media::IAudioFrameObserver
{
public:
    AgoraMediaDataPlugin *mediaDataPlugin;

    AgoraAudioRawData* getAudioRawDataWithAudioFrame(AudioFrame& audioFrame)
    {
        AgoraAudioRawData *data = [[AgoraAudioRawData alloc] init];
        data.samples = audioFrame.samples;
        data.bytesPerSample = audioFrame.bytesPerSample;
        data.channels = audioFrame.channels;
        data.samplesPerSec = audioFrame.samplesPerSec;
        data.renderTimeMs = audioFrame.renderTimeMs;
        data.buffer = (char *)audioFrame.buffer;
        data.bufferLength = audioFrame.samples * audioFrame.bytesPerSample;
        return data;
    }
    
    void modifiedAudioFrameWithNewAudioRawData(AudioFrame& audioFrame, AgoraAudioRawData *audioRawData)
    {
        audioFrame.samples = audioRawData.samples;
        audioFrame.bytesPerSample = audioRawData.bytesPerSample;
        audioFrame.channels = audioRawData.channels;
        audioFrame.samplesPerSec = audioRawData.samplesPerSec;
        audioFrame.renderTimeMs = audioRawData.renderTimeMs;
    }
    
    virtual bool onRecordAudioFrame(AudioFrame& audioFrame) override
    {
        @synchronized(mediaDataPlugin) {
            @autoreleasepool {
                if (!mediaDataPlugin) return true;
                if ([mediaDataPlugin.audioDelegate respondsToSelector:@selector(mediaDataPlugin:didRecordAudioRawData:)]) {
                    AgoraAudioRawData *data = getAudioRawDataWithAudioFrame(audioFrame);
                    AgoraAudioRawData *newData = [mediaDataPlugin.audioDelegate mediaDataPlugin:mediaDataPlugin didRecordAudioRawData:data];
                    modifiedAudioFrameWithNewAudioRawData(audioFrame, newData);
                }
            }
            return true;
        }
    }
    
    virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame) override
    {
        @synchronized(mediaDataPlugin) {
            @autoreleasepool {
                if (!mediaDataPlugin) return true;
                if ([mediaDataPlugin.audioDelegate respondsToSelector:@selector(mediaDataPlugin:willPlaybackAudioRawData:)]) {
                    AgoraAudioRawData *data = getAudioRawDataWithAudioFrame(audioFrame);
                    AgoraAudioRawData *newData = [mediaDataPlugin.audioDelegate mediaDataPlugin:mediaDataPlugin willPlaybackAudioRawData:data];
                    modifiedAudioFrameWithNewAudioRawData(audioFrame, newData);
                }
            }
            return true;
        }
    }
    
    virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) override
    {
        @synchronized(mediaDataPlugin) {
            @autoreleasepool {
                if (!mediaDataPlugin) return true;
                if ([mediaDataPlugin.audioDelegate respondsToSelector:@selector(mediaDataPlugin:willPlaybackBeforeMixingAudioRawData:)]) {
                    AgoraAudioRawData *data = getAudioRawDataWithAudioFrame(audioFrame);
                    AgoraAudioRawData *newData = [mediaDataPlugin.audioDelegate mediaDataPlugin:mediaDataPlugin willPlaybackBeforeMixingAudioRawData:data];
                    modifiedAudioFrameWithNewAudioRawData(audioFrame, newData);
                }
            }
            return true;
        }
    }
    
    virtual bool onMixedAudioFrame(AudioFrame& audioFrame) override
    {
        @synchronized(mediaDataPlugin) {
            @autoreleasepool {
                if (!mediaDataPlugin) return true;
                if ([mediaDataPlugin.audioDelegate respondsToSelector:@selector(mediaDataPlugin:didMixedAudioRawData:)]) {
                    AgoraAudioRawData *data = getAudioRawDataWithAudioFrame(audioFrame);
                    AgoraAudioRawData *newData = [mediaDataPlugin.audioDelegate mediaDataPlugin:mediaDataPlugin didMixedAudioRawData:data];
                    modifiedAudioFrameWithNewAudioRawData(audioFrame, newData);
                }
            }
            return true;
        }
    }
};

static AgoraVideoFrameObserver s_videoFrameObserver;
static AgoraAudioFrameObserver s_audioFrameObserver;

@implementation AgoraMediaDataPlugin
    
+ (instancetype)mediaDataPluginWithAgoraKit:(AgoraRtcEngineKit *)agoraKit {
    AgoraMediaDataPlugin *source = [[AgoraMediaDataPlugin alloc] init];
    source.agoraKit = agoraKit;
    if (!agoraKit) {
        return nil;
    }
    return source;
}

- (void)setVideoDelegate:(id<AgoraVideoDataPluginDelegate>)videoDelegate {
     _videoDelegate = videoDelegate;
    if (videoDelegate) {
        [self startVideoRawDataCallback];
    }
    else {
        [self stopVideoRawDataCallback];
    }
}

- (void)setAudioDelegate:(id<AgoraAudioDataPluginDelegate>)audioDelegate {
    _audioDelegate = audioDelegate;
    if (audioDelegate) {
        [self startAudioRawDataCallback];
    }
    else {
        [self stopAudioRawDataCallback];
    }
}

- (void)startVideoRawDataCallback {
    [self registerVideoObserverOrAudioVideoObserver:ObserverTypeVideo];
}

- (void)startAudioRawDataCallback {
    [self registerVideoObserverOrAudioVideoObserver:ObserverTypeAudio];
}

- (void)stopVideoRawDataCallback {
    [self unregisterVideoObserverOrAudioVideoObserver:ObserverTypeVideo];
}

- (void)stopAudioRawDataCallback {
    [self unregisterVideoObserverOrAudioVideoObserver:ObserverTypeAudio];
}

- (void)registerVideoObserverOrAudioVideoObserver:(ObserverType)observerType {
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)self.agoraKit.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine)
    {
        switch (observerType) {
            case ObserverTypeVideo:
                mediaEngine->registerVideoFrameObserver(&s_videoFrameObserver);
                s_videoFrameObserver.mediaDataPlugin = self;
                break;
            case ObserverTypeAudio:
                mediaEngine->registerAudioFrameObserver(&s_audioFrameObserver);
                s_audioFrameObserver.mediaDataPlugin = self;
                break;
            default:
                break;
        }
    }
}

- (void)unregisterVideoObserverOrAudioVideoObserver:(ObserverType)observerType {
    agora::rtc::IRtcEngine* rtc_engine = (agora::rtc::IRtcEngine*)self.agoraKit.getNativeHandle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtc_engine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine)
    {
        switch (observerType) {
            case ObserverTypeVideo:
                mediaEngine->registerVideoFrameObserver(NULL);
                s_videoFrameObserver.mediaDataPlugin = nil;
                break;
            case ObserverTypeAudio:
                mediaEngine->registerAudioFrameObserver(&s_audioFrameObserver);
                s_audioFrameObserver.mediaDataPlugin = nil;
                break;
            default:
                break;
        }
    }
}

- (void)screenShotDidCaptureLocalWithImage:(void (^)(UIImage *image))completion {
    self.imageBlock = completion;
    s_videoFrameObserver.getOneDidCaptureVideoFrame = true;
}

- (void)screenShotWillRenderWithUid:(NSUInteger)uid image:(void (^)(UIImage *image))completion {
    self.imageBlock = completion;
    s_videoFrameObserver.getOneWillRenderVideoFrame = true;
    s_videoFrameObserver.videoFrameUid = (unsigned int)uid;
}

- (void)yuvToUIImageWithVideoRawData:(AgoraVideoRawData *)data {

    int height = data.height;
    int yStride = data.yStride;
    int rotation = data.rotation;
    
    char* yBuffer = data.yBuffer;
    char* uBuffer = data.uBuffer;
    char* vBuffer = data.vBuffer;
    
    int Len = yStride * data.height * 3/2;
    int yLength = yStride * data.height;
    int uLength = yLength / 4;
    
    unsigned char * buf = (unsigned char *)malloc(Len);
    memcpy(buf, yBuffer, yLength);
    memcpy(buf + yLength, uBuffer, uLength);
    memcpy(buf + yLength + uLength, vBuffer, uLength);
    
    unsigned char * NV12buf = (unsigned char *)malloc(Len);
    [self yuv420p_to_nv12:buf nv12:NV12buf width:yStride height:height];
    @autoreleasepool {
        [self UIImageToJpg:NV12buf width:yStride height:height rotation:rotation];
    }
    if(buf != NULL) {
        free(buf);
        buf = NULL;
    }
    
    if(NV12buf != NULL) {
        free(NV12buf);
        NV12buf = NULL;
    }
    
}

// Agora SDK Raw Data format is YUV420P
- (void)yuv420p_to_nv12:(unsigned char*)yuv420p nv12:(unsigned char*)nv12 width:(int)width  height:(int)height{
    int i, j;
    int y_size = width * height;
    
    unsigned char* y = yuv420p;
    unsigned char* u = yuv420p + y_size;
    unsigned char* v = yuv420p + y_size * 5 / 4;
    
    unsigned char* y_tmp = nv12;
    unsigned char* uv_tmp = nv12 + y_size;
    
    // y
    memcpy(y_tmp, y, y_size);
    
    // u
    for (j = 0, i = 0; j < y_size * 0.5; j += 2, i++) {
        // swtich the location of U、V，to NV12 or NV21
#if 1
        uv_tmp[j] = u[i];
        uv_tmp[j+1] = v[i];
#else
        uv_tmp[j] = v[i];
        uv_tmp[j+1] = u[i];
#endif
    }
}

- (void)UIImageToJpg:(unsigned char *)buffer width:(int)width height:(int)height rotation:(int)rotation {
    UIImage *image = [self YUVtoUIImage:buffer width:width height:height rotation:rotation];
    if (self.imageBlock) {
        self.imageBlock(image);
    }
}

//This is API work well for NV12 data format only.
- (UIImage *)YUVtoUIImage:(unsigned char *)buffer width:(int)w height:(int)h rotation:(int)rotation {
    //YUV(NV12)-->CIImage--->UIImage Conversion
    NSDictionary *pixelAttributes = @{(NSString*)kCVPixelBufferIOSurfacePropertiesKey:@{}};
    CVPixelBufferRef pixelBuffer = NULL;
    CVReturn result = CVPixelBufferCreate(kCFAllocatorDefault,
                                          w,
                                          h,
                                          kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
                                          (__bridge CFDictionaryRef)(pixelAttributes),
                                          &pixelBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer,0);
    void *yDestPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);

    // Here y_ch0 is Y-Plane of YUV(NV12) data.
    unsigned char *y_ch0 = buffer;
    unsigned char *y_ch1 = buffer + w * h;
    memcpy(yDestPlane, y_ch0, w * h);
    void *uvDestPlane = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);

    // Here y_ch1 is UV-Plane of YUV(NV12) data.
    memcpy(uvDestPlane, y_ch1, w * h * 0.5);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);

    if (result != kCVReturnSuccess) {
        NSLog(@"Unable to create cvpixelbuffer %d", result);
    }
    
    // CIImage Conversion
    CIImage *coreImage = [CIImage imageWithCVPixelBuffer:pixelBuffer];
    CIContext *temporaryContext = [CIContext contextWithOptions:nil];
    CGImageRef videoImage = [temporaryContext createCGImage:coreImage
                                                       fromRect:CGRectMake(0, 0, w, h)];

    // UIImage Conversion
    UIImageOrientation orientation;
    switch (rotation) {
        case 0:
            orientation = UIImageOrientationUp;
            break;

        case 90:
            orientation = UIImageOrientationRight;
            break;

        case 180:
            orientation = UIImageOrientationDown;
            break;

        case 270:
            orientation = UIImageOrientationLeft;
            break;

        default:
            orientation = UIImageOrientationUp;
            break;
    }
    UIImage *finalImage = [[UIImage alloc] initWithCGImage:videoImage
                                                     scale:1.0
                                               orientation:orientation];
    CVPixelBufferRelease(pixelBuffer);
    CGImageRelease(videoImage);
    return finalImage;
}

@end
