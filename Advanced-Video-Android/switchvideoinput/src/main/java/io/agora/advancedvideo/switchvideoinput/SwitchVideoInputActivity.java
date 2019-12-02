package io.agora.advancedvideo.switchvideoinput;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.media.projection.MediaProjectionManager;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;

import androidx.annotation.Nullable;

import io.agora.advancedvideo.Constants;
import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.advancedvideo.externvideosource.ExternalVideoInputManager;
import io.agora.advancedvideo.externvideosource.ExternalVideoInputService;
import io.agora.advancedvideo.externvideosource.IExternalVideoInputService;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class SwitchVideoInputActivity extends BaseLiveActivity implements View.OnClickListener {
    private static final String TAG = SwitchVideoInputActivity.class.getSimpleName();
    private static final String VIDEO_PATH = "/sdcard/Android_Runtime_Google_IO_19.mp4";
    private static final int PROJECTION_REQ_CODE = 1 << 2;
    private static final int DEFAULT_VIDEO_TYPE = ExternalVideoInputManager.TYPE_SCREEN_SHARE;
    private static final int DEFAULT_SHARE_FRAME_RATE = 15;

    private int mCurVideoSource = DEFAULT_VIDEO_TYPE;
    private IExternalVideoInputService mService;
    private VideoInputServiceConnection mServiceConnection;

    private RelativeLayout mPreviewLayout;

    @Override
    protected void onInitializeVideo() {
        initLayout();
        joinChannel(config().getChannelName());
    }

    private void initLayout() {
        if (mIsBroadcaster) {
            LayoutInflater inflater = LayoutInflater.from(this);
            View layout = inflater.inflate(
                    R.layout.switch_video_broadcaster_layout,
                    mVideoContainer, true);
            Button switchBtn = layout.findViewById(R.id.switch_video_input_btn);
            switchBtn.setOnClickListener(this);

            mPreviewLayout = layout.findViewById(
                    R.id.switch_video_preview_layout);
            bindVideoService();
        }
    }

    private void addLocalPreview() {
        // Currently only local video sharing needs
        // a local preview.
        TextureView textureView = application().localPreview();
        mPreviewLayout.addView(textureView,
                RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
    }

    private void bindVideoService() {
        Intent intent = new Intent();
        intent.setClass(this, ExternalVideoInputService.class);
        mServiceConnection = new VideoInputServiceConnection();
        bindService(intent, mServiceConnection, Context.BIND_AUTO_CREATE);
    }

    private void joinChannel(String channel) {
        rtcEngine().joinChannel(null, channel, null, 0);
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
                        CreateRendererView(SwitchVideoInputActivity.this);
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
        rtcEngine().muteLocalAudioStream(view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    protected void onMuteVideoButtonClicked(View view) {
        rtcEngine().muteLocalVideoStream(view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    public void finish() {
        rtcEngine().leaveChannel();
        removeLocalPreview();
        unbindVideoService();
        super.finish();
    }

    private void removeLocalPreview() {
        if (hasLocalVideoSharePreview()) {
            mPreviewLayout.removeAllViews();
        }
    }

    private boolean hasLocalVideoSharePreview() {
        return mCurVideoSource == ExternalVideoInputManager.TYPE_LOCAL_VIDEO &&
                mIsBroadcaster && mPreviewLayout != null &&
                mPreviewLayout.getChildCount() > 0;
    }

    private void unbindVideoService() {
        if (mIsBroadcaster && mServiceConnection != null) {
            unbindService(mServiceConnection);
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.switch_video_input_btn:
                switchVideoInput();
                break;
        }
    }

    private void switchVideoInput() {
        if (mService != null) {
            int input = nextVideoInput();
            setVideoInput(input);
            mCurVideoSource = input;
        }
    }

    private int nextVideoInput() {
        int input = mCurVideoSource;
        if (input == ExternalVideoInputManager.TYPE_SCREEN_SHARE) {
            input = ExternalVideoInputManager.TYPE_LOCAL_VIDEO;
        } else {
            input += 1;
        }

        return input;
    }

    private class VideoInputServiceConnection implements ServiceConnection {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            mService = (IExternalVideoInputService) iBinder;
            setVideoInput(DEFAULT_VIDEO_TYPE);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mService = null;
        }
    }

    private void setVideoInput(int type) {
        if (mService == null) {
            return;
        }

        Intent intent = new Intent();
        if (type == ExternalVideoInputManager.TYPE_LOCAL_VIDEO) {
            addLocalPreview();
            // Video dimension should be confirmed by the developers.
            // The width and height of the video cannot be acquired before
            // the video is extracted.
            setVideoConfig(ExternalVideoInputManager.TYPE_LOCAL_VIDEO, 1080, 720);
            intent.putExtra(ExternalVideoInputManager.FLAG_VIDEO_PATH, VIDEO_PATH);
        } else if (type == ExternalVideoInputManager.TYPE_SCREEN_SHARE) {
            removeLocalPreview();
            // Responds to the screen share request in
            // onActivityResult callback method.
            requestMediaProjection();
            return;
        }

        try {
            mCurVideoSource = type;
            mService.setExternalVideoInput(type, intent);
        } catch (RemoteException e) {
            e.printStackTrace();
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
            startScreenShare(data);
        }
    }

    private void startScreenShare(Intent data) {
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        data.putExtra(ExternalVideoInputManager.FLAG_SCREEN_WIDTH, metrics.widthPixels);
        data.putExtra(ExternalVideoInputManager.FLAG_SCREEN_HEIGHT, metrics.heightPixels);
        data.putExtra(ExternalVideoInputManager.FLAG_SCREEN_DPI, (int) metrics.density);
        data.putExtra(ExternalVideoInputManager.FLAG_FRAME_RATE, DEFAULT_SHARE_FRAME_RATE);

        setVideoConfig(ExternalVideoInputManager.TYPE_SCREEN_SHARE, metrics.widthPixels, metrics.heightPixels);
        try {
            mService.setExternalVideoInput(ExternalVideoInputManager.TYPE_SCREEN_SHARE, data);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    private void setVideoConfig(int sourceType, int width, int height) {
        VideoEncoderConfiguration.ORIENTATION_MODE mode;
        switch (sourceType) {
            case ExternalVideoInputManager.TYPE_LOCAL_VIDEO:
                mode = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_LANDSCAPE;
                break;
            case ExternalVideoInputManager.TYPE_SCREEN_SHARE:
                mode = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT;
                break;
            default:
                mode = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
                break;

        }

        rtcEngine().setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                new VideoEncoderConfiguration.VideoDimensions(width, height),
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE, mode
        ));
    }
}
