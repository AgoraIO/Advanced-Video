package io.agora.rtc.ss.app;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;

public class AudienceActivity extends Activity {

    private static final String TAG = "audience";

    private FrameLayout mFlRoot;
    private RtcEngine mRtcEngine;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audience);

        mFlRoot = (FrameLayout) findViewById(R.id.fl_root);

        initEngineAndJoin();
    }

    private void initEngineAndJoin() {
        try {
            mRtcEngine = RtcEngine.create(getApplicationContext(), getString(R.string.agora_app_id), new IRtcEngineEventHandler() {

                @Override
                public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
                    Log.d(TAG, "onJoinChannelSuccess: " + (uid&0xFFFFFFL));
                }

                @Override
                public void onUserJoined(final int uid, int elapsed) {
                    Log.d(TAG, "onUserJoined: " + (uid&0xFFFFFFL));
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            setupRemoteView(uid);
                        }
                    });
                }

            });
        } catch (Exception e) {
            e.printStackTrace();
        }

        mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        mRtcEngine.enableVideo();
        mRtcEngine.setClientRole(Constants.CLIENT_ROLE_AUDIENCE);

        mRtcEngine.joinChannel(null, "ss_test", "", 0);
    }

    private void setupRemoteView(int uid) {
        SurfaceView surfaceV = RtcEngine.CreateRendererView(getApplicationContext());
        surfaceV.setZOrderOnTop(true);
        surfaceV.setZOrderMediaOverlay(true);
        mFlRoot.addView(surfaceV, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        mRtcEngine.setupRemoteVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, uid));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        mRtcEngine.leaveChannel();

        RtcEngine.destroy();
        mRtcEngine = null;
    }

}
