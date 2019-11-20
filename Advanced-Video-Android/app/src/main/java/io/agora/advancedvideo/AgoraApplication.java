package io.agora.advancedvideo;

import android.app.Application;

import io.agora.advancedvideo.rtc.AgoraEventHandler;
import io.agora.advancedvideo.rtc.EventHandler;
import io.agora.advancedvideo.utils.FileUtil;
import io.agora.rtc.RtcEngine;

public class AgoraApplication extends Application {
    private RtcEngine mRtcEngine;
    private AgoraEventHandler mHandler = new AgoraEventHandler();

    @Override
    public void onCreate() {
        super.onCreate();
        try {
            mRtcEngine = RtcEngine.create(getApplicationContext(), getString(R.string.PRIVATE_APP_ID), mHandler);
            mRtcEngine.setChannelProfile(io.agora.rtc.Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
            mRtcEngine.enableVideo();
            mRtcEngine.setLogFile(FileUtil.initializeLogFile(this));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public RtcEngine rtcEngine() { return mRtcEngine; }

    public void registerEventHandler(EventHandler handler) { mHandler.addHandler(handler); }

    public void removeEventHandler(EventHandler handler) { mHandler.removeHandler(handler); }

    @Override
    public void onTerminate() {
        super.onTerminate();
        RtcEngine.destroy();
    }
}
