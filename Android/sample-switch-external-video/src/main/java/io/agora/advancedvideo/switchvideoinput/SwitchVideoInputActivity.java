package io.agora.advancedvideo.switchvideoinput;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.media.projection.MediaProjectionManager;
import android.os.Environment;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.RelativeLayout;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;

import java.io.File;
import java.util.Locale;

import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.advancedvideo.externvideosource.ExternalVideoInputManager;
import io.agora.advancedvideo.externvideosource.ExternalVideoInputService;
import io.agora.advancedvideo.externvideosource.IExternalVideoInputService;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class SwitchVideoInputActivity extends BaseLiveActivity implements ExternalVideoInputDialog.OnSelectVideoInputListener {
    private static final String TAG = SwitchVideoInputActivity.class.getSimpleName();
    private static final String VIDEO_NAME = "localVideo.mp4";
    private static final int PROJECTION_REQ_CODE = 1 << 2;
    private static final int DEFAULT_VIDEO_TYPE = ExternalVideoInputManager.TYPE_LOCAL_VIDEO;
    private static final int DEFAULT_SHARE_FRAME_RATE = 15;

    // The developers should defines their video dimension, for the
    // video info cannot be obtained before the video is extracted.
    private static final int LOCAL_VIDEO_WIDTH = 1280;
    private static final int LOCAL_VIDEO_HEIGHT = 720;

    private boolean mLocalVideoExists = false;
    private String mLocalVideoPath;
    private int mCurVideoSource = DEFAULT_VIDEO_TYPE;
    private IExternalVideoInputService mService;
    private VideoInputServiceConnection mServiceConnection;

    private RelativeLayout mPreviewLayout;

    private AlertDialog mWarningDialog;
    private ExternalVideoInputDialog mVideoInputDialog;

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
                    videoContainer, true);
            mPreviewLayout = layout.findViewById(
                    R.id.switch_video_preview_layout);
            bindVideoService();
            checkLocalVideo();
        }
    }

    private void addLocalPreview() {
        // Currently only local video sharing needs
        // a local preview.
        TextureView textureView = application().localPreview();

        mPreviewLayout.removeAllViews();
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

    private void checkLocalVideo() {
        // If the video file does not exist, the option
        // will not be selectable
        File dir = getExternalFilesDir(Environment.DIRECTORY_MOVIES);
        File videoFile = new File(dir, VIDEO_NAME);
        mLocalVideoPath = videoFile.getAbsolutePath();
        mLocalVideoExists = videoFile.exists();
        if (!mLocalVideoExists) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.alert_no_local_video_title);
            String format = getResources().getString(R.string.alert_no_local_video_message);
            String message = String.format(Locale.getDefault(), format, videoFile.getParent());
            builder.setMessage(message);
            builder.setPositiveButton(R.string.confirm, null);
            builder.setCancelable(true);
            mWarningDialog = builder.create();
            mWarningDialog.show();
        }
    }

    private void joinChannel(String channel) {
        rtcEngine().joinChannel(null, channel, null, 0);
    }

    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
        if (mIsBroadcaster) return;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SurfaceView surfaceView = RtcEngine.
                        CreateRendererView(SwitchVideoInputActivity.this);
                rtcEngine().setupRemoteVideo(new VideoCanvas(
                        surfaceView, VideoCanvas.RENDER_MODE_FIT, uid));
                videoContainer.addView(surfaceView);
            }
        });
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        if (mIsBroadcaster) return;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                videoContainer.removeAllViews();
            }
        });
    }

    @Override
    protected void onLeaveButtonClicked(View view) {
        // No special work to do here
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

    protected void onMoreButtonClicked(View view) {
        if (!mIsBroadcaster || (mVideoInputDialog != null &&
                mVideoInputDialog.isShowing())) {
            return;
        }

        mVideoInputDialog = new ExternalVideoInputDialog(this, view, this);
        mVideoInputDialog.setLocalVideoEnabled(mLocalVideoExists);
        mVideoInputDialog.show();
    }

    @Override
    public void finish() {
        rtcEngine().leaveChannel();
        removeLocalPreview();
        unbindVideoService();

        if (mWarningDialog != null && mWarningDialog.isShowing()) {
            mWarningDialog.dismiss();
        }

        if (mVideoInputDialog != null && mVideoInputDialog.isShowing()) {
            mVideoInputDialog.dismiss();
        }

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
    public void onVideoInputSelected(int type) {
        if (mService == null) return;
        setVideoInput(type);
    }

    private void setVideoInput(int type) {
        Intent intent = new Intent();
        if (type == ExternalVideoInputManager.TYPE_LOCAL_VIDEO) {
            // Video dimension should be confirmed by the developers.
            // The width and height of the video cannot be acquired before
            // the video is extracted.
            setVideoConfig(ExternalVideoInputManager.TYPE_LOCAL_VIDEO,
                    LOCAL_VIDEO_WIDTH, LOCAL_VIDEO_HEIGHT);
            intent.putExtra(ExternalVideoInputManager.FLAG_VIDEO_PATH, mLocalVideoPath);

            try {
                if (mService.setExternalVideoInput(type, intent)) {
                    mCurVideoSource = type;
                    addLocalPreview();
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        } else if (type == ExternalVideoInputManager.TYPE_SCREEN_SHARE) {
            removeLocalPreview();
            // Responds to the screen share request in
            // onActivityResult callback method.
            requestMediaProjection();
        } else if (type == ExternalVideoInputManager.TYPE_AR_CORE) {

        }
    }

    private void requestMediaProjection() {
        MediaProjectionManager mpm = (MediaProjectionManager)
                getSystemService(Context.MEDIA_PROJECTION_SERVICE);
        Intent intent = mpm.createScreenCaptureIntent();
        startActivityForResult(intent, PROJECTION_REQ_CODE);
    }

    private class VideoInputServiceConnection implements ServiceConnection {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            mService = (IExternalVideoInputService) iBinder;
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mService = null;
        }
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