package io.agora.rtc.plugin.rawdata;

/**
 * Created by yt on 2018/3/5/005.
 */

public interface MediaDataAudioObserver {

    void onRecordAudioFrame(byte[] data, int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

    void onPlaybackAudioFrame(byte[] data,int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

    void onPlaybackAudioFrameBeforeMixing(byte[] data,int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);

    void onMixedAudioFrame(byte[] data,int videoType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength);
}
