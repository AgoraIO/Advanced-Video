package io.agora.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Locale;

import io.agora.openduo.AGApplication;
import io.agora.openduo.R;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.ResultCallback;
import io.agora.rtm.RtmCallEventListener;
import io.agora.rtm.RtmClient;
import io.agora.rtm.RtmMessage;
import io.agora.utils.Constant;

/**
 * "*" and "#" is useless
 */

public class NumberCallActivity extends AppCompatActivity {
    private final String TAG = NumberCallActivity.class.getSimpleName();

    private String myUID;
    private String mSubscriber;
    private TextView mCallTitle;
    private EditText mSubscriberPhoneNumberText;
    private StringBuffer mCallNumberText = new StringBuffer();

    private String channelName = "channelid";

    private RtmClient rtmClient;
    private final int REQUEST_CODE = 0x01;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_number);

        initRTMClient();

        initUI();
    }

    private void initUI() {
        Intent intent = getIntent();
        myUID = intent.getStringExtra("uid");
        mCallTitle = findViewById(R.id.meet_title);
        mCallTitle.setText(String.format(Locale.US, "Your account ID is %s", myUID));
        mSubscriberPhoneNumberText = findViewById(R.id.call_number_edit);
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

                if (!Constant.isFastlyClick()) {
                    if (mSubscriberPhoneNumberText.getText().toString().equals(myUID)) {
                        Toast.makeText(this, "could not call yourself", Toast.LENGTH_SHORT).show();
                    } else {
                        mSubscriber = mSubscriberPhoneNumberText.getText().toString();
                        // call out
                        //Since we don't have queryUserStatus in RTM yet, we can directly send
                        // the call request/invite to the other User
                        // and check if he is online using callback.
                        if (mSubscriberPhoneNumberText.getText().toString().isEmpty()) return;
                        checkIfSubscriberIsOnline(mSubscriberPhoneNumberText.getText().toString());
                    }
                } else {
                    Toast.makeText(this, "fast click", Toast.LENGTH_SHORT).show();
                }

                Log.i(TAG, "call number call init");

                break;
        }
    }

    private void sendCallInvite(final String subscriberPhoneNumber) {
        LocalInvitation localInvitation = rtmClient.getRtmCallManager().createLocalInvitation(subscriberPhoneNumber);
        channelName = myUID + mSubscriber;
        localInvitation.setContent(channelName);//we can pass extra info if required here, must not exceed 8kb if encoded in UTF-8.
        //Note: do not use localInvitation.setChannelId, its a reserved interface to interact with the Signaling SDK
        AGApplication.the().holdLocalInvitation(localInvitation);
        rtmClient.getRtmCallManager().sendLocalInvitation(localInvitation, new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {
                Log.i(TAG, "sendLocalInvitation method call succeeds subscriberPhoneNumber = " + subscriberPhoneNumber);
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                Log.i(TAG, "sendLocalInvitation error_code = " + errorInfo.getErrorCode()
                        + " and error_description " + errorInfo.getErrorDescription());
            }
        });
    }


    //Temporary replacement for queryUserStatus, the below method can be replaced by queryUserStatus after release
    //sendMessageToPeer/callee then cache PEER_MESSAGE_ERR_PEER_UNREACHABLE and consider the callee is offline for now.
    private void checkIfSubscriberIsOnline(final String subscriberPhoneNumber) {
        RtmMessage message = rtmClient.createMessage();
        message.setText("test-message");
        rtmClient.sendMessageToPeer(subscriberPhoneNumber, message, new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {
                Log.i(TAG, "sendMessageToPeer method call succeeds subscriberID = " + subscriberPhoneNumber);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        sendCallInvite(mSubscriberPhoneNumberText.getText().toString());
                    }
                });
            }

            @Override
            public void onFailure(final ErrorInfo errorInfo) {
                Log.i(TAG, "sendMessageToPeer error_code = " + errorInfo.getErrorCode()
                        + " and error_description " + errorInfo.getErrorDescription());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        final int errorCode = errorInfo.getErrorCode();
                        //1-PEER_MESSAGE_ERR_FAILURE, 2-PEER_MESSAGE_ERR_TIMEOUT, 3-PEER_MESSAGE_ERR_PEER_UNREACHABLE
                        if (errorCode == 1 || errorCode == 2 || errorCode == 3)
                            Toast.makeText(NumberCallActivity.this, subscriberPhoneNumber + " is offline ", Toast.LENGTH_SHORT).show();
                    }
                });
            }
        });
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

    private void addCallback() {

        if (rtmClient == null) {
            return;
        }
        Log.d(TAG, "addCallback RTM callbacks for NumberCallActivity");
        rtmClient.getRtmCallManager().setEventListener(new RtmCallEventListener() {
            @Override
            public void onLocalInvitationReceivedByPeer(LocalInvitation localInvitation) {
                Log.i(TAG, "onLocalInvitationReceivedByPeer myUID = " + myUID +
                        " CalleeId = " + localInvitation.getCalleeId() + " ChannelId = " + localInvitation.getContent());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Intent intent = new Intent(NumberCallActivity.this, CallActivity.class);
                        intent.putExtra("uid", myUID);
                        intent.putExtra("channelName", channelName);
                        intent.putExtra("subscriber", mSubscriber);
                        intent.putExtra("type", Constant.CALL_OUT);
                        startActivityForResult(intent, REQUEST_CODE);
                    }
                });
            }

            @Override
            public void onLocalInvitationAccepted(LocalInvitation localInvitation, String s) {

            }

            @Override
            public void onLocalInvitationRefused(LocalInvitation localInvitation, String s) {
                Log.i(TAG, "onLocalInvitationRefused " +
                        "myUID = " + myUID +
                        " CalleeId = " + localInvitation.getCalleeId() +
                        " ChannelId = " + localInvitation.getContent());
            }

            @Override
            public void onLocalInvitationCanceled(LocalInvitation localInvitation) {
                Log.i(TAG, "onLocalInvitationCanceled " +
                        "myUID = " + myUID +
                        " CalleeId = " + localInvitation.getCalleeId() +
                        " ChannelId = " + localInvitation.getContent());
            }

            @Override
            public void onLocalInvitationFailure(LocalInvitation localInvitation, int i) {
                Log.i(TAG, "onLocalInvitationFailure " +
                        "myUID = " + myUID +
                        " CalleeId = " + localInvitation.getCalleeId() +
                        " ChannelId = " + localInvitation.getContent() +
                        " Reason = " + i);
            }

            @Override
            public void onRemoteInvitationReceived(final RemoteInvitation remoteInvitation) {
                Log.i(TAG, "onRemoteInvitationReceived  ChannelId = " + remoteInvitation.getContent() + " CallerId = " + remoteInvitation.getCallerId());
                AGApplication.the().holdRemoteInvitation(remoteInvitation);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Intent intent = new Intent(NumberCallActivity.this, CallActivity.class);
                        intent.putExtra("uid", myUID);
                        intent.putExtra("channelName", remoteInvitation.getContent());
                        intent.putExtra("subscriber", remoteInvitation.getCallerId());
                        intent.putExtra("type", Constant.CALL_IN);
                        startActivityForResult(intent, REQUEST_CODE);
                    }
                });
            }

            @Override
            public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {

            }

            @Override
            public void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) {

            }

            @Override
            public void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation) {

            }

            @Override
            public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int i) {

            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        addCallback();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "onDestroy");
        rtmClient.logout(null);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Log.i(TAG, "onActivityResult requestCode: " + requestCode + " resultCode: " + resultCode);
        if (requestCode == REQUEST_CODE && resultCode == RESULT_OK) {
            if (data != null && data.getStringExtra("result").equals("finish")) {
                finish();
            }
        }
    }

    private void initRTMClient() {
        rtmClient = AGApplication.the().getRTMClient();
    }

}
