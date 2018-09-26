package io.agora.rtc.mediaio.app.videoSource.ui;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Matrix;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.SeekBar;

import java.util.HashMap;
import java.util.Map;

import io.agora.rtc.Constants;
import io.agora.rtc.gl.EglBase;
import io.agora.rtc.mediaio.AgoraTextureCamera;
import io.agora.rtc.mediaio.IVideoSink;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.app.BaseActivity;
import io.agora.rtc.mediaio.app.R;
import io.agora.rtc.mediaio.app.videoSource.source.AgoraLocalVideoSource;
import io.agora.rtc.mediaio.app.videoSource.source.PrivateTextureHelper;
import io.agora.rtc.mediaio.app.rtcEngine.AGEventHandler;
import io.agora.rtc.mediaio.app.rtcEngine.ConstantApp;

import static io.agora.rtc.mediaio.MediaIO.BufferType.BYTE_ARRAY;
import static io.agora.rtc.mediaio.MediaIO.BufferType.TEXTURE;
import static io.agora.rtc.mediaio.MediaIO.PixelFormat.I420;
import static io.agora.rtc.mediaio.MediaIO.PixelFormat.TEXTURE_OES;

public class PrivateTextureViewActivity extends BaseActivity implements AGEventHandler {
    public final static String TAG = "PrivateTextureView";
    private Map<Integer, Boolean> mUsers;
    private String mChannelName;
    private String videoPath;
    private int mClientRole;

    private TextureView mLocalTextureView;
    private IVideoSource mVideoSource;
    private IVideoSink mRender;

    private TextureView mRemoteTextureView;
    private IVideoSink mRemoteRender;

    private SeekBar mAlphaSeekBar;
    private SeekBar mRotationSeekBar;
    private SeekBar mZoomSeekBar;

