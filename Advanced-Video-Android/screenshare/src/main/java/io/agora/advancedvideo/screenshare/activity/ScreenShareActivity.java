package io.agora.advancedvideo.screenshare.activity;

import android.content.Context;
import android.content.Intent;
import android.graphics.SurfaceTexture;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.widget.RelativeLayout;

import androidx.annotation.Nullable;

import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.advancedvideo.gles.core.EglCore;
import io.agora.advancedvideo.gles.core.GlUtil;
import io.agora.advancedvideo.screenshare.R;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.MediaIO;
import io.agora.rtc.video.VideoCanvas;

public class ScreenShareActivity extends BaseLiveActivity implements SurfaceTexture.OnFrameAvailableListener  {
    private static final String TAG = ScreenShareActivity.class.getSimpleName();
    private static final String SHARE_THREAD_NAME = "screen-share";
    private static final String VIRTUAL_DISPLAY_NAME = "screen-share-display";
    private static final int PROJECTION_REQ_CODE = 1;
    private static final int CAPTURE_FRAME_RATE = 15;

    private ScreenShareTextureSource mTextureSource;
    private int mTextureId;
    private SurfaceTexture mSurfaceTexture;
    private float[] mTransform = new float[16];
    private Surface mDrawingSurface;

    private MediaProjectionManager mProjectionManager;
    private MediaProjection mMediaProjection;
    private VirtualDisplay mVirtualDisplay;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private int mScreenShareDpi;

    private ScreenShareThread mRenderThread;
    private Handler mRenderHandler;
    private CaptureRunnable mCaptureRunnable = new CaptureRunnable();

    private volatile IVideoFrameConsumer mVideoFrameSender;

    private View mVideoViewLayout;
    private boolean mChannelJoined;

    @Override
    protected void onInitializeVideo() {
        initView(mVideoContainer);
        getScreenProperties();

        if (mIsBroadcaster) {
            mProjectionManager = (MediaProjectionManager)
                    getSystemService(Context.MEDIA_PROJECTION_SERVICE);
            mTextureSource = new ScreenShareTextureSource();
            rtcEngine().setVideoSource(mTextureSource);
        }

        joinChannel(config().getChannelName());
    }

    private void initView(RelativeLayout container) {
        LayoutInflater inflater = LayoutInflater.from(this);
        mVideoViewLayout = inflater.inflate(
                R.layout.share_video_layout, container, false);
        container.addView(mVideoViewLayout,
                RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);

    }

