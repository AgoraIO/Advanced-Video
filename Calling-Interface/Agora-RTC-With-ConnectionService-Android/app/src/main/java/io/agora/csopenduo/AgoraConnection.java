package io.agora.csopenduo;

import android.content.Intent;
import android.telecom.Connection;
import android.telecom.DisconnectCause;
import android.util.Log;


/**
 * Handles the Calling Connection.
 * Created by minming on 2019/04/08.
 */
public final class AgoraConnection extends Connection {

    private static final String TAG = AgoraConnection.class.getSimpleName();
    private static boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);
    private String mAccount;
    private String mChannel;
    private String mSubscriber;
    private int mType;

    public AgoraConnection(String account, String channel, String subscriber, int type) {
        mAccount = account;
        mChannel = channel;
        mSubscriber = subscriber;
        mType = type;
        if(type == Constant.CALL_OUT) {
            setConnectionCapabilities(PROPERTY_SELF_MANAGED);
        }

    }

    @Override
    public void onDisconnect() {
        Log.i(TAG, "onDisconnect: called.");
        setDisconnected(new DisconnectCause(DisconnectCause.LOCAL));
        destroy();
    }

    @Override
    public void onAbort() {
        Log.i(TAG, "onAbort: called.");
        setDisconnected(new DisconnectCause(DisconnectCause.ERROR));
        destroy();
    }

    @Override
    public void onAnswer() {
        Log.i(TAG, "onAnswer: called.");
        setDisconnected(new DisconnectCause(DisconnectCause.LOCAL)); // tricky way to dismiss the system incall ui
        Intent intent = new Intent(CallSession.getInstance().getContext(), CallActivity.class);
        intent.putExtra("account", mAccount);
        intent.putExtra("channelName", mChannel);
        intent.putExtra("subscriber", mSubscriber);
        intent.putExtra("type", mType);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        CallSession.getInstance().getContext().startActivity(intent);

    }

    @Override
    public void onReject() {
        Log.i(TAG, "onReject: called.");
        AGApplication.the().getmAgoraAPI().channelInviteRefuse(mChannel, mSubscriber, 0, "{\"status\":0}");
        setDisconnected(new DisconnectCause(DisconnectCause.REJECTED));
        destroy();
    }
}
