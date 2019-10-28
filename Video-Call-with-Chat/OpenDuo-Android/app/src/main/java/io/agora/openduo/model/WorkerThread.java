package io.agora.openduo.model;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;
import android.view.SurfaceView;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashSet;
import java.util.Map;

import io.agora.openduo.R;
import io.agora.rtc.Constants;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.ResultCallback;
import io.agora.rtm.RtmCallManager;
import io.agora.rtm.RtmChannel;
import io.agora.rtm.RtmClient;

public class WorkerThread extends Thread {
    private final static Logger log = LoggerFactory.getLogger(WorkerThread.class);

    private final Context mContext;

    private static final int ACTION_WORKER_THREAD_QUIT = 0X1010; // quit this thread

    private static final int ACTION_WORKER_JOIN_CHANNEL = 0X2010;

    private static final int ACTION_WORKER_LEAVE_CHANNEL = 0X2011;

    private static final int ACTION_WORKER_CONFIG_ENGINE = 0X2012;

    private static final int ACTION_WORKER_PREVIEW = 0X2014;

    private static final int ACTION_WORKER_CONNECT_TO_RTM_SERVICE = 0X2015;

    private static final int ACTION_WORKER_DISCONNECT_FROM_RTM_SERVICE = 0X2016;

    private static final int ACTION_WORKER_QUERY_PEERS_ONLINE_STATUS = 0X2017;

    private static final int ACTION_WORKER_MAKE_A_CALL = 0X2018;

    private static final int ACTION_WORKER_MAKE_THE_CALL = 0X2019;

    private static final int ACTION_WORKER_HANG_UP_THE_CALL = 0X2020;

    private static final class WorkerThreadHandler extends Handler {

        private WorkerThread mWorkerThread;

        WorkerThreadHandler(WorkerThread thread) {
            this.mWorkerThread = thread;
        }

        public void release() {
            mWorkerThread = null;
        }

        @Override
        public void handleMessage(Message msg) {
            if (this.mWorkerThread == null) {
                log.warn("handler is already released! " + msg.what);
                return;
            }

            String[] data;
            String peerUid;

            switch (msg.what) {
                case ACTION_WORKER_THREAD_QUIT:
                    mWorkerThread.exit();
                    break;
                case ACTION_WORKER_CONNECT_TO_RTM_SERVICE:
                    data = (String[]) msg.obj;
                    mWorkerThread.connectToRtmService(data[0]);
                    break;
                case ACTION_WORKER_DISCONNECT_FROM_RTM_SERVICE:
                    mWorkerThread.disconnectFromRtmService();
                    break;
                case ACTION_WORKER_QUERY_PEERS_ONLINE_STATUS:
                    peerUid = (String) msg.obj;
                    mWorkerThread.queryPeersOnlineStatus(peerUid);
                    break;
                case ACTION_WORKER_MAKE_A_CALL:
                    data = (String[]) msg.obj;
                    mWorkerThread.makeACall(data[0], data[1]);
                    break;
                case ACTION_WORKER_MAKE_THE_CALL:
                    mWorkerThread.answerTheCall((RemoteInvitation) msg.obj);
                    break;
                case ACTION_WORKER_HANG_UP_THE_CALL:
                    mWorkerThread.hangupTheCall((RemoteInvitation) msg.obj);
                    break;
                case ACTION_WORKER_JOIN_CHANNEL:
                    data = (String[]) msg.obj;
                    mWorkerThread.joinChannel(data[0], msg.arg1);
                    break;
                case ACTION_WORKER_LEAVE_CHANNEL:
                    String channel = (String) msg.obj;
                    mWorkerThread.leaveChannel(channel);
                    break;
                case ACTION_WORKER_CONFIG_ENGINE:
                    Object[] configData = (Object[]) msg.obj;
                    mWorkerThread.configEngine((VideoEncoderConfiguration.VideoDimensions) configData[0], (String) configData[1], (String) configData[2]);
                    break;
                case ACTION_WORKER_PREVIEW:
                    Object[] previewData = (Object[]) msg.obj;
                    mWorkerThread.preview((boolean) previewData[0], (SurfaceView) previewData[1], (int) previewData[2]);
                    break;
            }
        }
    }

