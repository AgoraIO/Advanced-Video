package io.agora.cs;

import android.net.Uri;
import android.os.Bundle;
import android.telecom.Connection;
import android.telecom.ConnectionRequest;
import android.telecom.ConnectionService;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.util.Log;

/**
 * Created by minming on 2019/04/08.
 */
public class AgoraConnectionService extends ConnectionService {
    private static final String TAG = AgoraConnectionService.class.getSimpleName();
    private static boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    @Override
    public Connection onCreateIncomingConnection(PhoneAccountHandle connectionManagerPhoneAccount, ConnectionRequest request) {
        Log.i(TAG, "onCreateIncomingConnection: called.");

        Bundle extras = request.getExtras();
        String account = extras.getString(Constant.ACCOUNT_NAME);
        String channel = extras.getString(Constant.CHANNEL_NAME);
        String subscriber = extras.getString(Constant.SUBSCRIBER_NAME);
        int callType = extras.getInt(Constant.CALL_TYPE);
        int videoState = extras.getInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE);

        AgoraConnection ac = new AgoraConnection (account, channel, subscriber, callType);
        ac.setVideoState(videoState);
        ac.setAddress(Uri.fromParts(PhoneAccount.SCHEME_SIP, subscriber, null), TelecomManager.PRESENTATION_ALLOWED);
        ac.setCallerDisplayName(subscriber, TelecomManager.PRESENTATION_ALLOWED);
        CallSession.getInstance().setConnection(ac);

        return ac;
    }

    @Override
    public Connection onCreateOutgoingConnection(PhoneAccountHandle connectionManagerPhoneAccount, ConnectionRequest request) {
        Log.i(TAG, "onCreateOutgoingConnection: called.");

        Bundle extras = request.getExtras();
        String account = extras.getString(Constant.ACCOUNT_NAME);
        String channel = extras.getString(Constant.CHANNEL_NAME);
        String subscriber = extras.getString(Constant.SUBSCRIBER_NAME);
        int callType = extras.getInt(Constant.CALL_TYPE);
        int videoState = extras.getInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE);

        AgoraConnection ac = new AgoraConnection (account, channel, subscriber, callType);
        ac.setVideoState(videoState);
        ac.setAddress(Uri.fromParts(PhoneAccount.SCHEME_SIP, subscriber, null), TelecomManager.PRESENTATION_ALLOWED);
        ac.setCallerDisplayName(subscriber, TelecomManager.PRESENTATION_ALLOWED);
        CallSession.getInstance().setConnection(ac);

        return ac;
    }

    @Override
    public void onCreateIncomingConnectionFailed(
            PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        Log.e(TAG,"onCreateIncomingConnectionFailed");
    }

    @Override
    public void onCreateOutgoingConnectionFailed(
            PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        Log.e(TAG,"onCreateOutcomingConnectionFailed");
    }
}
