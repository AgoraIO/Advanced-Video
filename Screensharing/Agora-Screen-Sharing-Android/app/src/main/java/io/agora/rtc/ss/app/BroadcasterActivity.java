package io.agora.rtc.ss.app;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;

import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.Constants;
import io.agora.rtc.ss.ScreenSharingClient;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class BroadcasterActivity extends Activity{

    private static final String LOG_TAG = BroadcasterActivity.class.getSimpleName();

    private RtcEngine mRtcEngine;
    private FrameLayout mFlCam;
    private FrameLayout mFlSS;
    private boolean mSS = false;
    private VideoEncoderConfiguration mVEC;
    private ScreenSharingClient mSSClient;

    private final ScreenSharingClient.IStateListener mListener = new ScreenSharingClient.IStateListener() {
        @Override
        public void onError(int error) {
            Log.e(LOG_TAG, "Screen share service error happened: " + error);
        }

        @Override
        public void onTokenWillExpire() {
            Log.d(LOG_TAG, "Screen share service token will expire");
            mSSClient.renewToken(null); // Replace the token with your valid token
        }
    };

    private final IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() {

        @Override
        public void onUserOffline(int uid, int reason) {
            Log.d(LOG_TAG, "onUserOffline: " + uid + " reason: " + reason);
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.d(LOG_TAG, "onJoinChannelSuccess: " + channel + " " + elapsed);
        }

        @Override
        public void onUserJoined(final int uid, int elapsed) {
            Log.d(LOG_TAG, "onUserJoined: " + (uid&0xFFFFFFL));
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if(uid == Constant.SCREEN_SHARE_UID) {
                        setupRemoteView(uid);
                    }
                }
            });
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_broadcaster);

        mFlCam = (FrameLayout) findViewById(R.id.camera_preview);
        mFlSS = (FrameLayout) findViewById(R.id.screen_share_preview);

        mSSClient = ScreenSharingClient.getInstance();
        mSSClient.setListener(mListener);

        mVEC = new VideoEncoderConfiguration(VideoEncoderConfiguration.VD_840x480,
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_24,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE);
    }

    private void initAgoraEngineAndJoinChannel() {
        initializeAgoraEngine();
        setupVideoProfile();
        setupLocalVideo();
        joinChannel();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (mRtcEngine != null) {
            leaveChannel();
            RtcEngine.destroy();
            mRtcEngine = null;
        }

        if (mSS) {
            mSSClient.stop(getApplicationContext());
        }
    }

    public void onCameraSharingClicked(View view) {
        Button button = (Button) view;
        boolean selected = button.isSelected();
        button.setSelected(!selected);

        if (button.isSelected()) {
            initAgoraEngineAndJoinChannel();
            button.setText(getResources().getString(R.string.label_stop_camera));
        } else {
            leaveChannel();
            button.setText(getResources().getString(R.string.label_start_camera));
        }

        mRtcEngine.enableLocalVideo(button.isSelected());
    }

    public void onScreenSharingClicked(View view) {
        Button button = (Button) view;
        boolean selected = button.isSelected();
        button.setSelected(!selected);

        if (button.isSelected()) {
            mSSClient.start(getApplicationContext(), getResources().getString(R.string.agora_app_id), null,
                    getResources().getString(R.string.label_channel_name), Constant.SCREEN_SHARE_UID, mVEC);
            button.setText(getResources().getString(R.string.label_stop_sharing_your_screen));
            mSS = true;
        } else {
            mSSClient.stop(getApplicationContext());
            button.setText(getResources().getString(R.string.label_start_sharing_your_screen));
            mSS = false;
        }
    }

    private void initializeAgoraEngine() {
        try {
            mRtcEngine = RtcEngine.create(getApplicationContext(), getString(R.string.agora_app_id), mRtcEventHandler);
        } catch (Exception e) {
            Log.e(LOG_TAG, Log.getStackTraceString(e));

            throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
        }
    }

    private void setupVideoProfile() {
        mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        mRtcEngine.enableVideo();
        mRtcEngine.setVideoEncoderConfiguration(mVEC);
        mRtcEngine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
    }

    private void setupLocalVideo() {
        SurfaceView camV = RtcEngine.CreateRendererView(getApplicationContext());
        camV.setZOrderOnTop(true);
        camV.setZOrderMediaOverlay(true);
        mFlCam.addView(camV, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        mRtcEngine.setupLocalVideo(new VideoCanvas(camV, VideoCanvas.RENDER_MODE_FIT, Constant.CAMERA_UID));
        mRtcEngine.enableLocalVideo(false);
    }

    private void setupRemoteView(int uid) {
        SurfaceView ssV = RtcEngine.CreateRendererView(getApplicationContext());
        ssV.setZOrderOnTop(true);
        ssV.setZOrderMediaOverlay(true);
        mFlSS.addView(ssV, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        mRtcEngine.setupRemoteVideo(new VideoCanvas(ssV, VideoCanvas.RENDER_MODE_FIT, uid));
    }

    private void joinChannel() {
        mRtcEngine.joinChannel(null, getResources().getString(R.string.label_channel_name),"Extra Optional Data", Constant.CAMERA_UID); // if you do not specify the uid, we will generate the uid for you
    }

    private void leaveChannel() {
        mRtcEngine.leaveChannel();
    }
}
