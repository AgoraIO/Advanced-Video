package io.agora.openduo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Locale;

import io.agora.openduo.R;
import io.agora.openduo.model.AGEventHandler;
import io.agora.openduo.model.ConstantApp;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;

/**
 * "*" and "#" is useless
 */

public class DialerActivity extends BaseActivity implements AGEventHandler {
    private final static Logger log = LoggerFactory.getLogger(DialerActivity.class);

    private String mMyUid;
    private String mSubscriber;
    private String mChannel;

    private TextView mCallTitle;
    private EditText mSubscriberPhoneNumberText;
    private StringBuffer mCallNumberText = new StringBuffer("");

    private final int REQUEST_CODE = 0x01;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dialer);
    }

    @Override
    protected void initUIandEvent() {
        this.event().addEventHandler(this);

        Intent intent = getIntent();
        mMyUid = intent.getStringExtra(ConstantApp.ACTION_KEY_UID);

        mCallTitle = (TextView) findViewById(R.id.meet_title);
        mCallTitle.setText(String.format(Locale.US, "Your account ID: %s", mMyUid));
        mSubscriberPhoneNumberText = (EditText) findViewById(R.id.call_number_edit);
    }

    @Override
    protected void deInitUIandEvent() {
        this.event().removeEventHandler(this);
    }

    public void CallClickInit(View v) {
        switch (v.getId()) {
            case R.id.call_number_delete:
                if (mCallNumberText.length() > 0) {
                    mCallNumberText.delete(mCallNumberText.length() - 1, mCallNumberText.length());
                    mSubscriberPhoneNumberText.setText(mCallNumberText);
                    mSubscriberPhoneNumberText.setSelection(mSubscriberPhoneNumberText.getText().toString().length());
                }
                break;

            case R.id.call_number_call: // number layout call out button
                if (mSubscriberPhoneNumberText.getText().toString().equals(mMyUid)) {
                    showLongToast("Can not call yourself");
                } else {
                    mSubscriber = mSubscriberPhoneNumberText.getText().toString();
                    // call out

                    mChannel = mMyUid + mSubscriber;

                    worker().queryPeersOnlineStatus(mSubscriber);
                }

                log.debug("trying to call " + mSubscriber + " " + mChannel);
                break;
        }
    }


    /**
     * number click button
     *
     * @param v
     */
    public void numberClick(View v) {
        mCallNumberText.append(((Button) v).getText().toString());
        mSubscriberPhoneNumberText.setText(mCallNumberText);
        mSubscriberPhoneNumberText.setSelection(mSubscriberPhoneNumberText.getText().toString().length());
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        log.debug("onDestroy");
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        log.debug("onActivityResult requestCode: " + requestCode + " resultCode: " + resultCode);
        if (requestCode == REQUEST_CODE && resultCode == RESULT_OK) {
            if (data != null && data.getStringExtra("result").equals("finish")) {
                finish();
            }
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
        if (online) {
            worker().makeACall(mSubscriber, mChannel);
        } else {
            showLongToast(uid + " is not available now");
        }
    }

    @Override
    public void onInvitationReceivedByPeer(LocalInvitation invitation) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Intent intent = new Intent(DialerActivity.this, CallActivity.class);
                intent.putExtra(ConstantApp.ACTION_KEY_UID, mMyUid);
                intent.putExtra(ConstantApp.ACTION_KEY_SUBSCRIBER, mSubscriber);
                intent.putExtra(ConstantApp.ACTION_KEY_CHANNEL_NAME, mChannel);
                intent.putExtra(ConstantApp.ACTION_KEY_MakeOrReceive, ConstantApp.CALL_OUT);
                startActivityForResult(intent, REQUEST_CODE);
            }
        });
    }

    @Override
    public void onLocalInvitationAccepted(LocalInvitation invitation, String response) {

    }

    @Override
    public void onLocalInvitationRefused(LocalInvitation invitation, String response) {

    }

    @Override
    public void onLocalInvitationCanceled(LocalInvitation invitation) {

    }

    @Override
    public void onInvitationReceived(final RemoteInvitation invitation) {
        if (config().mRemoteInvitation != null) { // During a call
            return;
        }

        config().mRemoteInvitation = invitation;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Intent intent = new Intent(DialerActivity.this, CallActivity.class);

                intent.putExtra(ConstantApp.ACTION_KEY_UID, mMyUid);
                intent.putExtra(ConstantApp.ACTION_KEY_SUBSCRIBER, invitation.getCallerId());
                intent.putExtra(ConstantApp.ACTION_KEY_CHANNEL_NAME, invitation.getContent()); // TODO getChannelId() will be available in RTM 1.0
                intent.putExtra(ConstantApp.ACTION_KEY_MakeOrReceive, ConstantApp.CALL_IN);

                startActivityForResult(intent, REQUEST_CODE);
            }
        });

        log.debug("onInvitationReceived " + invitation + " " + invitation.getChannelId() + " " + invitation.getContent() + " " + invitation.getResponse());
    }

    @Override
    public void onInvitationRefused(RemoteInvitation invitation) {

    }

    @Override
    public void onRemoteInvitationCanceled(RemoteInvitation invitation) {

    }

    @Override
    public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {

    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {

    }

    @Override
    public void onUserOffline(int uid, int reason) {

    }

    @Override
    public void onExtraCallback(int type, Object... data) {
        needReLogin(type, data);
    }

    @Override
    public void onLastmileQuality(int quality) {

    }

    @Override
    public void onLastmileProbeResult(IRtcEngineEventHandler.LastmileProbeResult result) {

    }
}
