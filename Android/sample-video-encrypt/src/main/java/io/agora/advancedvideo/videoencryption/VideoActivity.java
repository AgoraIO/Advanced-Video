package io.agora.advancedvideo.videoencryption;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import io.agora.advancedvideo.Constants;
import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class VideoActivity extends BaseLiveActivity {
    private static final String TAG = VideoActivity.class.getSimpleName();

    private PacketProcessor mProcessor = new PacketProcessor();
    private FrameLayout mLocalPreview;
    private FrameLayout mRemotePreview;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mProcessor.registerProcessing();
    }

    @Override
    protected void onInitializeVideo() {
        initView();
        setVideoConfig();
        setupLocalVideo();
        rtcEngine().joinChannel(null, config().getChannelName(), null, 0);
    }

    private void initView() {
        LayoutInflater inflater = LayoutInflater.from(this);
        View layout = inflater.inflate(R.layout.activity_video, videoContainer, false);
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

    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
        Log.i(TAG, "onJoinChannelSuccess:" + (uid & 0xFFFFFFFFL));
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                setupRemoteVideo(uid);
            }
        });
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
        Log.i(TAG, "onJoinChannelSuccess:" + (uid & 0xFFFFFFFFL));
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        Log.i(TAG, "onUserOffline:" + (uid & 0xFFFFFFFFL));
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mRemotePreview.removeAllViews();
            }
        });
    }

    @Override
    protected void onLeaveButtonClicked(View view) {

    }

    @Override
    public void finish() {
        mProcessor.unregisterProcessing();
        rtcEngine().leaveChannel();
        super.finish();
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
}
