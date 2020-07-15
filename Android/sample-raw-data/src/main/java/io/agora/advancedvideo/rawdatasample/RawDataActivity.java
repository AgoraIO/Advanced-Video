package io.agora.advancedvideo.rawdatasample;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import io.agora.advancedvideo.Constants;
import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.advancedvideo.rawdata.MediaDataAudioObserver;
import io.agora.advancedvideo.rawdata.MediaDataObserverPlugin;
import io.agora.advancedvideo.rawdata.MediaDataVideoObserver;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class RawDataActivity extends BaseLiveActivity implements MediaDataVideoObserver, MediaDataAudioObserver {
    private static String TAG = "RawData";

    private FrameLayout mLocalPreview;
    private FrameLayout mRemotePreview;

    private MediaDataObserverPlugin mediaDataObserverPlugin;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mediaDataObserverPlugin = MediaDataObserverPlugin.the();
        mediaDataObserverPlugin.registerAVRawDataObserver(rtcEngine().getNativeHandle(), false, true);
        mediaDataObserverPlugin.addVideoObserver(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mediaDataObserverPlugin != null) {
            mediaDataObserverPlugin.unRegisterAVRawDataObserver(rtcEngine().getNativeHandle(), false, true);
            mediaDataObserverPlugin.removeVideoObserver(this);
        }
        rtcEngine().leaveChannel();
    }

    @Override
    protected void onInitializeVideo() {
        initView();
        setVideoConfig();
        setupLocalVideo();
        rtcEngine().joinChannel(token(), config().getChannelName(), null, 0);
    }

    private void initView() {
        LayoutInflater inflater = LayoutInflater.from(this);
        View layout = inflater.inflate(R.layout.activity_rawdata, videoContainer, false);
        videoContainer.addView(layout);

        mLocalPreview = videoContainer.findViewById(R.id.local_preview_layout);
        mRemotePreview = videoContainer.findViewById(R.id.remote_preview_layout);
    }

    private void setVideoConfig() {
        rtcEngine().setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                Constants.VIDEO_DIMENSIONS[config().getVideoDimenIndex()],
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT
        ));
    }

    private void setupLocalVideo() {
        if (!mIsBroadcaster) {
            return;
        }

        SurfaceView surface = RtcEngine.CreateRendererView(this);
        rtcEngine().setupLocalVideo(new VideoCanvas(surface, VideoCanvas.RENDER_MODE_HIDDEN, 0));
        mLocalPreview.addView(surface);
    }

    private void setupRemoteVideo(int uid) {
        if (mRemotePreview.getChildCount() >= 1) {
            return;
        }

        SurfaceView surface = RtcEngine.CreateRendererView(this);
        surface.setZOrderOnTop(true);
        rtcEngine().setupRemoteVideo(new VideoCanvas(surface, VideoCanvas.RENDER_MODE_HIDDEN, uid));
        mRemotePreview.addView(surface);
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        super.onJoinChannelSuccess(channel, uid, elapsed);
        Log.i(TAG, String.format("onJoinChannelSuccess %s %d", channel, uid));
    }

    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
        super.onFirstRemoteVideoDecoded(uid, width, height, elapsed);
        Log.i(TAG, String.format("onFirstRemoteVideoDecoded %d %d %d", uid, width, height));
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mediaDataObserverPlugin != null) {
                    mediaDataObserverPlugin.addDecodeBuffer(uid);
                }
                setupRemoteVideo(uid);
            }
        });
    }

    @Override
    public void onUserJoined(final int uid, int elapsed) {
        super.onUserJoined(uid, elapsed);
        Log.i(TAG, String.format("onUserJoined %d", uid));
    }

    @Override
    public void onUserOffline(final int uid, int reason) {
        super.onUserOffline(uid, reason);
        Log.i(TAG, String.format("onUserOffline %d", uid));
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mediaDataObserverPlugin != null) {
                    mediaDataObserverPlugin.removeDecodeBuffer(uid);
                }
                mRemotePreview.removeAllViews();
            }
        });
    }

    @Override
    protected void onLeaveButtonClicked(View view) {

    }

    @Override
    protected void onSwitchCameraButtonClicked(View view) {
        rtcEngine().switchCamera();
    }

    @Override
    protected void onMuteAudioButtonClicked(View view) {
        rtcEngine().muteLocalAudioStream(view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    protected void onMuteVideoButtonClicked(View view) {
        rtcEngine().muteLocalVideoStream(view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    protected void onMoreButtonClicked(View view) {

    }

    @Override
    public void onCaptureVideoFrame(byte[] data, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {
        Bitmap bmp = YUVUtils.blur(this, YUVUtils.i420ToBitmap(width, height, rotation, bufferLength, data, yStride, uStride, vStride), 10);
        // copy the new byte array
        System.arraycopy(YUVUtils.bitmapToI420(width, height, bmp), 0, data, 0, bufferLength);
    }

    @Override
    public void onRenderVideoFrame(int uid, byte[] data, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {
        Log.i(TAG, String.format("onRenderVideoFrame uid %d width %d height %d bufferLength %d", uid, width, height, bufferLength));
    }

    @Override
    public void onRecordAudioFrame(byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }

    @Override
    public void onPlaybackAudioFrame(byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }

    @Override
    public void onPlaybackAudioFrameBeforeMixing(int uid, byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }

    @Override
    public void onMixedAudioFrame(byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }
}
