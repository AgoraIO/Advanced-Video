package io.agora.advancedvideo.screenshare;

import android.content.Context;
import android.content.Intent;
import android.media.projection.MediaProjectionManager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.widget.RelativeLayout;

import androidx.annotation.Nullable;

import io.agora.advancedvideo.Constants;
import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.rtc.RtcEngine;

import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class ScreenShareActivity extends BaseLiveActivity {
    private static final String TAG = ScreenShareActivity.class.getSimpleName();
    private static final int PROJECTION_REQ_CODE = 1;

    private View mVideoViewLayout;

    private Intent mServiceIntent;

    @Override
    protected void onInitializeVideo() {
        initView(mVideoContainer);
        setVideoConfig();
        joinChannel(config().getChannelName());
    }

    private void initView(RelativeLayout container) {
        LayoutInflater inflater = LayoutInflater.from(this);
        mVideoViewLayout = inflater.inflate(
                R.layout.share_video_layout, container, false);
        container.addView(mVideoViewLayout,
                RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
    }

    private void setVideoConfig() {
        rtcEngine().setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                Constants.VIDEO_DIMENSIONS[config().getVideoDimenIndex()],
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT
        ));
    }

    private void joinChannel(String channel) {
        rtcEngine().joinChannel(null, channel, null, 0);
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        Log.i(TAG, "onJoinSuccess channel:" + channel + " uid:" + (uid & 0xFFFFFFFFL));
        if (mIsBroadcaster) {
            requestMediaProjection();
        } else {
            // Audience will initialize remote video preview
            // when they first receive a decoded remote
            // video frame
        }
    }

    private void requestMediaProjection() {
        MediaProjectionManager mpm = (MediaProjectionManager)
                getSystemService(Context.MEDIA_PROJECTION_SERVICE);
        Intent intent = mpm.createScreenCaptureIntent();
        startActivityForResult(intent, PROJECTION_REQ_CODE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PROJECTION_REQ_CODE && resultCode == RESULT_OK) {
            startShareService(data);
        }
    }

    private void startShareService(Intent data) {
        mServiceIntent = new Intent(data);
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        mServiceIntent.putExtra(ScreenShareService.FLAG_SCREEN_WIDTH, metrics.widthPixels);
        mServiceIntent.putExtra(ScreenShareService.FLAG_SCREEN_HEIGHT, metrics.heightPixels);
        mServiceIntent.putExtra(ScreenShareService.FLAG_SCREEN_DPI, metrics.densityDpi);
        mServiceIntent.setClass(this, ScreenShareService.class);
        startService(mServiceIntent);
    }

    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
        Log.i(TAG, "onFirstRemoteVideoDecoded: uid " + (uid & 0xFFFFFFFFL));

        if (mIsBroadcaster) {
            return;
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SurfaceView surfaceView = RtcEngine.
                        CreateRendererView(ScreenShareActivity.this);
                rtcEngine().setupRemoteVideo(new VideoCanvas(
                        surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
                ((RelativeLayout)mVideoViewLayout).addView(surfaceView);
            }
        });

    }

    @Override
    public void onUserJoined(int uid, int elapsed) {
        Log.i(TAG, "onUserJoined: uid " + (uid & 0xFFFFFFFFL));
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        if (mIsBroadcaster) {
            return;
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((RelativeLayout)mVideoViewLayout).removeAllViews();
            }
        });
    }

    @Override
    protected void onLeaveButtonClicked(View view) {

    }

    @Override
    public void finish() {
        leaveChannel();
        stopShareService();
        super.finish();
    }

    private void leaveChannel() {
        rtcEngine().leaveChannel();
    }

    private void stopShareService() {
        if (mServiceIntent != null) {
            stopService(mServiceIntent);
            mServiceIntent = null;
        }
    }

    @Override
    protected void onSwitchCameraButtonClicked(View view) {
        // Screen sharing will ignore this event
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

}