    private void getScreenProperties() {
        DisplayMetrics metric = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metric);
        mScreenShareDpi = metric.densityDpi;
        mSurfaceWidth = metric.widthPixels;
        mSurfaceHeight = metric.heightPixels;
    }

    private void joinChannel(String channel) {
        rtcEngine().joinChannel(null, channel, null, 0);
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        Log.i(TAG, "onJoinSuccess channel:" + channel + " uid:" + (uid & 0xFFFFFFFFL));
        mChannelJoined = true;
        if (mIsBroadcaster) {
            requestMediaProjection();
        } else {
            // Audience will initialize remote video preview
            // when they first receive a decoded remote
            // video frame
        }
    }

    private void requestMediaProjection() {
        Intent intent = mProjectionManager.createScreenCaptureIntent();
        startActivityForResult(intent, PROJECTION_REQ_CODE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PROJECTION_REQ_CODE && resultCode == RESULT_OK) {
            mMediaProjection = mProjectionManager.getMediaProjection(resultCode, data);
            initVirtualDisplay();
        }
    }

    private void initVirtualDisplay() {
        mRenderThread = new ScreenShareThread(SHARE_THREAD_NAME);
        mRenderThread.start();
        mRenderHandler = new Handler(mRenderThread.getLooper());

        mRenderHandler.post(new Runnable() {
            @Override
            public void run() {
                mSurfaceTexture.setOnFrameAvailableListener(ScreenShareActivity.this, mRenderHandler);
                mVirtualDisplay = mMediaProjection.createVirtualDisplay(
                        VIRTUAL_DISPLAY_NAME, mSurfaceWidth, mSurfaceHeight, mScreenShareDpi,
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC, mDrawingSurface,
                        null, null);
            }
        });
    }

    @Override
    public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {
        Log.i(TAG, "onFirstRemoteVideoDecoded: uid " + (uid & 0xFFFFFFFFL));

        if (mIsBroadcaster) {
            return;
        }

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SurfaceView surfaceView = RtcEngine.
                        CreateRendererView(ScreenShareActivity.this);
                rtcEngine().setupRemoteVideo(new VideoCanvas(
                        surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
                ((RelativeLayout)mVideoViewLayout).addView(surfaceView);
            }
        });

    }

    @Override
    public void onUserJoined(int uid, int elapsed) {
        Log.i(TAG, "onUserJoined: uid " + (uid & 0xFFFFFFFFL));
    }

    @Override
    public void onUserOffline(int uid, int reason) {

    }

    @Override
    protected void onLeaveButtonClicked(View view) {

    }

    @Override
    public void finish() {
        if (mChannelJoined) {
            stopShare();
        }
        leaveChannel();
        super.finish();
    }

    public void stopShare() {
        if (mVirtualDisplay != null) {
            mVirtualDisplay.release();
        }

        if (mMediaProjection != null) {
            mMediaProjection.stop();
        }

        if (mRenderThread != null && mRenderThread.isAlive()) {
            mRenderThread.quit();
            mRenderHandler = null;
        }
        mChannelJoined = false;
    }

    private void leaveChannel() {
        rtcEngine().leaveChannel();
    }

    @Override
    protected void onSwitchCameraButtonClicked(View view) {
        // Screen sharing will ignore this event
    }

    @Override
    protected void onMuteAudioButtonClicked(View view) {
        rtcEngine().muteLocalAudioStream(!view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    protected void onMuteVideoButtonClicked(View view) {
        rtcEngine().muteLocalVideoStream(!view.isActivated());
        view.setActivated(!view.isActivated());
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        // A new image frame becomes available to the surface texture
        Log.i(TAG, "onFrameAvailable:");

        if (mVideoFrameSender != null) {
            mVideoFrameSender.consumeTextureFrame(mTextureId,
                    MediaIO.PixelFormat.TEXTURE_OES.intValue(),
                    mSurfaceWidth, mSurfaceHeight, 0,
                    System.currentTimeMillis(), mTransform);
        }

        mRenderHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                try {
                    mSurfaceTexture.updateTexImage();
                    mSurfaceTexture.getTransformMatrix(mTransform);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }, 50);
    }

    private class CaptureRunnable implements Runnable {
        private int mFrameRate;
        private int mInterval;

        @Override
        public void run() {
            if (!inShareThread()) {
                return;
            }

            if (mFrameRate != CAPTURE_FRAME_RATE) {
                mInterval = 1000 / CAPTURE_FRAME_RATE;
                mFrameRate = CAPTURE_FRAME_RATE;
            }

            // This callback method is invoked in the
            // OpenGL thread, and the texture is
            // created when the thread starts running.
            try {
                mSurfaceTexture.updateTexImage();
                mSurfaceTexture.getTransformMatrix(mTransform);
            } catch (Exception e) {
                e.printStackTrace();
                return;
            }

            if (mVideoFrameSender != null) {
                Log.i(TAG, "onFrameCapture:" + Thread.currentThread().getName());
                mVideoFrameSender.consumeTextureFrame(mTextureId,
                        MediaIO.PixelFormat.TEXTURE_OES.intValue(),
                        mSurfaceWidth, mSurfaceHeight, 0,
                        System.currentTimeMillis(), mTransform);
            }

            if (mRenderHandler != null) {
                mRenderHandler.postDelayed(mCaptureRunnable, mInterval);
            }
        }

        private boolean inShareThread() {
            return Thread.currentThread().getName().equals(SHARE_THREAD_NAME);
        }
    }

    private class ScreenShareTextureSource implements IVideoSource {
        @Override
        public boolean onInitialize(final IVideoFrameConsumer consumer) {
            Log.i(TAG, "ScreenShareTexture onInitialize");
            mVideoFrameSender = consumer;
            return true;
        }

        @Override
        public boolean onStart() {
            Log.i(TAG, "ScreenShareTexture onStart");
            return true;
        }

        @Override
        public void onStop() {
            Log.i(TAG, "ScreenShareTexture onStop");
        }

        @Override
        public void onDispose() {
            Log.i(TAG, "ScreenShareTexture onDispose");
            mVideoFrameSender = null;
        }

        @Override
        public int getBufferType() {
            return MediaIO.BufferType.TEXTURE.intValue();
        }
    }

    private class ScreenShareThread extends HandlerThread {
        private EglCore mEglCore;
        private EGLSurface mEglSurface;

        ScreenShareThread(String name) {
            super(name);
        }

        @Override
        public void run() {
            initOpenGLES();
            super.run();
            releaseOpenGLES();
        }

        private void initOpenGLES() {
            mEglCore = new EglCore();
            mEglSurface = mEglCore.createOffscreenSurface(1, 1);
            mEglCore.makeCurrent(mEglSurface);
            mTextureId = GlUtil.createTextureObject(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
            mSurfaceTexture = new SurfaceTexture(mTextureId);
            mSurfaceTexture.setDefaultBufferSize(mSurfaceWidth, mSurfaceHeight);
            mDrawingSurface = new Surface(mSurfaceTexture);
        }

        private void releaseOpenGLES() {
            mDrawingSurface.release();
            mEglCore.makeNothingCurrent();
            mEglCore.releaseSurface(mEglSurface);
            mSurfaceTexture.release();
            GlUtil.deleteTextureObject(mTextureId);
            mTextureId = 0;
            mEglCore.release();
        }
    }
}
