package io.agora.advancedvideo.screenshare;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.SurfaceTexture;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.util.Log;
import android.view.Surface;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import io.agora.advancedvideo.AgoraApplication;
import io.agora.advancedvideo.gles.core.EglCore;
import io.agora.advancedvideo.gles.core.GlUtil;
import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.MediaIO;

public class ScreenShareService extends Service implements SurfaceTexture.OnFrameAvailableListener {
    public static final String FLAG_SCREEN_WIDTH = "screen-width";
    public static final String FLAG_SCREEN_HEIGHT = "screen-height";
    public static final String FLAG_SCREEN_DPI = "screen-dpi";
    public static final String FLAG_FRAME_RATE = "screen-frame-rate";

    private static final String TAG = ScreenShareService.class.getSimpleName();
    private static final String SHARE_THREAD_NAME = "screen-share";
    private static final String VIRTUAL_DISPLAY_NAME = "screen-share-display";

    private static final String CHANNEL_ID = "ScreenShareServiceChannel";
    private static final int NOTIFICATION_ID = 1;

    private static final int DEFAULT_SCREEN_WIDTH = 640;
    private static final int DEFAULT_SCREEN_HEIGHT = 480;
    private static final int DEFAULT_SCREEN_DPI = 3;
    private static final int DEFAULT_FRAME_RATE = 15;

    private int mTextureId;
    private SurfaceTexture mSurfaceTexture;
    private Surface mSurface;
    private float[] mTransform = new float[16];

    private MediaProjection mMediaProjection;
    private VirtualDisplay mVirtualDisplay;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private int mScreenShareDpi;
    private int mFrameInterval;

    private ScreenShareThread mRenderThread;
    private Handler mRenderHandler;
    private IVideoFrameConsumer mVideoFrameSender;

    private AgoraApplication application() {
        return (AgoraApplication) getApplication();
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        getScreenParams(intent);
        setRtcVideoSource();
        startForeground();
        startShareThread();
        startShare(intent);
        return START_NOT_STICKY;
    }

    private void setRtcVideoSource() {
        application().rtcEngine().setVideoSource(new ScreenShareTextureSource());
    }

    private void getScreenParams(Intent intent) {
        mSurfaceWidth = intent.getIntExtra(FLAG_SCREEN_WIDTH, DEFAULT_SCREEN_WIDTH);
        mSurfaceHeight = intent.getIntExtra(FLAG_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT);
        mScreenShareDpi = intent.getIntExtra(FLAG_SCREEN_DPI, DEFAULT_SCREEN_DPI);
        int frameRate = intent.getIntExtra(FLAG_FRAME_RATE, DEFAULT_FRAME_RATE);
        mFrameInterval = 1000 / frameRate;
    }

    private void startForeground() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(
                    CHANNEL_ID, "ScreenShare Service Channel",
                    NotificationManager.IMPORTANCE_DEFAULT
            );

            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(serviceChannel);
        }

        Intent notificationIntent = new Intent(this, ScreenShareActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this,
                0, notificationIntent, 0);

        Notification notification = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("Screen Share Service")
                .setContentText("You are sharing the screen with friends")
                .setContentIntent(pendingIntent)
                .build();

        startForeground(NOTIFICATION_ID, notification);
    }

    private void startShareThread() {
        mRenderThread = new ScreenShareThread(SHARE_THREAD_NAME);
        mRenderThread.start();
        mRenderHandler = new Handler(mRenderThread.getLooper());
    }

    private void startShare(Intent intent) {
        mSurfaceTexture.setOnFrameAvailableListener(this, mRenderHandler);
        MediaProjectionManager pm = (MediaProjectionManager)
                getSystemService(Context.MEDIA_PROJECTION_SERVICE);
        mMediaProjection = pm.getMediaProjection(Activity.RESULT_OK, intent);
        mVirtualDisplay = mMediaProjection.createVirtualDisplay(
                VIRTUAL_DISPLAY_NAME, mSurfaceWidth, mSurfaceHeight, mScreenShareDpi,
                DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC, mSurface,
                null, null);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (mVirtualDisplay != null) {
            mVirtualDisplay.release();
        }

        if (mMediaProjection != null) {
            mMediaProjection.stop();
        }

        if (mRenderHandler != null) {
            mRenderHandler = null;
        }

        if (mRenderThread != null && mRenderThread.isAlive()) {
            mRenderThread.quit();
            mRenderThread = null;
        }
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        // A new image frame becomes available to the surface texture
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
        }, mFrameInterval);
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
            mSurface = new Surface(mSurfaceTexture);
        }

        private void releaseOpenGLES() {
            mSurface.release();
            mEglCore.makeNothingCurrent();
            mEglCore.releaseSurface(mEglSurface);
            mSurfaceTexture.release();
            GlUtil.deleteTextureObject(mTextureId);
            mTextureId = 0;
            mEglCore.release();
        }
    }
}
