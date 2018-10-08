package io.agora.rtc.ss.app;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.ss.GLRender;
import io.agora.rtc.ss.ImgTexFrame;
import io.agora.rtc.ss.SinkConnector;
import io.agora.rtc.ss.capture.ScreenCapture;
import io.agora.rtc.video.AgoraVideoFrame;

public class HelloAgoraScreenSharingActivity extends Activity {

    private static final String LOG_TAG = "AgoraScreenSharing";

    private static final int PERMISSION_REQ_ID_RECORD_AUDIO = 22;

    private ScreenCapture mScreenCapture;
    private GLRender mScreenGLRender;

    private RtcEngine mRtcEngine;

    private boolean mIsLandSpace = false;

    private void initModules() {
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);

        if (mScreenGLRender == null) {
            mScreenGLRender = new GLRender();
        }
        if (mScreenCapture == null) {
            mScreenCapture = new ScreenCapture(getApplicationContext(), mScreenGLRender, metrics.densityDpi);
        }

        mScreenCapture.mImgTexSrcConnector.connect(new SinkConnector<ImgTexFrame>() {
            @Override
            public void onFormatChanged(Object obj) {
                Log.d(LOG_TAG, "onFormatChanged " + obj.toString());
            }

            @Override
            public void onFrameAvailable(ImgTexFrame frame) {
                Log.d(LOG_TAG, "onFrameAvailable " + frame.toString());

                if (mRtcEngine == null) {
                    return;
                }

                AgoraVideoFrame vf = new AgoraVideoFrame();
                vf.format = AgoraVideoFrame.FORMAT_TEXTURE_OES;
                vf.timeStamp = frame.pts;
                vf.stride = frame.mFormat.mWidth;
                vf.height = frame.mFormat.mHeight;
                vf.textureID = frame.mTextureId;
                vf.syncMode = true;
                vf.eglContext14 = mScreenGLRender.getEGLContext();
                vf.transform = frame.mTexMatrix;

                mRtcEngine.pushExternalVideoFrame(vf);
            }
        });

        mScreenCapture.setOnScreenCaptureListener(new ScreenCapture.OnScreenCaptureListener() {
            @Override
            public void onStarted() {
                Log.d(LOG_TAG, "Screen Record Started");
            }

            @Override
            public void onError(int err) {
                Log.d(LOG_TAG, "onError " + err);
                switch (err) {
                    case ScreenCapture.SCREEN_ERROR_SYSTEM_UNSUPPORTED:
                        break;
                    case ScreenCapture.SCREEN_ERROR_PERMISSION_DENIED:
                        break;
                }
            }
        });

        WindowManager wm = (WindowManager) getApplicationContext()
                .getSystemService(Context.WINDOW_SERVICE);
        int screenWidth = wm.getDefaultDisplay().getWidth();
        int screenHeight = wm.getDefaultDisplay().getHeight();
        if ((mIsLandSpace && screenWidth < screenHeight) ||
                (!mIsLandSpace) && screenWidth > screenHeight) {
            screenWidth = wm.getDefaultDisplay().getHeight();
            screenHeight = wm.getDefaultDisplay().getWidth();
        }

        setOffscreenPreview(screenWidth, screenHeight);

        if (mRtcEngine == null) {
            try {
                mRtcEngine = RtcEngine.create(getApplicationContext(), getString(R.string.agora_app_id), new IRtcEngineEventHandler() {
                    @Override
                    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
                        Log.d(LOG_TAG, "onJoinChannelSuccess " + channel + " " + elapsed);
                    }

                    @Override
                    public void onWarning(int warn) {
                        Log.d(LOG_TAG, "onWarning " + warn);
                    }

                    @Override
                    public void onError(int err) {
                        Log.d(LOG_TAG, "onError " + err);
                    }

                    @Override
                    public void onAudioRouteChanged(int routing) {
                        Log.d(LOG_TAG, "onAudioRouteChanged " + routing);
                    }
                });
            } catch (Exception e) {
                Log.e(LOG_TAG, Log.getStackTraceString(e));

                throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
            }

            mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
            mRtcEngine.enableVideo();

            if (mRtcEngine.isTextureEncodeSupported()) {
                mRtcEngine.setExternalVideoSource(true, true, true);
            } else {
                throw new RuntimeException("Can not work on device do not supporting texture" + mRtcEngine.isTextureEncodeSupported());
            }

            mRtcEngine.setVideoProfile(Constants.VIDEO_PROFILE_360P, true);

            mRtcEngine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        }
    }

    private void deInitModules() {
        RtcEngine.destroy();
        mRtcEngine = null;

        if (mScreenCapture != null) {
            mScreenCapture.release();
            mScreenCapture = null;
        }

        if (mScreenGLRender != null) {
            mScreenGLRender.quit();
            mScreenGLRender = null;
        }
    }

    /**
     * Set offscreen preview.
     *
     * @param width  offscreen width
     * @param height offscreen height
     * @throws IllegalArgumentException
     */
    public void setOffscreenPreview(int width, int height) throws IllegalArgumentException {
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("Invalid offscreen resolution");
        }

        mScreenGLRender.init(width, height);
    }

    private void startCapture() {
        mScreenCapture.start();
    }

    private void stopCapture() {
        mScreenCapture.stop();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hello_agora_screen_sharing);

        checkSelfPermission(Manifest.permission.RECORD_AUDIO, PERMISSION_REQ_ID_RECORD_AUDIO);
    }

    public void onLiveSharingScreenClicked(View view) {
        if (!checkSelfPermission(Manifest.permission.RECORD_AUDIO, PERMISSION_REQ_ID_RECORD_AUDIO)) {
            return;
        }

        Button button = (Button) view;
        boolean selected = button.isSelected();
        button.setSelected(!selected);

        if (button.isSelected()) {
            initModules();
            startCapture();

            String channel = "ss_test" + System.currentTimeMillis();
            channel = "ss_test";

            TextView tv = (TextView) findViewById(R.id.channel_name_info);
            tv.setText(getString(R.string.label_channel_info, channel));

            button.setText(R.string.label_stop_sharing_your_screen);
            mRtcEngine.muteAllRemoteAudioStreams(true);
            mRtcEngine.muteAllRemoteVideoStreams(true);
            mRtcEngine.joinChannel(null, channel, "", 0);
        } else {
            TextView tv = (TextView) findViewById(R.id.channel_name_info);
            tv.setText(R.string.label_hello_agora);

            button.setText(R.string.label_start_sharing_your_screen);

            mRtcEngine.leaveChannel();

            stopCapture();
        }
    }

    public boolean checkSelfPermission(String permission, int requestCode) {
        Log.i(LOG_TAG, "checkSelfPermission " + permission + " " + requestCode);
        if (ContextCompat.checkSelfPermission(this,
                permission)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                    new String[]{permission},
                    requestCode);
            return false;
        }
        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String permissions[], @NonNull int[] grantResults) {
        Log.i(LOG_TAG, "onRequestPermissionsResult " + grantResults[0] + " " + requestCode);

        switch (requestCode) {
            case PERMISSION_REQ_ID_RECORD_AUDIO: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                } else {
                    showLongToast("No permission for " + Manifest.permission.RECORD_AUDIO);
                    finish();
                }
                break;
            }
        }
    }

    public final void showLongToast(final String msg) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_LONG).show();
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        deInitModules();
    }
}
