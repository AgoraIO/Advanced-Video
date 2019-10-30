package io.agora.openduo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.agora.openduo.R;
import io.agora.openduo.model.AGEventHandler;
import io.agora.openduo.model.ConstantApp;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.RtmStatusCode;

public class MainActivity extends BaseActivity implements AGEventHandler {
    private final static Logger log = LoggerFactory.getLogger(MainActivity.class);

    private EditText textAccountName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
    }

    @Override
    protected void initUIandEvent() {
        textAccountName = (EditText) findViewById(R.id.account_name);
        textAccountName.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                boolean isEmpty = s.toString().isEmpty();
                findViewById(R.id.button_login).setEnabled(!isEmpty);
            }
        });
    }

    @Override
    protected void deInitUIandEvent() {
        event().removeEventHandler(this);
    }

    public void onClickLogin(View v) {
        event().addEventHandler(this);

        String uid = textAccountName.getText().toString().trim();
        worker().connectToRtmService(uid);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        log.debug("onDestroy");
    }

    @Override
    public void onLoginSuccess(final String uid) {
        log.debug("onLoginSuccess " + uid);
        forwardToDialerActivity(uid);
    }

    private void forwardToDialerActivity(final String uid) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Intent intent = new Intent(MainActivity.this, DialerActivity.class);
                intent.putExtra(ConstantApp.ACTION_KEY_UID, uid);
                startActivity(intent);
            }
        });
    }

    @Override
    public void onLoginFailed(String uid, ErrorInfo error) {
        final String errMsg = "RTM Login failed " + uid + " " + error.getErrorCode() + " " + error.getErrorDescription();

        if (error.getErrorCode() == RtmStatusCode.LoginError.LOGIN_ERR_ALREADY_LOGIN) {
            log.debug("Already login " + uid);
            forwardToDialerActivity(uid);
            return;
        }

        log.error(errMsg);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                showLongToast(errMsg);
            }
        });
    }

    @Override
    public void onPeerOnlineStatusQueried(String uid, boolean online) {

    }

    @Override
    public void onInvitationReceivedByPeer(LocalInvitation invitation) {

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
    public void onInvitationReceived(RemoteInvitation invitation) {

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

    }

    @Override
    public void onLastmileQuality(int quality) {

    }

    @Override
    public void onLastmileProbeResult(IRtcEngineEventHandler.LastmileProbeResult result) {

    }
}
