package io.agora.openduo.ui;

import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.SurfaceView;
import android.view.View;
import android.widget.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Locale;

import io.agora.openduo.R;
import io.agora.openduo.model.AGEventHandler;
import io.agora.openduo.model.ConstantApp;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;

public class CallActivity extends BaseActivity implements AGEventHandler {
    private final static Logger log = LoggerFactory.getLogger(DialerActivity.class);

    private String mMyUid;
    private String mSubscriber;
    private String mChannel;

    private CheckBox mCheckMute;
    private TextView mCallTitle;
    private ImageView mCallHangupBtn;
    private RelativeLayout mLayoutCallIn;

    private FrameLayout mLayoutBigView;
    private FrameLayout mLayoutSmallView;

    private MediaPlayer mPlayer;
    private int callType = ConstantApp.UNKNOWN;
    private boolean mIsCallInRefuse = false;
    private int mRemoteUid = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);
    }

    @Override
    protected void initUIandEvent() {
        this.event().addEventHandler(this);

        mCallTitle = (TextView) findViewById(R.id.meet_title);

        mCheckMute = (CheckBox) findViewById(R.id.call_mute_button);
        mCheckMute.setOnCheckedChangeListener(oncheckChangeListerener);

        mCallHangupBtn = (ImageView) findViewById(R.id.call_button_hangup);
        mLayoutCallIn = (RelativeLayout) findViewById(R.id.call_layout_callin);

        mLayoutBigView = (FrameLayout) findViewById(R.id.remote_video_view_container);
        mLayoutSmallView = (FrameLayout) findViewById(R.id.local_video_view_container);

        setupForCall();
    }

    @Override
    protected void deInitUIandEvent() {
        worker().leaveChannel(mChannel);
        this.event().removeEventHandler(this);
    }

    private void setupForCall() {
        Intent intent = getIntent();
        mMyUid = intent.getStringExtra(ConstantApp.ACTION_KEY_UID);
        mSubscriber = intent.getStringExtra(ConstantApp.ACTION_KEY_SUBSCRIBER);
        mChannel = intent.getStringExtra(ConstantApp.ACTION_KEY_CHANNEL_NAME);
        callType = intent.getIntExtra(ConstantApp.ACTION_KEY_MakeOrReceive, ConstantApp.UNKNOWN);

        worker().configEngine(VideoEncoderConfiguration.VD_640x360, null, null);

        setupLocalVideo();

        if (callType == ConstantApp.CALL_IN) {
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
        } else if (callType == ConstantApp.CALL_OUT) {
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

            worker().joinChannel(mChannel, Integer.valueOf(mMyUid));
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        log.debug("onNewIntent " + intent);

        setupForCall();
    }

    private CompoundButton.OnCheckedChangeListener oncheckChangeListerener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            rtcEngine().muteLocalAudioStream(isChecked);
        }
    };

    public void CallClickInit(View v) {
        switch (v.getId()) {
            case R.id.call_in_hangup:
                callInRefuse();
                break;
            case R.id.call_in_pickup:
                mIsCallInRefuse = false;

                answerTheCall();

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

    private void answerTheCall() {
        worker().joinChannel(mChannel, Integer.valueOf(mMyUid));
        worker().answerTheCall(config().mRemoteInvitation);
    }

    private void callOutHangup() {
        onEncCallClicked();

        worker().hangupTheCall(null); // local|out
    }

    private void callInRefuse() {
        onEncCallClicked();

        // "status": 0 // Default
        // "status": 1 // Busy
        config().mRemoteInvitation.setResponse("{\"status\":0}");

        worker().hangupTheCall(config().mRemoteInvitation);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        log.debug("onDestroy");
        if (mPlayer != null && mPlayer.isPlaying()) {
            mPlayer.stop();
        }
    }

    @Override
    public void onBackPressed() {
        log.debug("onBackPressed callType: " + callType + " mIsCallInRefuse: " + mIsCallInRefuse);

        if (callType == ConstantApp.CALL_IN && mIsCallInRefuse) {
            callInRefuse();
        } else {
            callOutHangup();
        }
        super.onBackPressed();
    }

    public void onSwitchCameraClicked(View view) {
        rtcEngine().switchCamera();
    }

    public void onEncCallClicked() {
        finish();
    }

    private void setupLocalVideo() {
        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        mLayoutBigView.addView(surfaceView);

        worker().preview(true, surfaceView, 0);

        mLayoutBigView.setVisibility(View.VISIBLE);
    }

    private void setupRemoteVideo(int uid) {
        log.debug("setupRemoteVideo uid: " + (uid & 0xFFFFFFFFL) + " " + mLayoutBigView.getChildCount());
        View view = mLayoutBigView.getChildAt(0);
        mLayoutBigView.removeAllViews();
        ((SurfaceView) view).setZOrderMediaOverlay(true);
        ((SurfaceView) view).setZOrderOnTop(true);
        mLayoutSmallView.addView(view);
        mLayoutSmallView.setVisibility(View.VISIBLE);

        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        mLayoutBigView.addView(surfaceView);

        rtcEngine().setupRemoteVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
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

    @Override
    public void onLoginSuccess(String uid) {

    }

    @Override
    public void onLoginFailed(String uid, ErrorInfo error) {

    }

    @Override
    public void onPeerOnlineStatusQueried(String uid, boolean online) {

    }

    @Override
    public void onInvitationReceivedByPeer(LocalInvitation invitation) {
        log.debug("onInvitationReceivedByPeer " + invitation);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mCallHangupBtn.setVisibility(View.VISIBLE);

                mCallTitle.setText(String.format(Locale.US, "%s is being called ...", mSubscriber));
            }
        });
    }

    @Override
    public void onLocalInvitationAccepted(LocalInvitation invitation, String response) {
        log.debug("onLocalInvitationAccepted " + invitation + " " + invitation.getResponse() + " " + response);

        if (mPlayer != null && mPlayer.isPlaying()) {
            mPlayer.stop();
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mCallTitle.setVisibility(View.GONE);
            }
        });
    }

    @Override
    public void onLocalInvitationRefused(LocalInvitation invitation, final String response) {
        log.debug("onLocalInvitationRefused " + invitation + " " + invitation.getResponse() + " " + response);

        final String callee = invitation.getCalleeId();

        if (mPlayer != null && mPlayer.isPlaying()) {
            mPlayer.stop();
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (response.contains("status") && response.contains("1")) {
                    showLongToast("line busy for " + callee);
                } else {
                    showLongToast(callee + " reject your call");
                }

                onEncCallClicked();
            }
        });
    }

    @Override
    public void onLocalInvitationCanceled(LocalInvitation invitation) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                onEncCallClicked();
            }
        });
    }

    @Override
    public void onInvitationReceived(RemoteInvitation invitation) {
        invitation.setResponse("{\"status\":1}"); // Busy, already in call

        worker().hangupTheCall(invitation);
    }

    @Override
    public void onInvitationRefused(RemoteInvitation invitation) {
        String channel = config().mChannel;

        log.debug("onInvitationRefused " + invitation + " " + invitation.getResponse() + " " + channel);

        if (channel == null) {
            return;
        }

        // TODO RTM 1.0 will support getChannelId()
        if (TextUtils.equals(channel, invitation.getChannelId()) || TextUtils.equals(channel, invitation.getContent())) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    onEncCallClicked();
                }
            });
        }
    }

    @Override
    public void onRemoteInvitationCanceled(RemoteInvitation invitation) {
        String channel = config().mChannel;
        log.debug("onInvitationRefused " + invitation + " " + invitation.getResponse() + " " + channel);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                showLongToast("Call canceled");
                onEncCallClicked();
            }
        });
    }


    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
        log.debug("onFirstRemoteVideoDecoded " + (uid & 0xFFFFFFFFL) + " " + height + " " + width);

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
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {

    }

    @Override
    public void onUserOffline(int uid, int reason) {
        onRemoteUserLeft(uid);
    }

    @Override
    public void onExtraCallback(int type, Object... data) {
        needReLogin(type, data);

        if (EVENT_TYPE_ON_USER_VIDEO_MUTED == type) {
            onRemoteUserVideoMuted((int) data[0], (boolean) data[1]);
        }
    }

    @Override
    public void onLastmileQuality(int quality) {

    }

    @Override
    public void onLastmileProbeResult(IRtcEngineEventHandler.LastmileProbeResult result) {

    }
}
