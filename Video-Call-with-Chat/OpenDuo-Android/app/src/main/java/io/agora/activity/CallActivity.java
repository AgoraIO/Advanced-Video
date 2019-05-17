package io.agora.activity;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.*;

import java.util.Locale;

import io.agora.openduo.AGApplication;
import io.agora.openduo.R;
import io.agora.rtc.Constants;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.ResultCallback;
import io.agora.rtm.RtmCallEventListener;
import io.agora.rtm.RtmClient;
import io.agora.utils.Constant;

/**
 * Created by beryl on 2017/11/6.
 */

public class CallActivity extends AppCompatActivity implements AGApplication.OnAgoraEngineInterface {
    private final String TAG = CallActivity.class.getSimpleName();

    private static final int PERMISSION_REQ_ID_RECORD_AUDIO = 22;
    private static final int PERMISSION_REQ_ID_CAMERA = PERMISSION_REQ_ID_RECORD_AUDIO + 1;
    private static final int PERMISSION_REQ_ID_STORAGE = PERMISSION_REQ_ID_CAMERA + 1;

    private RtmClient rtmClient;
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
    private int callType = -1;
    private boolean mIsCallInRefuse = false;
    private int mRemoteUid = 0;
    private RemoteInvitation remoteInvitation;
    private LocalInvitation localInvitation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);

        InitUI();

        if (checkSelfPermission(Manifest.permission.RECORD_AUDIO, PERMISSION_REQ_ID_RECORD_AUDIO)
                && checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA)
                && checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_STORAGE)) {
            initAgoraEngineAndJoinChannel();
        }
    }

    private void InitUI() {
        mCallTitle = findViewById(R.id.meet_title);

        mCheckMute = findViewById(R.id.call_mute_button);
        mCheckMute.setOnCheckedChangeListener(oncheckChangeListerener);

        mCallHangupBtn = findViewById(R.id.call_button_hangup);
        mLayoutCallIn = findViewById(R.id.call_layout_callin);

        mLayoutBigView = findViewById(R.id.remote_video_view_container);
        mLayoutSmallView = findViewById(R.id.local_video_view_container);
    }

    private void setupData() {
        Intent intent = getIntent();
        mSubscriber = intent.getStringExtra("subscriber");
        channelName = intent.getStringExtra("channelName");

        //Get Invitation objects form AGApplication
        remoteInvitation = AGApplication.the().getRemoteInvitation();
        localInvitation = AGApplication.the().getLocalInvitation();

        callType = intent.getIntExtra("type", -1);
        if (callType == Constant.CALL_IN) {
            mIsCallInRefuse = true;
            mLayoutCallIn.setVisibility(View.VISIBLE);
            mCallHangupBtn.setVisibility(View.GONE);
            mCallTitle.setText(String.format(Locale.US, "%s is calling...", mSubscriber));

            try {
                mPlayer = MediaPlayer.create(this, R.raw.basic_ring);
                mPlayer.setLooping(true);
                mPlayer.start();
            } catch (Exception e) {
                e.printStackTrace();
            }
            setupLocalVideo(); // Tutorial Step 3
        } else if (callType == Constant.CALL_OUT) {
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

            setupLocalVideo(); // Tutorial Step 3
            joinChannel(); // Tutorial Step 4
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
    public void onUserOffline(final int uid, int reason) { // Tutorial Step 7
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                onRemoteUserLeft(uid);
            }
        });
    }

    @Override
    public void onUserMuteVideo(final int uid, final boolean muted) { // Tutorial Step 10
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

            case R.id.call_in_hangup:
                callInRefuse();
                break;

            case R.id.call_in_pickup:
                mIsCallInRefuse = false;
                joinChannel(); // Tutorial Step 4

                rtmClient.getRtmCallManager().acceptRemoteInvitation(remoteInvitation, new ResultCallback<Void>() {
                    @Override
                    public void onSuccess(Void aVoid) {
                        Log.i(TAG, "acceptRemoteInvitation method call succeeds");
                    }

                    @Override
                    public void onFailure(ErrorInfo errorInfo) {
                        Log.i(TAG, "acceptRemoteInvitation error_code = " + errorInfo.getErrorCode()
                                + "and error_description" + errorInfo.getErrorDescription());
                    }
                });
                mLayoutCallIn.setVisibility(View.GONE);
                mCallHangupBtn.setVisibility(View.VISIBLE);
                mCallTitle.setVisibility(View.GONE);
                if (mPlayer != null && mPlayer.isPlaying()) {
                    mPlayer.stop();
                }
                break;

            case R.id.call_button_hangup: // call out canceled or call ended
                callOutHangup();
                break;
        }
    }

    private void callOutHangup() {
        Log.i(TAG, "callOutHangup localInvitation = " + localInvitation);
        if (rtmClient != null) {
            rtmClient.getRtmCallManager().cancelLocalInvitation(localInvitation, new ResultCallback<Void>() {
                @Override
                public void onSuccess(Void aVoid) {
                    Log.i(TAG, "cancelLocalInvitation method call succeeds");
                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {
                    Log.i(TAG, "cancelLocalInvitation error_code = " + errorInfo.getErrorCode()
                            + "and error_description" + errorInfo.getErrorDescription());
                }
            });
        }
        onEncCallClicked();
    }

    private void callInRefuse() {
        if (rtmClient != null)
            rtmClient.getRtmCallManager().refuseRemoteInvitation(remoteInvitation, new ResultCallback<Void>() {
                @Override
                public void onSuccess(Void aVoid) {
                    Log.i(TAG, "refuseRemoteInvitation method call succeeds");
                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {
                    Log.i(TAG, "refuseRemoteInvitation error_code = " + errorInfo.getErrorCode()
                            + "and error_description" + errorInfo.getErrorDescription());
                }
            });
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        Log.i(TAG, "onJoinChannelSuccess channel: " + channel + " uid: " + uid);
    }


    private void addSignalingCallback() {
        if (rtmClient == null) {
            return;
        }

        rtmClient.getRtmCallManager().setEventListener(new RtmCallEventListener() {
            @Override
            public void onLocalInvitationReceivedByPeer(LocalInvitation localInvitation) {
                Log.i(TAG, "onLocalInvitationReceivedByPeer channelID = " + localInvitation.getContent() + "  CalleeId = " + localInvitation.getCalleeId());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mCallHangupBtn.setVisibility(View.VISIBLE);

                        mCallTitle.setText(String.format(Locale.US, "%s is being called ...", mSubscriber));
                    }
                });
            }

            @Override
            public void onLocalInvitationAccepted(LocalInvitation localInvitation, String s) {
                Log.i(TAG, "onLocalInvitationAccepted channelID = " + localInvitation.getContent() + "  CalleeId = " + localInvitation.getCalleeId());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mPlayer != null && mPlayer.isPlaying()) {
                            mPlayer.stop();
                        }
                        mCallTitle.setVisibility(View.GONE);
                    }
                });
            }

            @Override
            public void onLocalInvitationRefused(LocalInvitation localInvitation, String s) {
                Log.i(TAG, "onLocalInvitationRefused channelID = " + localInvitation.getContent() + "  CalleeId = " + localInvitation.getCalleeId());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mPlayer != null && mPlayer.isPlaying()) {
                            mPlayer.stop();
                        }
                        if (remoteInvitation.getResponse().contains("status") && remoteInvitation.getResponse().contains("1")) {
                            Toast.makeText(CallActivity.this, remoteInvitation.getCallerId() + " reject your call for busy", Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(CallActivity.this, remoteInvitation.getCallerId() + " reject your call", Toast.LENGTH_SHORT).show();
                        }

                        onEncCallClicked();
                    }
                });
            }

            @Override
            public void onLocalInvitationCanceled(LocalInvitation localInvitation) {
                Log.i(TAG, "onLocalInvitationCanceled channelID = " + localInvitation.getContent() + "  CalleeId = " + localInvitation.getCalleeId());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        onEncCallClicked();
                    }
                });
            }

            @Override
            public void onLocalInvitationFailure(LocalInvitation localInvitation, int i) {
                Log.i(TAG, "onLocalInvitationFailure channelID = " + localInvitation.getContent() + "  CalleeId = " + localInvitation.getCalleeId());
            }

            @Override
            public void onRemoteInvitationReceived(final RemoteInvitation remoteInvitation) {
                Log.i(TAG, "onRemoteInvitationReceived  channelID = " + remoteInvitation.getContent() + "  CallerId = " + remoteInvitation.getCallerId());
                // "status": 0 // Default
                // "status": 1 // Busy
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        remoteInvitation.setResponse("{\"status\":1}");
                        rtmClient.getRtmCallManager().refuseRemoteInvitation(remoteInvitation, null);
                    }
                });
            }

            @Override
            public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {
                Log.i(TAG, "onRemoteInvitationAccepted  channelID = " + remoteInvitation.getContent() + "  CallerId = " + remoteInvitation.getCallerId());
            }

            @Override
            public void onRemoteInvitationRefused(final RemoteInvitation remoteInvitation) {
                Log.i(TAG, "onRemoteInvitationRefused channelID = " + remoteInvitation.getContent() + " CallerId = " + remoteInvitation.getCallerId() + " extra = " + remoteInvitation.getResponse());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (remoteInvitation.getContent().equals(channelName)) {
                            onEncCallClicked();
                        }
                    }
                });
            }

            @Override
            public void onRemoteInvitationCanceled(final RemoteInvitation remoteInvitation) {
                Log.i(TAG, "onRemoteInvitationCanceled channelID = " + remoteInvitation.getContent() + " CallerId = " + remoteInvitation.getCallerId());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (remoteInvitation.getContent().equals(channelName)) {
                            onEncCallClicked();
                        }
                    }
                });
            }

            @Override
            public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int i) {
                Log.i(TAG, "onRemoteInvitationFailure code = " + i);
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
        Log.i(TAG, "onBackPressed callType: " + callType + " mIsCallInRefuse: " + mIsCallInRefuse);
        if (callType == Constant.CALL_IN && mIsCallInRefuse) {
            callInRefuse();
        } else {
            callOutHangup();
        }
        super.onBackPressed();
    }

    // Tutorial Step 8
    public void onSwitchCameraClicked(View view) {
        mRtcEngine.switchCamera();
    }

    // Tutorial Step 6
    public void onEncCallClicked() {
        finish();
    }

    // Tutorial Step 1
    private void initializeAgoraEngine() {
        rtmClient = AGApplication.the().getRTMClient();
        mRtcEngine = AGApplication.the().getmRtcEngine();
        Log.i(TAG, "initializeAgoraEngine mRtcEngine :" + mRtcEngine);
        if (mRtcEngine != null) {
            mRtcEngine.setLogFile("/sdcard/sdklog.txt");
        }
        setupVideoProfile();

    }

    // Tutorial Step 2
    private void setupVideoProfile() {
        mRtcEngine.enableVideo();
        mRtcEngine.setVideoProfile(Constants.VIDEO_PROFILE_360P, false);
    }

    // Tutorial Step 3
    private void setupLocalVideo() {
        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        mLayoutBigView.addView(surfaceView);
        mRtcEngine.setupLocalVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, 0));
        mLayoutBigView.setVisibility(View.VISIBLE);
        int ret = mRtcEngine.startPreview();
        Log.i(TAG, "setupLocalVideo startPreview enter << ret :" + ret);
    }

    // Tutorial Step 4
    private void joinChannel() {
        int ret = mRtcEngine.joinChannel(null, channelName, "Extra Optional Data", 0); // if you do not specify the uid, we will generate the uid for you
        Log.i(TAG, "joinChannel enter ret :" + ret);
    }

    // Tutorial Step 5
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


    // Tutorial Step 7
    private void onRemoteUserLeft(int uid) {
        if (uid == mRemoteUid) {
            finish();
        }
    }

    // Tutorial Step 10
    private void onRemoteUserVideoMuted(int uid, boolean muted) {
        FrameLayout container = findViewById(R.id.remote_video_view_container);

        SurfaceView surfaceView = (SurfaceView) container.getChildAt(0);

        Object tag = surfaceView.getTag();
        if (tag != null && (Integer) tag == uid) {
            surfaceView.setVisibility(muted ? View.GONE : View.VISIBLE);
        }
    }


    public boolean checkSelfPermission(String permission, int requestCode) {
        Log.i(TAG, "checkSelfPermission " + permission + " " + requestCode);
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
        Log.i(TAG, "onRequestPermissionsResult " + grantResults[0] + " " + requestCode);

        switch (requestCode) {
            case PERMISSION_REQ_ID_RECORD_AUDIO: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA);
                } else {
                    showLongToast("No permission for " + Manifest.permission.RECORD_AUDIO);
                    onEncCallClicked();
                }
                break;
            }
            case PERMISSION_REQ_ID_CAMERA: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_STORAGE);
                } else {
                    showLongToast("No permission for " + Manifest.permission.CAMERA);
                    onEncCallClicked();
                }
                break;
            }
            case PERMISSION_REQ_ID_STORAGE: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    initAgoraEngineAndJoinChannel();
                } else {
                    showLongToast("No permission for " + Manifest.permission.WRITE_EXTERNAL_STORAGE);
                    onEncCallClicked();
                }
                break;
            }
        }
    }

    private void initAgoraEngineAndJoinChannel() {
        initializeAgoraEngine();
        AGApplication.the().setOnAgoraEngineInterface(this);
        setupData();
    }

}
