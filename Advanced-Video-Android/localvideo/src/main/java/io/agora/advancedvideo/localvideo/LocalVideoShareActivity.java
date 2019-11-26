package io.agora.advancedvideo.localvideo;

import android.util.Log;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.RelativeLayout;

import io.agora.advancedvideo.Constants;
import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.MediaIO;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class LocalVideoShareActivity extends BaseLiveActivity implements LocalVideoCodec.OnFrameTransferListener {
    private static final String TAG = LocalVideoShareActivity.class.getSimpleName();
    private static final String VIDEO_PATH = "/sdcard/Android_Runtime_Google_IO_19.mp4";

    private LocalVideoCodec localVideoCodec = new LocalVideoCodec(VIDEO_PATH, this);
    private LocalVideoSource mVideoSource = new LocalVideoSource();
    private IVideoFrameConsumer mConsumer;

    @Override
    protected void onInitializeVideo() {
        initLayout();
        joinChannel(config().getChannelName());
    }

    private void initLayout() {
        if (mIsBroadcaster) {
            // Broadcasters draw a local preview, and in
            // the meantime share the video
            TextureView textureView = new TextureView(this);
            textureView.setSurfaceTextureListener(localVideoCodec);
            mVideoContainer.addView(textureView,
                    RelativeLayout.LayoutParams.MATCH_PARENT,
                    RelativeLayout.LayoutParams.MATCH_PARENT);
            rtcEngine().setVideoSource(mVideoSource);
            setVideoConfig();
        }
    }

    private void setVideoConfig() {
        rtcEngine().setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                Constants.VIDEO_DIMENSIONS[config().getVideoDimenIndex()],
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_LANDSCAPE
        ));
    }

    private void joinChannel(String channel) {
        rtcEngine().joinChannel(null, channel, null, 0);
    }

    private class LocalVideoSource implements IVideoSource {
        @Override
        public boolean onInitialize(IVideoFrameConsumer consumer) {
            mConsumer = consumer;
            return true;
        }

        @Override
        public boolean onStart() {
            return true;
        }

        @Override
        public void onStop() {

        }

        @Override
        public void onDispose() {
            mConsumer = null;
        }

        @Override
        public int getBufferType() {
            return MediaIO.BufferType.TEXTURE.intValue();
        }
    }

    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
        Log.i(TAG, "onFirstRemoteVideoDecoded:" + (uid & 0xFFFFFFFFL));
        if (mIsBroadcaster) {
            return;
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SurfaceView surfaceView = RtcEngine.
                        CreateRendererView(LocalVideoShareActivity.this);
                rtcEngine().setupRemoteVideo(new VideoCanvas(
                        surfaceView, VideoCanvas.RENDER_MODE_FIT, uid));
                mVideoContainer.addView(surfaceView);
            }
        });
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        Log.i(TAG, "onUserOffline:" + (uid & 0xFFFFFFFFL));
        if (mIsBroadcaster) {
            return;
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mVideoContainer.removeAllViews();
            }
        });
    }

    @Override
    protected void onLeaveButtonClicked(View view) {

    }

    @Override
    protected void onSwitchCameraButtonClicked(View view) {
        // Local video sharing will ignore this event
    }

    @Override
    protected void onMuteAudioButtonClicked(View view) {
        rtcEngine().muteLocalAudioStream(!view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    protected void onMuteVideoButtonClicked(View view) {
        rtcEngine().muteLocalVideoStream(!view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    public void onFrameTransfer(int textureId, int width, int height, float[] matrix) {
        Log.i("LocalVideoShare", "onFrameTransfer");
        if (mConsumer != null) {
            mConsumer.consumeTextureFrame(textureId,
                    MediaIO.PixelFormat.TEXTURE_OES.intValue(),
                    width, height, 0,
                    System.currentTimeMillis(), matrix);
        }
    }

    @Override
    public void finish() {
        rtcEngine().leaveChannel();
        super.finish();
    }
}
