package io.agora.openduo.model;

import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.LocalInvitation;
import io.agora.rtm.RemoteInvitation;

/**
 * Forward event from engine to UI
 * TODO make it simple, now we have lots of event in it
 */
public interface AGEventHandler {
    void onLoginSuccess(String uid);

    void onLoginFailed(String uid, ErrorInfo error);

    void onPeerOnlineStatusQueried(String uid, boolean online);

    void onInvitationReceivedByPeer(LocalInvitation invitation);

    void onLocalInvitationAccepted(LocalInvitation invitation, String response);

    void onLocalInvitationRefused(LocalInvitation invitation, String response);

    void onLocalInvitationCanceled(LocalInvitation invitation);

    void onInvitationReceived(RemoteInvitation invitation);

    void onInvitationRefused(RemoteInvitation invitation);

    void onRemoteInvitationCanceled(RemoteInvitation invitation);

    void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed);

    void onJoinChannelSuccess(String channel, int uid, int elapsed);

    void onUserOffline(int uid, int reason);

    void onExtraCallback(int type, Object... data);

    void onLastmileQuality(int quality);

    void onLastmileProbeResult(IRtcEngineEventHandler.LastmileProbeResult result);

    int EVENT_TYPE_ON_DATA_CHANNEL_MSG = 3;

    int EVENT_TYPE_ON_USER_VIDEO_MUTED = 6;

    int EVENT_TYPE_ON_USER_AUDIO_MUTED = 7;

    int EVENT_TYPE_ON_SPEAKER_STATS = 8;

    int EVENT_TYPE_ON_AGORA_MEDIA_ERROR = 9;

    int EVENT_TYPE_ON_USER_VIDEO_STATS = 10;

    int EVENT_TYPE_ON_APP_ERROR = 13;

    int EVENT_TYPE_ON_AUDIO_ROUTE_CHANGED = 18;

    int EVENT_TYPE_ON_RELOGIN_NEEDED = 19;
}