    private boolean mLocalSourceFlag = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_private_texture_view);
        mUsers = new HashMap<>();
        mRemoteTextureView = (TextureView) findViewById(R.id.textureView2);
        ;
        mRemoteRender = new PrivateTextureHelper(this, mRemoteTextureView);

        SeekBarCallBack seekBarCallBack = new SeekBarCallBack();
        mAlphaSeekBar = (SeekBar) findViewById(R.id.seekBar);
        mAlphaSeekBar.setOnSeekBarChangeListener(seekBarCallBack);
        mRotationSeekBar = (SeekBar) findViewById(R.id.seekBar2);
        mRotationSeekBar.setOnSeekBarChangeListener(seekBarCallBack);
        mZoomSeekBar = (SeekBar) findViewById(R.id.seekBar3);
        mZoomSeekBar.setOnSeekBarChangeListener(seekBarCallBack);
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        worker().leaveChannel(mChannelName);
        worker().preview(false, null, 0);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    protected void initUIandEvent() {
        Log.i(TAG, "initUIandEvent");
        event().addEventHandler(this);
        Intent i = getIntent();
        mChannelName = i.getStringExtra(ConstantApp.ACTION_KEY_ROOM_NAME);
        videoPath = i.getStringExtra(ConstantApp.ACTION_KEY_VIDEO_PATH);
        mClientRole = i.getIntExtra(ConstantApp.ACTION_KEY_CROLE, Constants.CLIENT_ROLE_BROADCASTER);
        mVideoSource = new AgoraLocalVideoSource(this, 640, 480, videoPath);
        FrameLayout container = (FrameLayout) findViewById(R.id.texture_view_container);
        if (container.getChildCount() >= 1) {
            return;
        }
        mLocalTextureView = new TextureView(this);
        container.addView(mLocalTextureView);
        mRender = new PrivateTextureHelper(this, mLocalTextureView);
        ((PrivateTextureHelper) mRender).init(((AgoraLocalVideoSource) mVideoSource).getEglContext());
        ((PrivateTextureHelper) mRender).setBufferType(TEXTURE);
        ((PrivateTextureHelper) mRender).setPixelFormat(TEXTURE_OES);
        worker().setVideoSource(mVideoSource);

        worker().setLocalRender(mRender);
        worker().preview(true, null, 0);
        doConfigEngine(mClientRole);

        worker().joinChannel(mChannelName, 0);
    }

    protected void deInitUIandEvent() {
        Log.i(TAG, "deInitUIandEvent()");
        worker().leaveChannel(mChannelName);
        if (mClientRole == Constants.CLIENT_ROLE_BROADCASTER) {
            worker().preview(false, null, 0);
        }
        event().removeEventHandler(this);
        mUsers.clear();
    }


    public void onFirstRemoteVideoDecoded(final int uid, final int width, final int height, final int elapsed) {
        Log.d(TAG, "onFirstRemoteVideoDecoded");
        if (uid != config().mUid) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    addNewUser(uid, width, height);
                }
            });
        }
    }

    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        Log.d(TAG, "onJoinChannelSuccess");
    }

    public void onUserOffline(int uid, int reason) {
        Log.d(TAG, "onUserOffline");
    }
    //from AGEventHandler end

    private void doConfigEngine(int cRole) {
        Log.d(TAG, "doConfigEngine");
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        int prefIndex = pref.getInt(ConstantApp.PrefManager.PREF_PROPERTY_PROFILE_IDX, ConstantApp.DEFAULT_PROFILE_IDX);
        if (prefIndex > ConstantApp.VIDEO_PROFILES.length - 1) {
            prefIndex = ConstantApp.DEFAULT_PROFILE_IDX;
        }
        int vProfile = ConstantApp.VIDEO_PROFILES[prefIndex];

        worker().configEngine(cRole, vProfile, false);
    }

    private void addNewUser(int uid, int width, int height) {
        Log.d(TAG, "addNewUser");
        if (mUsers.containsKey(uid)) return;

        mUsers.put(uid, true);

        ((PrivateTextureHelper) mRemoteRender).init(null);
        ((PrivateTextureHelper) mRemoteRender).setBufferType(BYTE_ARRAY);
        ((PrivateTextureHelper) mRemoteRender).setPixelFormat(I420);
        worker().addRemoteRender(uid, mRemoteRender);
    }


    public class SeekBarCallBack implements SeekBar.OnSeekBarChangeListener {
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (seekBar == mAlphaSeekBar) {
                float alpha = progress / 100.0f;
                mLocalTextureView.setAlpha(1 - alpha);
            }

            if (seekBar == mRotationSeekBar) {
                float rotation = progress * 1.0f;
                mRemoteTextureView.setRotation(rotation);
            }

            if (seekBar == mZoomSeekBar) {
                float zoom = 1.0f + progress / 100.0f;
                Matrix matrix = new Matrix();
                matrix.postScale(zoom, zoom);
                mRemoteTextureView.setTransform(matrix);
            }
        }

        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        public void onStopTrackingTouch(SeekBar seekBar) {
        }
    }


    public void switchSource(View view) {
        worker().preview(false, null, 0);

        FrameLayout container = (FrameLayout) findViewById(R.id.texture_view_container);
        container.removeAllViews();
        if (container.getChildCount() >= 1) {
            return;
        }
        mLocalTextureView = new TextureView(this);
        WindowManager wm = (WindowManager) this.getSystemService(Context.WINDOW_SERVICE);
        EglBase.Context sharedContext;
        if (mLocalSourceFlag) {
            Log.i(TAG, "switch to camera source");
            mVideoSource = new AgoraTextureCamera(this, 640, 480);
            sharedContext = ((AgoraTextureCamera) mVideoSource).getEglContext();
            container.getLayoutParams().width = wm.getDefaultDisplay().getWidth() * 2 / 5;
        } else {
            Log.i(TAG, "switch to local video source");
            mVideoSource = new AgoraLocalVideoSource(this, 640, 480, videoPath);
            sharedContext = ((AgoraLocalVideoSource) mVideoSource).getEglContext();
            container.getLayoutParams().width = wm.getDefaultDisplay().getWidth();
        }
        mRender = new PrivateTextureHelper(this, mLocalTextureView);
        ((PrivateTextureHelper) mRender).init(sharedContext);
        ((PrivateTextureHelper) mRender).setBufferType(TEXTURE);
        ((PrivateTextureHelper) mRender).setPixelFormat(TEXTURE_OES);
        worker().setVideoSource(mVideoSource);
        worker().setLocalRender(mRender);
        worker().preview(true, null, 0);
        mLocalSourceFlag = !mLocalSourceFlag;
        container.addView(mLocalTextureView);
    }
}
