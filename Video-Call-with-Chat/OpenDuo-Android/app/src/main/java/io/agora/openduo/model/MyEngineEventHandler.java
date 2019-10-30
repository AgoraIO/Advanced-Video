package io.agora.openduo.model;


import android.content.Context;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;
import io.agora.rtm.RtmCallEventListener;
import io.agora.rtm.RtmChannelAttribute;
import io.agora.rtm.RtmChannelListener;
import io.agora.rtm.RtmChannelMember;
import io.agora.rtm.RtmClientListener;
import io.agora.rtm.RtmMessage;
import io.agora.rtm.RtmStatusCode;

public class MyEngineEventHandler {

    public MyEngineEventHandler(Context ctx, EngineConfig config) {
        this.mContext = ctx;
        this.mConfig = config;
    }

    private final EngineConfig mConfig;

    private final Context mContext;

    private final ConcurrentHashMap<AGEventHandler, Integer> mEventHandlerList = new ConcurrentHashMap<>();

    public void addEventHandler(AGEventHandler handler) {
        this.mEventHandlerList.put(handler, 0);
    }

    public void removeEventHandler(AGEventHandler handler) {
        this.mEventHandlerList.remove(handler);
    }

    final IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() {
        private final Logger log = LoggerFactory.getLogger(this.getClass());

        @Override
        public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
            log.debug("onFirstRemoteVideoDecoded " + (uid & 0xFFFFFFFFL) + " " + width + " " + height + " " + elapsed);

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onFirstRemoteVideoDecoded(uid, width, height, elapsed);
            }
        }

        @Override
        public void onFirstLocalVideoFrame(int width, int height, int elapsed) {
            log.debug("onFirstLocalVideoFrame " + width + " " + height + " " + elapsed);
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            log.debug("onUserOffline " + (uid & 0xFFFFFFFFL) + " " + reason);

            // FIXME this callback may return times
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onUserOffline(uid, reason);
            }
        }

        @Override
        public void onUserMuteVideo(int uid, boolean muted) {
            log.debug("onUserMuteVideo " + (uid & 0xFFFFFFFFL) + " " + muted);

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_USER_VIDEO_MUTED, uid, muted);
            }
        }

        @Override
        public void onRtcStats(RtcStats stats) {
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            log.debug("onRemoteVideoStats " + stats.uid + " " + stats.receivedBitrate + " " + stats.rendererOutputFrameRate + " " + stats.width + " " + stats.height);

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_USER_VIDEO_STATS, stats);
            }
        }

        @Override
        public void onAudioVolumeIndication(AudioVolumeInfo[] speakerInfos, int totalVolume) {
            if (speakerInfos == null) {
                // quick and dirty fix for crash
                // TODO should reset UI for no sound
                return;
            }

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_SPEAKER_STATS, (Object) speakerInfos);
            }
        }

        @Override
        public void onLeaveChannel(RtcStats stats) {

        }

        @Override
        public void onLastmileQuality(int quality) {
            log.debug("onLastmileQuality " + quality);
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onLastmileQuality(quality);
            }
        }

        @Override
        public void onLastmileProbeResult(IRtcEngineEventHandler.LastmileProbeResult result) {
            log.debug("onLastmileProbeResult " + result);
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onLastmileProbeResult(result);
            }
        }

        @Override
        public void onError(int error) {
            log.debug("onError " + error + " " + RtcEngine.getErrorDescription(error));

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_AGORA_MEDIA_ERROR, error, RtcEngine.getErrorDescription(error));
            }
        }

        @Override
        public void onConnectionLost() {
            log.debug("onConnectionLost");

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_APP_ERROR, ConstantApp.AppError.NO_NETWORK_CONNECTION);
            }
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            log.debug("onJoinChannelSuccess " + channel + " " + uid + " " + (uid & 0xFFFFFFFFL) + " " + elapsed);

            mConfig.mUid = uid;

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onJoinChannelSuccess(channel, uid, elapsed);
            }
        }

        public void onRejoinChannelSuccess(String channel, int uid, int elapsed) {
            log.debug("onRejoinChannelSuccess " + channel + " " + uid + " " + elapsed);
        }

        @Override
        public void onAudioRouteChanged(int routing) {
            log.debug("onAudioRouteChanged " + routing);

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_AUDIO_ROUTE_CHANGED, routing);
            }
        }

        public void onWarning(int warn) {
            log.debug("onWarning " + warn);
        }
    };

    final RtmClientListener mRtmEventHandler = new RtmClientListener() {
        private final Logger log = LoggerFactory.getLogger(this.getClass());

        @Override
        public void onConnectionStateChanged(int state, int reason) {
            log.debug("onConnectionStateChanged " + state + " " + reason);

            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            if (RtmStatusCode.ConnectionState.CONNECTION_STATE_DISCONNECTED == state) {
                while (it.hasNext()) {
                    AGEventHandler handler = it.next();
                    handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_RELOGIN_NEEDED, false);
                }
            } else if (RtmStatusCode.ConnectionState.CONNECTION_STATE_ABORTED == state) {
                while (it.hasNext()) {
                    AGEventHandler handler = it.next();
                    handler.onExtraCallback(AGEventHandler.EVENT_TYPE_ON_RELOGIN_NEEDED, true); // banned
                }
            }
        }

        @Override
        public void onMessageReceived(RtmMessage rtmMessage, String s) {

        }

        @Override
        public void onTokenExpired() {

        }
    };

    final RtmChannelListener mRtmChannelEventHandler = new RtmChannelListener() {
        @Override
        public void onMessageReceived(RtmMessage rtmMessage, RtmChannelMember rtmChannelMember) {

        }

        @Override
        public void onMemberJoined(RtmChannelMember rtmChannelMember) {

        }

        @Override
        public void onMemberLeft(RtmChannelMember rtmChannelMember) {

        }

        @Override
        public void onMemberCountUpdated(int memberCount) {

        }

        @Override
        public void onAttributesUpdated(List<RtmChannelAttribute> attributeList) {

        }
    };

    final RtmCallEventListener mRtmCallEventHandler = new RtmCallEventListener() {

        @Override
        public void onLocalInvitationReceivedByPeer(LocalInvitation localInvitation) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onInvitationReceivedByPeer(localInvitation);
            }
        }

        @Override
        public void onLocalInvitationAccepted(LocalInvitation localInvitation, String response) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onLocalInvitationAccepted(localInvitation, response);
            }
        }

        @Override
        public void onLocalInvitationRefused(LocalInvitation localInvitation, String response) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onLocalInvitationRefused(localInvitation, response);
            }
        }

        @Override
        public void onLocalInvitationCanceled(LocalInvitation localInvitation) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onLocalInvitationCanceled(localInvitation);
            }
        }

        @Override
        public void onLocalInvitationFailure(LocalInvitation localInvitation, int i) {

        }

        @Override
        public void onRemoteInvitationReceived(RemoteInvitation remoteInvitation) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onInvitationReceived(remoteInvitation);
            }
        }

        @Override
        public void onRemoteInvitationAccepted(RemoteInvitation remoteInvitation) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
        }

        @Override
        public void onRemoteInvitationRefused(RemoteInvitation remoteInvitation) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onInvitationRefused(remoteInvitation);
            }
        }

        @Override
        public void onRemoteInvitationCanceled(RemoteInvitation remoteInvitation) {
            Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
            while (it.hasNext()) {
                AGEventHandler handler = it.next();
                handler.onRemoteInvitationCanceled(remoteInvitation);
            }
        }

        @Override
        public void onRemoteInvitationFailure(RemoteInvitation remoteInvitation, int i) {

        }
    };

    final void notifyRTMLoginSuccess(String uid) {
        Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
        while (it.hasNext()) {
            AGEventHandler handler = it.next();
            handler.onLoginSuccess(uid);
        }
    }

    final void notifyRTMLoginFailure(String uid, ErrorInfo error) {
        Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
        while (it.hasNext()) {
            AGEventHandler handler = it.next();
            handler.onLoginFailed(uid, error);
        }
    }

    final void notifyRTMPeerOnlineStatusQueried(String uid, boolean online) {
        Iterator<AGEventHandler> it = mEventHandlerList.keySet().iterator();
        while (it.hasNext()) {
            AGEventHandler handler = it.next();
            handler.onPeerOnlineStatusQueried(uid, online);
        }
    }

}