    private WorkerThreadHandler mWorkerHandler;

    private boolean mReady;

    public final void waitForReady() {
        while (!mReady) {
            try {
                Thread.sleep(20);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            log.debug("wait for " + WorkerThread.class.getSimpleName());
        }
    }

    @Override
    public void run() {
        log.trace("start to run");
        Looper.prepare();

        mWorkerHandler = new WorkerThreadHandler(this);

        ensureRtmClientReadyLock();
        ensureRtcEngineReadyLock();

        mReady = true;

        // enter thread looper
        Looper.loop();
    }

    private RtcEngine mRtcEngine;

    private RtmClient mRtmClient;

    private RtmChannel mRtmChannelObj;

    public final void enablePreProcessor() {
    }

    public final void setPreParameters(float lightness, int smoothness) {
    }

    public final void disablePreProcessor() {
    }

    public final void connectToRtmService(final String uid) {
        if (Thread.currentThread() != this) {
            log.warn("connectToRtmService() - worker thread asynchronously " + uid);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_CONNECT_TO_RTM_SERVICE;
            envelop.obj = new String[]{uid};
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        ensureRtmClientReadyLock();

        mRtmClient.logout(new ResultCallback<Void>() { // to avoid LOGIN_ERR_ALREADY_LOGIN
            @Override
            public void onSuccess(Void aVoid) {
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
            }
        });

        mRtmClient.login("", uid, new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {
                mEngineEventHandler.notifyRTMLoginSuccess(uid);
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                mEngineEventHandler.notifyRTMLoginFailure(uid, errorInfo);
            }
        });

        RtmCallManager callMgr = mRtmClient.getRtmCallManager();
        callMgr.setEventListener(mEngineEventHandler.mRtmCallEventHandler);

        log.debug("connectToRtmService " + uid);
    }

    public final void disconnectFromRtmService() {
        if (Thread.currentThread() != this) {
            log.warn("disconnectFromRtmService() - worker thread asynchronously");
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_DISCONNECT_FROM_RTM_SERVICE;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        mRtmClient.logout(new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {

            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {

            }
        });

        log.debug("disconnectFromRtmService");
    }

    public final void queryPeersOnlineStatus(final String peerUid) {
        if (Thread.currentThread() != this) {
            log.warn("queryPeersOnlineStatus() - worker thread asynchronously " + peerUid);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_QUERY_PEERS_ONLINE_STATUS;
            envelop.obj = peerUid;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        HashSet<String> peers = new HashSet<>();
        peers.add(peerUid);

        mRtmClient.queryPeersOnlineStatus(peers, new ResultCallback<Map<String, Boolean>>() {
            @Override
            public void onSuccess(Map<String, Boolean> stringBooleanMap) {
                log.debug("queryPeersOnlineStatus-onSuccess " + peerUid + " " + stringBooleanMap.get(peerUid));
                mEngineEventHandler.notifyRTMPeerOnlineStatusQueried(peerUid, stringBooleanMap.get(peerUid));
            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {
                log.debug("queryPeersOnlineStatus-onFailure " + peerUid + " " + errorInfo);
                mEngineEventHandler.notifyRTMPeerOnlineStatusQueried(peerUid, false);
            }
        });

        log.debug("queryPeersOnlineStatus " + peerUid);
    }

    public final void makeACall(final String peerUid, final String channel) {
        if (Thread.currentThread() != this) {
            log.warn("makeACall() - worker thread asynchronously " + peerUid + " " + channel);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_MAKE_A_CALL;
            envelop.obj = new String[]{peerUid, channel};
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        RtmCallManager callMgr = mRtmClient.getRtmCallManager();

        mEngineConfig.mInvitation = callMgr.createLocalInvitation(peerUid);
        mEngineConfig.mInvitation.setChannelId(channel); // TODO Available in RTM 1.0, use Content so far
        mEngineConfig.mInvitation.setContent(channel);

        callMgr.sendLocalInvitation(mEngineConfig.mInvitation, new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {

            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {

            }
        });

        log.debug("makeACall " + peerUid + " " + mEngineConfig.mInvitation + " " + callMgr + " " + mEngineConfig.mInvitation.getChannelId() + " " + mEngineConfig.mInvitation.getContent());
    }

    public final void answerTheCall(final RemoteInvitation invitation) {
        if (Thread.currentThread() != this) {
            log.warn("answerTheCall() - worker thread asynchronously " + invitation);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_MAKE_THE_CALL;
            envelop.obj = invitation;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        RtmCallManager callMgr = mRtmClient.getRtmCallManager();

        callMgr.acceptRemoteInvitation(invitation, new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {

            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {

            }
        });

        log.debug("answerTheCall " + invitation + " " + invitation.getCallerId() + " " + invitation.getChannelId() + " " + callMgr);
    }

    public final void hangupTheCall(final RemoteInvitation invitation) {
        if (Thread.currentThread() != this) {
            log.warn("hangupTheCall() - worker thread asynchronously " + invitation);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_HANG_UP_THE_CALL;
            envelop.obj = invitation;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        RtmCallManager callMgr = mRtmClient.getRtmCallManager();

        if (invitation == null) {
            callMgr.cancelLocalInvitation(mEngineConfig.mInvitation, new ResultCallback<Void>() {
                @Override
                public void onSuccess(Void aVoid) {

                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {

                }
            });
            log.debug("hangupTheCall(local) " + mEngineConfig.mInvitation + " " + callMgr);
            return;
        }

        callMgr.refuseRemoteInvitation(invitation, new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {

            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {

            }
        });

        log.debug("hangupTheCall " + invitation + " " + invitation.getCallerId() + " " + invitation.getChannelId() + " " + callMgr);
    }

    public final void joinChannel(final String channel, int uid) {
        if (Thread.currentThread() != this) {
            log.warn("joinChannel() - worker thread asynchronously " + channel + " " + uid);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_JOIN_CHANNEL;
            envelop.obj = new String[]{channel};
            envelop.arg1 = uid;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        ensureRtmClientReadyLock();
        ensureRtcEngineReadyLock();
        mRtcEngine.joinChannel(null, channel, "OpenDuo", uid);

        if (mRtmChannelObj == null) {
            mRtmChannelObj = mRtmClient.createChannel(channel, mEngineEventHandler.mRtmChannelEventHandler);
        } else if (!TextUtils.equals(mRtmChannelObj.getId(), channel)) { // Leave channel
            mRtmChannelObj.leave(new ResultCallback<Void>() {
                @Override
                public void onSuccess(Void aVoid) {
                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {
                }
            });
            mRtmChannelObj.release();
            mRtmChannelObj = null;
            mRtmChannelObj = mRtmClient.createChannel(channel, mEngineEventHandler.mRtmChannelEventHandler);
        }

        mRtmChannelObj.join(new ResultCallback<Void>() {
            @Override
            public void onSuccess(Void aVoid) {

            }

            @Override
            public void onFailure(ErrorInfo errorInfo) {

            }
        });

        mEngineConfig.mChannel = channel;

        enablePreProcessor();
        log.debug("joinChannel " + channel + " " + uid);
    }

    public final void leaveChannel(String channel) {
        if (Thread.currentThread() != this) {
            log.warn("leaveChannel() - worker thread asynchronously " + channel);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_LEAVE_CHANNEL;
            envelop.obj = channel;
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        if (mRtmChannelObj != null) {
            mRtmChannelObj.leave(new ResultCallback<Void>() {
                @Override
                public void onSuccess(Void aVoid) {
                }

                @Override
                public void onFailure(ErrorInfo errorInfo) {
                }
            });
            mRtmChannelObj.release();
            mRtmChannelObj = null;
        }

        if (mRtcEngine != null) {
            mRtcEngine.leaveChannel();
        }

        disablePreProcessor();

        mEngineConfig.reset();
        log.debug("leaveChannel " + channel);
    }

    private EngineConfig mEngineConfig;

    public final EngineConfig getEngineConfig() {
        return mEngineConfig;
    }

    private final MyEngineEventHandler mEngineEventHandler;

    public final void configEngine(VideoEncoderConfiguration.VideoDimensions videoDimension, String encryptionKey, String encryptionMode) {
        if (Thread.currentThread() != this) {
            log.warn("configEngine() - worker thread asynchronously " + videoDimension + " " + encryptionMode);
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_CONFIG_ENGINE;
            envelop.obj = new Object[]{videoDimension, encryptionKey, encryptionMode};
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        ensureRtcEngineReadyLock();
        mRtcEngine.enableVideo();

        mEngineConfig.mVideoDimension = videoDimension;

        if (!TextUtils.isEmpty(encryptionKey)) {
            mRtcEngine.setEncryptionMode(encryptionMode);

            mRtcEngine.setEncryptionSecret(encryptionKey);
        }

        // mRtcEngine.setVideoProfile(mEngineConfig.mVideoProfile, false); // for sdk earlier than 2.3.0
        mRtcEngine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(videoDimension,
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT));

        log.debug("configEngine " + mEngineConfig.mVideoDimension + " " + encryptionMode);
    }

    public final void preview(boolean start, SurfaceView view, int uid) {
        if (Thread.currentThread() != this) {
            log.warn("preview() - worker thread asynchronously " + start + " " + view + " " + (uid & 0XFFFFFFFFL));
            Message envelop = new Message();
            envelop.what = ACTION_WORKER_PREVIEW;
            envelop.obj = new Object[]{start, view, uid};
            mWorkerHandler.sendMessage(envelop);
            return;
        }

        ensureRtcEngineReadyLock();
        if (start) {
            mRtcEngine.setupLocalVideo(new VideoCanvas(view, VideoCanvas.RENDER_MODE_HIDDEN, uid));
            mRtcEngine.startPreview();
        } else {
            mRtcEngine.stopPreview();
        }
    }

    public static String getDeviceID(Context context) {
        // XXX according to the API docs, this value may change after factory reset
        // use Android id as device id
        return Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
    }

    private RtmClient ensureRtmClientReadyLock() {
        if (mRtmClient == null) {
            String appId = mContext.getString(R.string.agora_app_id);
            if (TextUtils.isEmpty(appId)) {
                throw new RuntimeException("NEED TO use your App ID, get your own ID at https://dashboard.agora.io/");
            }
            try {
                mRtmClient = RtmClient.createInstance(mContext, appId, mEngineEventHandler.mRtmEventHandler);
            } catch (Exception e) {
                log.error(Log.getStackTraceString(e));
                throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
            }
        }
        return mRtmClient;
    }

    private RtcEngine ensureRtcEngineReadyLock() {
        if (mRtcEngine == null) {
            String appId = mContext.getString(R.string.agora_app_id);
            if (TextUtils.isEmpty(appId)) {
                throw new RuntimeException("NEED TO use your App ID, get your own ID at https://dashboard.agora.io/");
            }
            try {
                mRtcEngine = RtcEngine.create(mContext, appId, mEngineEventHandler.mRtcEventHandler);
            } catch (Exception e) {
                log.error(Log.getStackTraceString(e));
                throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
            }
            mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_COMMUNICATION);
            mRtcEngine.enableVideo();
            mRtcEngine.enableAudioVolumeIndication(200, 3, true); // 200 ms
        }
        return mRtcEngine;
    }

    public MyEngineEventHandler eventHandler() {
        return mEngineEventHandler;
    }

    public RtcEngine getRtcEngine() {
        return mRtcEngine;
    }

    /**
     * call this method to exit
     * should ONLY call this method when this thread is running
     */
    public final void exit() {
        if (Thread.currentThread() != this) {
            log.warn("exit() - exit app thread asynchronously");
            mWorkerHandler.sendEmptyMessage(ACTION_WORKER_THREAD_QUIT);
            return;
        }

        mReady = false;

        // TODO should remove all pending(read) messages

        log.debug("exit() > start");

        // exit thread looper
        Looper.myLooper().quit();

        mWorkerHandler.release();

        log.debug("exit() > end");
    }

    public WorkerThread(Context context) {
        this.mContext = context;

        this.mEngineConfig = new EngineConfig();
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        this.mEngineConfig.mUid = pref.getInt(ConstantApp.PrefManager.PREF_PROPERTY_UID, 0);

        this.mEngineEventHandler = new MyEngineEventHandler(mContext, this.mEngineConfig);
    }
}

