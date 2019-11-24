package io.agora.advancedvideo;

import android.app.Application;
import android.content.SharedPreferences;

import io.agora.advancedvideo.rtc.AgoraEventHandler;
import io.agora.advancedvideo.rtc.EngineConfig;
import io.agora.advancedvideo.rtc.EventHandler;
import io.agora.advancedvideo.utils.FileUtil;
import io.agora.advancedvideo.utils.PrefManager;
import io.agora.rtc.RtcEngine;

public class AgoraApplication extends Application {
    private RtcEngine mRtcEngine;
    private EngineConfig mGlobalConfig = new EngineConfig();
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

        initConfig();
    }

    private void initConfig() {
        SharedPreferences pref = PrefManager.getPreferences(getApplicationContext() );
        mGlobalConfig.setVideoDimenIndex(pref.getInt(
                Constants.PREF_RESOLUTION_IDX, Constants.DEFAULT_PROFILE_IDX));

        // boolean showStats = pref.getBoolean(Constants.PREF_ENABLE_STATS, false);
        // mGlobalConfig.setIfShowVideoStats(showStats);
        // mStatsManager.enableStats(showStats);
    }

    public RtcEngine rtcEngine() { return mRtcEngine; }

    public EngineConfig engineConfig() { return mGlobalConfig; }

    public void registerEventHandler(EventHandler handler) { mHandler.addHandler(handler); }

    public void removeEventHandler(EventHandler handler) { mHandler.removeHandler(handler); }

    @Override
    public void onTerminate() {
        super.onTerminate();
        RtcEngine.destroy();
    }
}
