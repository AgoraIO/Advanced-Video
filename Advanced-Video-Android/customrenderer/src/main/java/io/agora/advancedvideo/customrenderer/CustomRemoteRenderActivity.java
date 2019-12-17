package io.agora.advancedvideo.customrenderer;

import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.mediaio.AgoraSurfaceView;
import io.agora.rtc.mediaio.MediaIO;
import io.agora.rtc.video.VideoCanvas;

/**
 * Note that this activity only take into account the
 * one broadcaster, multi audience scenario.
 * The behavior of joining more than one broadcasters
 * is undefined.
 */
public class CustomRemoteRenderActivity extends BaseLiveActivity {
    private static final String TAG = CustomRemoteRenderActivity.class.getSimpleName();

    @Override
    protected void onInitializeVideo() {
        joinChannel();
    }

    private void joinChannel() {
        rtcEngine().joinChannel(null, config().getChannelName(), null, 0);
    }

    @Override
    public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
        Log.i(TAG, "onFirstRemoteVideoDecoded uid:" + (uid & 0xFFFFFFFFL));
        if (!mIsBroadcaster) {
            createRemoteRender(uid);
        }
    }

    private void createRemoteRender(final int uid) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AgoraSurfaceView surfaceView = new AgoraSurfaceView(
                        CustomRemoteRenderActivity.this);
                surfaceView.init(null);
                surfaceView.setZOrderOnTop(false);
                surfaceView.setBufferType(MediaIO.BufferType.BYTE_BUFFER);
                surfaceView.setPixelFormat(MediaIO.PixelFormat.I420);
                mVideoContainer.addView(surfaceView);
                rtcEngine().setRemoteRenderMode(uid, VideoCanvas.RENDER_MODE_HIDDEN);
                rtcEngine().setRemoteVideoRenderer(uid, surfaceView);
            }
        });
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        Log.i(TAG, "onJoinChannelSuccess uid:" + (uid & 0xFFFFFFFFL));
        if (mIsBroadcaster) {
            createLocalPreview();
        }
    }

    private void createLocalPreview() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SurfaceView surfaceView = RtcEngine.CreateRendererView(
                        CustomRemoteRenderActivity.this);
                // This video keeps itself at the bottom of all
                // the other UI elements (buttons, etc).
                surfaceView.setZOrderOnTop(false);
                rtcEngine().setupLocalVideo(new VideoCanvas(
                        surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, 0));
                mVideoContainer.addView(surfaceView);
            }
        });
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        if (!mIsBroadcaster) {
            remoteRemoteUser();
        }
    }

    private void remoteRemoteUser() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mVideoContainer.getChildCount() > 0) {
                    mVideoContainer.removeAllViews();
                }
            }
        });
    }

    @Override
    protected void onLeaveButtonClicked(View view) {

    }

    @Override
    public void finish() {
        Log.i(TAG, "leave channel");
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
