package io.agora.switchlive.rtc;

import android.util.Log;

import java.util.concurrent.ConcurrentHashMap;

import io.agora.rtc.IRtcEngineEventHandler;

public class MyEngineEventHandler {
    private static final String LOG_TAG = "EnginHandler";

    private final ConcurrentHashMap<AGEventHandler, Integer> mEventHandlerList = new ConcurrentHashMap<>();

    public void addEventHandler(AGEventHandler handler) {
        this.mEventHandlerList.put(handler, 0);
    }

    public void removeEventHandler(AGEventHandler handler) {
        this.mEventHandlerList.remove(handler);
    }

    final IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() {
        @Override
        public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
            Log.i(LOG_TAG, "onFirstRemoteVideoDecoded " + (uid & 0xFFFFFFFFL) + " " + width + " " + height + " " + elapsed);

            for (AGEventHandler handler : mEventHandlerList.keySet()) {
                handler.onFirstRemoteVideoDecoded(uid, width, height, elapsed);
            }
        }

        @Override
        public void onFirstRemoteVideoFrame(int uid, int width, int height, int elapsed) {
            Log.i(LOG_TAG, "onFirstRemoteVideoFrame " + (uid & 0xFFFFFFFFL) + " " + width + " " + height + " " + elapsed);
        }

        @Override
        public void onFirstLocalVideoFrame(int width, int height, int elapsed) {
            Log.i(LOG_TAG, "onFirstLocalVideoFrame " + width + " " + height + " " + elapsed);
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
            Log.i(LOG_TAG, "onUserJoined " + (uid & 0xFFFFFFFFL) + " " + elapsed);

            for (AGEventHandler handler : mEventHandlerList.keySet()) {
                handler.onUserJoined(uid, elapsed);
            }
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            for (AGEventHandler handler : mEventHandlerList.keySet()) {
                handler.onUserOffline(uid, reason);
            }
        }

        @Override
        public void onUserMuteVideo(int uid, boolean muted) {
        }

        @Override
        public void onRtcStats(RtcStats stats) {
        }

        @Override
        public void onLeaveChannel(RtcStats stats) {
            Log.i(LOG_TAG, "onLeaveChannel " + stats);
        }

        @Override
        public void onLastmileQuality(int quality) {
            Log.i(LOG_TAG, "onLastmileQuality " + quality);

            for (AGEventHandler handler : mEventHandlerList.keySet()) {
                handler.onLastmileQuality(quality);
            }
        }

        @Override
        public void onError(int err) {
            super.onError(err);

            Log.i(LOG_TAG, "onError " + err);
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(LOG_TAG, "onJoinChannelSuccess " + channel + " " + uid + " " + (uid & 0xFFFFFFFFL) + " " + elapsed);

            for (AGEventHandler handler : mEventHandlerList.keySet()) {
                handler.onJoinChannelSuccess(channel, uid, elapsed);
            }
        }

        public void onRejoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(LOG_TAG, "onRejoinChannelSuccess " + channel + " " + uid + " " + elapsed);
        }

        public void onWarning(int warn) {
            Log.i(LOG_TAG, "onWarning " + warn);
        }
    };
}
