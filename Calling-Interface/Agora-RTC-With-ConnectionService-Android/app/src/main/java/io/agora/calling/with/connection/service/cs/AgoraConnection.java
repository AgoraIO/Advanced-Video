package io.agora.calling.with.connection.service.cs;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.telecom.Connection;
import android.telecom.DisconnectCause;
import android.util.Log;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.agora.calling.with.connection.service.AGApplication;
import io.agora.calling.with.connection.service.model.ConstantApp;
import io.agora.calling.with.connection.service.model.EngineConfig;
import io.agora.calling.with.connection.service.model.WorkerThread;
import io.agora.calling.with.connection.service.ui.CallActivity;

/**
 * Handles the Calling Connection.
 * Created by minming on 2019/04/08.
 */
public final class AgoraConnection extends Connection {
    private final static Logger log = LoggerFactory.getLogger(AgoraConnection.class);
    private static boolean DEBUG = Log.isLoggable(log.getName(), Log.DEBUG);

    private Context mContext;

    private String mMyUid;
    private String mChannel;
    private String mSubscriber;
    private int mType;

    public AgoraConnection(Context context, String myUid, String channel, String subscriber, int type) {
        this.mContext = context;

        mMyUid = myUid;
        mChannel = channel;
        mSubscriber = subscriber;
        mType = type;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
//          setConnectionProperties(PROPERTY_SELF_MANAGED); // If apply totally customized in-call UI
        }
        setConnectionCapabilities(CAPABILITY_MUTE); // apply according to your scenario
    }

    @Override
    public void onShowIncomingCallUi() {
        log.debug("onShowIncomingCallUi: called.");
        super.onShowIncomingCallUi();
    }

    @Override
    public void onDisconnect() {
        log.debug("onDisconnect: called.");
        super.onDisconnect();

        setDisconnected(new DisconnectCause(DisconnectCause.LOCAL));
        destroy();
    }

    @Override
    public void onAbort() {
        log.debug("onAbort: called.");
        super.onAbort();

        setDisconnected(new DisconnectCause(DisconnectCause.CANCELED));
        destroy();
    }

    @Override
    public void onAnswer() {
        log.debug("onAnswer: called.");
        super.onAnswer();

        setDisconnected(new DisconnectCause(DisconnectCause.LOCAL)); // tricky way to dismiss the system incall ui

        Intent intent = new Intent(mContext, CallActivity.class);
        intent.putExtra(ConstantApp.ACTION_KEY_UID, mMyUid);
        intent.putExtra(ConstantApp.ACTION_KEY_CHANNEL_NAME, mChannel);
        intent.putExtra(ConstantApp.ACTION_KEY_SUBSCRIBER, mSubscriber);
        intent.putExtra(ConstantApp.ACTION_KEY_MakeOrReceive, mType);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        mContext.startActivity(intent);
    }

    @Override
    public void onReject() {
        log.debug("onReject: called.");
        super.onReject();

        WorkerThread worker = ((AGApplication) mContext.getApplicationContext()).getWorkerThread();
        EngineConfig config = worker.getEngineConfig();

        // "status": 0 // Default
        // "status": 1 // Busy
        config.mRemoteInvitation.setResponse("{\"status\":0}");
        worker.hangupTheCall(config.mRemoteInvitation);

        setDisconnected(new DisconnectCause(DisconnectCause.REJECTED));
        destroy();
    }
}
