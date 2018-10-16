package io.agora.rtc.plugin.rawdata;


import java.nio.ByteBuffer;

public class MediaPreProcessing {
    static {
        System.loadLibrary("apm-plugin-raw-data-api-java");
    }

    public interface ProgressCallback {

        void onCaptureVideoFrame(int uid, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs);

        void onRenderVideoFrame(int uid, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs);

        void onRecordAudioFrame(int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

        void onPlaybackAudioFrame(int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

        void onPlaybackAudioFrameBeforeMixing(int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

        void onMixedAudioFrame(int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);
    }

    public static native void setCallback(ProgressCallback callback);

    public static native void setVideoCaptureByteBUffer(ByteBuffer byteBuffer);

    public static native void setAudioRecordByteBUffer(ByteBuffer byteBuffer);

    public static native void setAudioPlayByteBUffer(ByteBuffer byteBuffer);

    public static native void setBeforeAudioMixByteBUffer(ByteBuffer byteBuffer);

    public static native void setAudioMixByteBUffer(ByteBuffer byteBuffer);

    public static native void setVideoDecodeByteBUffer(int uid, ByteBuffer byteBuffer);

    public static native void releasePoint();


}
