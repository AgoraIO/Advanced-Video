package io.agora.csopenduo;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.telecom.DisconnectCause;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.*;

import java.util.Locale;

import io.agora.AgoraAPI;
import io.agora.AgoraAPIOnlySignal;
import io.agora.IAgoraAPI;

import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

/**
 * Created by beryl on 2017/11/6.
 */

public class CallActivity extends Activity implements AGApplication.OnAgoraEngineInterface {
    private static final String TAG = CallActivity.class.getSimpleName();
    private static boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    private AgoraAPIOnlySignal mAgoraAPI;
    private RtcEngine mRtcEngine;

    private String mSubscriber;

    private CheckBox mCheckMute;
    private TextView mCallTitle;
    private ImageView mCallHangupBtn;
    private RelativeLayout mLayoutCallIn;

    private FrameLayout mLayoutBigView;
    private FrameLayout mLayoutSmallView;

    private String channelName = "channelid";
    private MediaPlayer mPlayer;
    private int mCallType = -1;
    private boolean mIsCallInRefuse = false;
    private int mRemoteUid = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);
        InitUI();
        initAgoraEngineAndJoinChannel();

    }

    private void InitUI() {
        mCallTitle = (TextView) findViewById(R.id.meet_title);

        mCheckMute = (CheckBox) findViewById(R.id.call_mute_button);
        mCheckMute.setOnCheckedChangeListener(oncheckChangeListerener);

        mCallHangupBtn = (ImageView) findViewById(R.id.call_button_hangup);
        mLayoutCallIn = (RelativeLayout) findViewById(R.id.call_layout_callin);

        mLayoutBigView = (FrameLayout) findViewById(R.id.remote_video_view_container);
        mLayoutSmallView = (FrameLayout) findViewById(R.id.local_video_view_container);
    }

    private void setupData() {
        Intent intent = getIntent();

        mSubscriber = intent.getStringExtra("subscriber");
        channelName = intent.getStringExtra("channelName");
        mCallType = intent.getIntExtra("type", -1);
        if (mCallType == Constant.CALL_OUT) {
            mLayoutCallIn.setVisibility(View.GONE);
            mCallHangupBtn.setVisibility(View.VISIBLE);
            mCallTitle.setText(String.format(Locale.US, "%s is be called...", mSubscriber));

            try {
                mPlayer = MediaPlayer.create(this, R.raw.basic_tones);
                mPlayer.setLooping(true);
                mPlayer.start();
            } catch (Exception e) {
                e.printStackTrace();
            }

            setupLocalVideo();
            joinChannel();
        } else {
            callInPickup();
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Log.i(TAG, "onNewIntent");
        setupData();
    }

    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) { // Tutorial Step 5
        Log.i(TAG, "onFirstRemoteVideoDecoded  uid:" + uid);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mRemoteUid != 0) {
                    return;
                }
                mRemoteUid = uid;
                setupRemoteVideo(uid);
            }
        });
    }

    @Override
    public void onUserOffline(final int uid, int reason) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                onRemoteUserLeft(uid);
            }
        });
    }

    @Override
    public void onUserMuteVideo(final int uid, final boolean muted) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                onRemoteUserVideoMuted(uid, muted);
            }
        });
    }

    private CompoundButton.OnCheckedChangeListener oncheckChangeListerener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            mRtcEngine.muteLocalAudioStream(isChecked);
        }
    };

    public void CallClickInit(View v) {
        switch (v.getId()) {

            case R.id.call_button_hangup: // call out canceled or call ended

                callOutHangup();
                break;
        }
    }

    private void callInPickup () {
        mIsCallInRefuse = false;
        setupLocalVideo();
        joinChannel();
        mAgoraAPI.channelInviteAccept(channelName, mSubscriber, 0, null);
        mLayoutCallIn.setVisibility(View.GONE);
        mCallHangupBtn.setVisibility(View.VISIBLE);
        mCallTitle.setVisibility(View.GONE);

        CallSession.getInstance().getConnection().setActive();
    }

    private void callOutHangup() {
        if (mAgoraAPI != null)
            mAgoraAPI.channelInviteEnd(channelName, mSubscriber, 0);
    }

    private void callInRefuse() {
        // "status": 0 // Default
        // "status": 1 // Busy
        if (mAgoraAPI != null)
            mAgoraAPI.channelInviteRefuse(channelName, mSubscriber, 0, "{\"status\":0}");
        AgoraConnection ac = CallSession.getInstance().getConnection();
        ac.setDisconnected(new DisconnectCause(DisconnectCause.REJECTED));
        ac.destroy();

        onEncCallClicked();
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        Log.i(TAG, "onJoinChannelSuccess channel: " + channel + " uid: " + uid);
    }

    private void addSignalingCallback() {
        if (mAgoraAPI == null) {
            return;
        }

        mAgoraAPI.callbackSet(new AgoraAPI.CallBack() {

            @Override
            public void onLogout(final int i) {
                Log.i(TAG, "onLogout  i = " + i);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (i == IAgoraAPI.ECODE_LOGOUT_E_KICKED) { // other login the account
                            Toast.makeText(CallActivity.this, "Other login account ,you are logout.", Toast.LENGTH_SHORT).show();

                        } else if (i == IAgoraAPI.ECODE_LOGOUT_E_NET) { // net
                            Toast.makeText(CallActivity.this, "Logout for Network can not be.", Toast.LENGTH_SHORT).show();
                            finish();
                        }
                        Intent intent = new Intent();
                        intent.putExtra("result", "finish");
                        setResult(RESULT_OK, intent);
                        finish();
                    }
                });

            }

            /**
             * call in receiver
             */
            @Override
            public void onInviteReceived(final String channelID, final String account, final int uid, String s2) {
                Log.i(TAG, "onInviteReceived  channelID = " + channelID + "  account = " + account);

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
//                      "status": 0 // Default
//                      "status": 1 // Busy
                        mAgoraAPI.channelInviteRefuse(channelID, account, uid, "{\"status\":1}");

                    }
                });
            }

            /**
             * call out other ,local receiver
             */
            @Override
            public void onInviteReceivedByPeer(final String channelID, String account, int uid) {
                Log.i(TAG, "onInviteReceivedByPeer  channelID = " + channelID + "  account = " + account);

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mCallHangupBtn.setVisibility(View.VISIBLE);

                        mCallTitle.setText(String.format(Locale.US, "%s is being called ...", mSubscriber));
                    }
                });
            }

            /**
             * other receiver call accept callback
             * @param channelID
             * @param account
             * @param uid
             * @param s2
             */
            @Override
            public void onInviteAcceptedByPeer(String channelID, final String account, int uid, String s2) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mPlayer != null && mPlayer.isPlaying()) {
                            mPlayer.stop();
                        }
                        mCallTitle.setVisibility(View.GONE);
                        CallSession.getInstance().getConnection().setActive();
                    }
                });

            }

            /**
             * other receiver call refuse callback
             * @param channelID
             * @param account
             * @param uid
             * @param s2
             */

            @Override
            public void onInviteRefusedByPeer(String channelID, final String account, int uid, final String s2) {
                Log.i(TAG, "onInviteRefusedByPeer channelID = " + channelID + " account = " + account + " s2 = " + s2);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mPlayer != null && mPlayer.isPlaying()) {
                            mPlayer.stop();
                        }
                        if (s2.contains("status") && s2.contains("1")) {
                            Toast.makeText(CallActivity.this, account + " reject your call for busy", Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(CallActivity.this, account + " reject your call", Toast.LENGTH_SHORT).show();
                        }
                        AgoraConnection ac = CallSession.getInstance().getConnection();
                        ac.setDisconnected(new DisconnectCause(DisconnectCause.REJECTED));
                        ac.destroy();
                        onEncCallClicked();
                    }
                });
            }


            /**
             * end call remote receiver callback
             * @param channelID
             * @param account
             * @param uid
             * @param s2
             */
            @Override
            public void onInviteEndByPeer(final String channelID, String account, int uid, String s2) {
                Log.i(TAG, "onInviteEndByPeer channelID = " + channelID + " account = " + account);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (channelID.equals(channelName)) {
                            AgoraConnection ac = CallSession.getInstance().getConnection();
                            ac.setDisconnected(new DisconnectCause(DisconnectCause.REMOTE));
                            ac.destroy();
                            onEncCallClicked();
                        }

                    }
                });
            }

            /**
             * end call local receiver callback
             * @param channelID
             * @param account
             * @param uid
             */
            @Override
            public void onInviteEndByMyself(String channelID, String account, int uid) {
                Log.i(TAG, "onInviteEndByMyself channelID = " + channelID + "  account = " + account);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        AgoraConnection ac = CallSession.getInstance().getConnection();
                        ac.setDisconnected(new DisconnectCause(DisconnectCause.LOCAL));
                        ac.destroy();
                        onEncCallClicked();
                    }
                });
            }
        });
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
    protected void onResume() {
        super.onResume();
        addSignalingCallback();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        Log.i(TAG, "onDestroy");
        if (mPlayer != null && mPlayer.isPlaying()) {
            mPlayer.stop();
        }


        if (mRtcEngine != null) {
            mRtcEngine.stopPreview();
            mRtcEngine.leaveChannel();
        }
        mRtcEngine = null;

    }

    @Override
    public void onBackPressed() {
        Log.i(TAG, "onBackPressed callType: " + mCallType + " mIsCallInRefuse: " + mIsCallInRefuse);
        if (mCallType == Constant.CALL_IN && mIsCallInRefuse) {
            callInRefuse();
        } else {
            callOutHangup();
        }
        super.onBackPressed();
    }

    public void onSwitchCameraClicked(View view) {
        mRtcEngine.switchCamera();
    }


    public void onEncCallClicked() {
        finish();
    }

    private void initializeAgoraEngine() {
        mAgoraAPI = AGApplication.the().getmAgoraAPI();
        mRtcEngine = AGApplication.the().getmRtcEngine();
        Log.i(TAG, "initializeAgoraEngine mRtcEngine :" + mRtcEngine);
        if (mRtcEngine != null) {
            mRtcEngine.setLogFile("/sdcard/sdklog.txt");
        }
        setupVideoProfile();

    }

    private void setupVideoProfile() {
        mRtcEngine.enableVideo();
        mRtcEngine.enableAudio();
//      mRtcEngine.setVideoProfile(Constants.VIDEO_PROFILE_360P, false); // Earlier than 2.3.0

        mRtcEngine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(VideoEncoderConfiguration.VD_640x360, VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT));
    }

    private void setupLocalVideo() {
        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        mLayoutBigView.addView(surfaceView);
        mRtcEngine.setupLocalVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, 0));
        mLayoutBigView.setVisibility(View.VISIBLE);
        int ret = mRtcEngine.startPreview();
        Log.i(TAG, "setupLocalVideo startPreview enter << ret :" + ret);
    }

    private void joinChannel() {
        int ret = mRtcEngine.joinChannel(null, channelName, "Extra Optional Data", 0); // if you do not specify the uid, we will generate the uid for you
        Log.i(TAG, "joinChannel enter ret :" + ret);
    }

    private void setupRemoteVideo(int uid) {
        Log.i(TAG, "setupRemoteVideo uid: " + uid + " " + mLayoutBigView.getChildCount());
        if (mLayoutBigView.getChildCount() >= 1) {
            mLayoutBigView.removeAllViews();
        }

        SurfaceView surfaceViewSmall = RtcEngine.CreateRendererView(getBaseContext());
        surfaceViewSmall.setZOrderMediaOverlay(true);
        mLayoutSmallView.addView(surfaceViewSmall);
        mRtcEngine.setupLocalVideo(new VideoCanvas(surfaceViewSmall, VideoCanvas.RENDER_MODE_HIDDEN, 0));
        mLayoutSmallView.setVisibility(View.VISIBLE);

        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        mLayoutBigView.addView(surfaceView);
        mRtcEngine.setupRemoteVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
        mLayoutBigView.setVisibility(View.VISIBLE);
    }


    private void onRemoteUserLeft(int uid) {
        if (uid == mRemoteUid) {
            finish();
        }
    }

    private void onRemoteUserVideoMuted(int uid, boolean muted) {
        FrameLayout container = (FrameLayout) findViewById(R.id.remote_video_view_container);

        SurfaceView surfaceView = (SurfaceView) container.getChildAt(0);

        Object tag = surfaceView.getTag();
        if (tag != null && (Integer) tag == uid) {
            surfaceView.setVisibility(muted ? View.GONE : View.VISIBLE);
        }
    }

    private void initAgoraEngineAndJoinChannel() {
        initializeAgoraEngine();
        AGApplication.the().setOnAgoraEngineInterface(this);
        setupData();
    }

}
