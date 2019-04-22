package io.agora.openduo;

import android.app.Application;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.RtmClient;
import io.agora.rtm.RtmClientListener;
import io.agora.rtm.RtmMessage;


public class AGApplication extends Application {
    private final String TAG = AGApplication.class.getSimpleName();

    private static AGApplication mInstance;
    private RtmClient rtmClient;
    private RtcEngine mRtcEngine;
    private RemoteInvitation remoteInvitation;
    private LocalInvitation localInvitation;

    public static AGApplication the() {
        return mInstance;
    }

    public AGApplication() {
        mInstance = this;
    }

    private OnAgoraEngineInterface onAgoraEngineInterface;

    private final IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() {
        @Override
        public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
            if (onAgoraEngineInterface != null) {
                onAgoraEngineInterface.onFirstRemoteVideoDecoded(uid, width, height, elapsed);
            }
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, "onUserOffline uid: " + uid + " reason:" + reason);
            if (onAgoraEngineInterface != null) {
                onAgoraEngineInterface.onUserOffline(uid, reason);
            }
        }

        @Override
        public void onUserMuteVideo(final int uid, final boolean muted) {
            if (onAgoraEngineInterface != null) {
                onAgoraEngineInterface.onUserMuteVideo(uid, muted);
            }
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            super.onJoinChannelSuccess(channel, uid, elapsed);
            Log.i(TAG, "onJoinChannelSuccess channel:" + channel + " uid:" + uid);
            if (onAgoraEngineInterface != null) {
                onAgoraEngineInterface.onJoinChannelSuccess(channel, uid, elapsed);
            }
        }

    };

    @Override
    public void onCreate() {
        super.onCreate();

        setupAgoraEngine();
    }

    public RtcEngine getmRtcEngine() {
        return mRtcEngine;
    }

    public RtmClient getRTMClient() {
        return rtmClient;
    }

    private void setupAgoraEngine() {
        String appID = getString(R.string.agora_app_id);

        try {
            rtmClient = RtmClient.createInstance(this, appID, new RtmClientListener() {
                @Override
                public void onConnectionStateChanged(int state, int reason) {
                    Log.i(TAG, "RtmClient onConnectionStateChanged state = " + state + " and reason = " + reason);
                }

                @Override
                public void onMessageReceived(RtmMessage rtmMessage, String peerId) {
                    Log.i(TAG, "RtmClient onMessageReceived rtmMessage_text = " + rtmMessage.getText() + " and peerId = " + peerId);
                }
            });
            // set log filter to debug
            rtmClient.setLogFilter(RtmClient.LOG_FILTER_DEBUG);

            String ts = new SimpleDateFormat("yyyyMMdd", Locale.ENGLISH).format(new Date());
            String rtmLogPath = Environment.getExternalStorageDirectory().toString() + "/" + getPackageName() + "/rtm/";
            File rtmLogDir = new File(rtmLogPath);
            rtmLogDir.mkdirs();
            rtmClient.setLogFile(rtmLogPath + ts + ".log");
            Log.e(TAG, "RTM_log_path = " + rtmLogPath);

            mRtcEngine = RtcEngine.create(getBaseContext(), appID, mRtcEventHandler);
            Log.i(TAG, "setupAgoraEngine mRtcEngine :" + mRtcEngine);
        } catch (Exception e) {
            Log.e(TAG, Log.getStackTraceString(e));
            throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
        }
    }

    public void setOnAgoraEngineInterface(OnAgoraEngineInterface onAgoraEngineInterface) {
        this.onAgoraEngineInterface = onAgoraEngineInterface;
    }

    public void holdRemoteInvitation(RemoteInvitation remoteInvitation) {
        this.remoteInvitation = remoteInvitation;
    }

    public void holdLocalInvitation(LocalInvitation localInvitation) {
        this.localInvitation = localInvitation;
    }

    public RemoteInvitation getRemoteInvitation() {
        return remoteInvitation;
    }

    public LocalInvitation getLocalInvitation() {
        return localInvitation;
    }

    public interface OnAgoraEngineInterface {
        void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed);

        void onUserOffline(int uid, int reason);

        void onUserMuteVideo(final int uid, final boolean muted);

        void onJoinChannelSuccess(String channel, int uid, int elapsed);
    }
}
