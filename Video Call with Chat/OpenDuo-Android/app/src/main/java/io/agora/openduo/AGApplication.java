package io.agora.openduo;

import android.app.Application;
import android.util.Log;

import io.agora.AgoraAPIOnlySignal;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;


public class AGApplication extends Application {
    private final String TAG = AGApplication.class.getSimpleName();

    private static AGApplication mInstance;
    private AgoraAPIOnlySignal m_agoraAPI;
    private RtcEngine mRtcEngine;

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

    public AgoraAPIOnlySignal getmAgoraAPI() {
        return m_agoraAPI;
    }

    private void setupAgoraEngine() {
        String appID = getString(R.string.agora_app_id);

        try {
            m_agoraAPI = AgoraAPIOnlySignal.getInstance(this, appID);
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

    public interface OnAgoraEngineInterface {
        void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed);

        void onUserOffline(int uid, int reason);

        void onUserMuteVideo(final int uid, final boolean muted);

        void onJoinChannelSuccess(String channel, int uid, int elapsed);
    }
}
