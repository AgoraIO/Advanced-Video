package io.agora.advancedvideo.rawdata;

import java.nio.ByteBuffer;

public class MediaPreProcessing {
    static {
        System.loadLibrary("apm-plugin-raw-data");
    }

    interface IAudioFrameObserver {
        void onRecordAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);
        void onPlaybackAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);
        void onPlaybackAudioFrameBeforeMixing(int uid, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);
        void onMixedAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);
    }

    interface IVideoFrameObserver {
        void onCaptureVideoFrame(int videoFrameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs);
        void onRenderVideoFrame(int uid, int videoFrameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs);
    }

    static native void registerAudioFrameObserver(IAudioFrameObserver observer, long rtcEngineNativeHandle);

    static native void unRegisterAudioFrameObserver(long rtcEngineNativeHandle);

    static native void registerVideoFrameObserver(IVideoFrameObserver observer, long rtcEngineNativeHandle);

    static native void unRegisterVideoFrameObserver(long rtcEngineNativeHandle);

    static native void setAudioRecordByteBuffer(ByteBuffer byteBuffer);

    static native void setAudioPlayByteBuffer(ByteBuffer byteBuffer);

    static native void setBeforeAudioMixByteBuffer(ByteBuffer byteBuffer);

    static native void setAudioMixByteBuffer(ByteBuffer byteBuffer);

    static native void setVideoCaptureByteBuffer(ByteBuffer byteBuffer);

    static native void setVideoDecodeByteBuffer(int uid, ByteBuffer byteBuffer);

    static native void release();

}
