package io.agora.calling.with.connection.service.cs;

import android.net.Uri;
import android.os.Bundle;
import android.telecom.Connection;
import android.telecom.ConnectionRequest;
import android.telecom.ConnectionService;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.agora.calling.with.connection.service.model.ConstantApp;

/**
 * Created by minming on 2019/04/08.
 */
public class AgoraConnectionService extends ConnectionService {
    private final static Logger log = LoggerFactory.getLogger(AgoraConnectionService.class);
    private static boolean DEBUG = true; // Log.isLoggable(TAG, Log.DEBUG)

    public static final String SCHEME_AG = "customized_call";

    @Override
    public Connection onCreateIncomingConnection(PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        log.info("onCreateIncomingConnection: called. " + phoneAccount + " " + request);

        Bundle extras = request.getExtras();
        String myUid = extras.getString(ConstantApp.ACTION_KEY_UID);
        String channel = extras.getString(ConstantApp.ACTION_KEY_CHANNEL_NAME);
        String subscriber = extras.getString(ConstantApp.ACTION_KEY_SUBSCRIBER);
        int callType = extras.getInt(ConstantApp.ACTION_KEY_MakeOrReceive);
        int videoState = extras.getInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE);

        AgoraConnection aConnection = new AgoraConnection(getApplicationContext(), myUid, channel, subscriber, callType);
        aConnection.setVideoState(videoState);
        aConnection.setAddress(Uri.fromParts(SCHEME_AG, subscriber, null), TelecomManager.PRESENTATION_ALLOWED);
        aConnection.setCallerDisplayName(subscriber, TelecomManager.PRESENTATION_ALLOWED);
        aConnection.setRinging();
        CallSession.getInstance().setConnection(aConnection);

        return aConnection;
    }

    @Override
    public Connection onCreateOutgoingConnection(PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        log.info("onCreateOutgoingConnection: called. " + phoneAccount + " " + request);

        Bundle extras = request.getExtras();
        String account = extras.getString(ConstantApp.ACTION_KEY_UID);
        String channel = extras.getString(ConstantApp.ACTION_KEY_CHANNEL_NAME);
        String subscriber = extras.getString(ConstantApp.ACTION_KEY_SUBSCRIBER);
        int callType = extras.getInt(ConstantApp.ACTION_KEY_MakeOrReceive);
        int videoState = extras.getInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE);

        AgoraConnection aConnection = new AgoraConnection(getApplicationContext(), account, channel, subscriber, callType);
        aConnection.setVideoState(videoState);
        aConnection.setAddress(Uri.fromParts(SCHEME_AG, subscriber, null), TelecomManager.PRESENTATION_ALLOWED);
        aConnection.setCallerDisplayName(subscriber, TelecomManager.PRESENTATION_ALLOWED);
        aConnection.setDialing();
        CallSession.getInstance().setConnection(aConnection);

        return aConnection;
    }

    @Override
    public void onCreateIncomingConnectionFailed(
            PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        log.error("onCreateIncomingConnectionFailed: called. " + phoneAccount + " " + request);
    }

    @Override
    public void onCreateOutgoingConnectionFailed(
            PhoneAccountHandle phoneAccount, ConnectionRequest request) {
        log.error("onCreateOutgoingConnectionFailed: called. " + phoneAccount + " " + request);
    }
}
