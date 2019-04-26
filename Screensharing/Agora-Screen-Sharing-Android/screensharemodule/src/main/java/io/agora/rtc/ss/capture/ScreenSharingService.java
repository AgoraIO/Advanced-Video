package io.agora.rtc.ss.capture;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.WindowManager;
import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.ss.Constant;
import io.agora.rtc.ss.aidl.IErrorNotification;
import io.agora.rtc.ss.aidl.IScreenShare;
import io.agora.rtc.ss.gles.GLRender;
import io.agora.rtc.ss.gles.ImgTexFrame;
import io.agora.rtc.ss.gles.SinkConnector;
import io.agora.rtc.video.AgoraVideoFrame;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class ScreenSharingService extends Service {

    private static final String LOG_TAG = ScreenSharingService.class.getSimpleName();

    private ScreenCapture mScreenCapture;
    private GLRender mScreenGLRender;
    private RtcEngine mRtcEngine;
    private boolean mIsLandSpace = false;
    private Context mContext;
    private ScreenCaptureSource mSCS;

    private RemoteCallbackList<IErrorNotification> mCallbacks
            = new RemoteCallbackList<IErrorNotification>();

    private final IScreenShare.Stub mBinder = new IScreenShare.Stub() {
        public void registerCallback(IErrorNotification cb) {
            if (cb != null) mCallbacks.register(cb);
        }

        public void unregisterCallback(IErrorNotification cb) {
            if (cb != null) mCallbacks.unregister(cb);
        }

        public void startShare() {
            startCapture();
        }

        public void stopShare() {
            stopCapture();
        }
    };

    private void initModules() {
        WindowManager wm = (WindowManager) getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
        DisplayMetrics metrics = new DisplayMetrics();
        wm.getDefaultDisplay().getMetrics(metrics);

        if (mScreenGLRender == null) {
            mScreenGLRender = new GLRender();
        }
        if (mScreenCapture == null) {
            mScreenCapture = new ScreenCapture(mContext, mScreenGLRender, metrics.densityDpi);
        }

        mScreenCapture.mImgTexSrcConnector.connect(new SinkConnector<ImgTexFrame>() {
            @Override
            public void onFormatChanged(Object obj) {
                Log.d(LOG_TAG, "onFormatChanged " + obj.toString());
            }

            @Override
            public void onFrameAvailable(ImgTexFrame frame) {
                Log.d(LOG_TAG, "onFrameAvailable " + frame.toString());

                if (mRtcEngine == null) {
                    return;
                }

                mSCS.getConsumer().consumeTextureFrame(frame.mTextureId, AgoraVideoFrame.FORMAT_TEXTURE_OES, frame.mFormat.mWidth,
                        frame.mFormat.mHeight, 0, frame.pts, frame.mTexMatrix);
            }
        });

        mScreenCapture.setOnScreenCaptureListener(new ScreenCapture.OnScreenCaptureListener() {
            @Override
            public void onStarted() {
                Log.d(LOG_TAG, "Screen Record Started");
            }

            @Override
            public void onError(int err) {
                Log.d(LOG_TAG, "onError " + err);
                switch (err) {
                    case ScreenCapture.SCREEN_ERROR_SYSTEM_UNSUPPORTED:
                        break;
                    case ScreenCapture.SCREEN_ERROR_PERMISSION_DENIED:
                        break;
                }
            }
        });

        int screenWidth = wm.getDefaultDisplay().getWidth();
        int screenHeight = wm.getDefaultDisplay().getHeight();
        if ((mIsLandSpace && screenWidth < screenHeight) ||
                (!mIsLandSpace) && screenWidth > screenHeight) {
            screenWidth = wm.getDefaultDisplay().getHeight();
            screenHeight = wm.getDefaultDisplay().getWidth();
        }

        setOffscreenPreview(screenWidth, screenHeight);
    }

    private void deInitModules() {
        mRtcEngine.leaveChannel();
        RtcEngine.destroy();
        mRtcEngine = null;

        if (mScreenCapture != null) {
            mScreenCapture.release();
            mScreenCapture = null;
        }

        if (mScreenGLRender != null) {
            mScreenGLRender.quit();
            mScreenGLRender = null;
        }
    }

    /**
     * Set offscreen preview.
     *
     * @param width  offscreen width
     * @param height offscreen height
     * @throws IllegalArgumentException
     */
    public void setOffscreenPreview(int width, int height) throws IllegalArgumentException {
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("Invalid offscreen resolution");
        }

        mScreenGLRender.init(width, height);
    }

    private void startCapture() {
        mScreenCapture.start();
    }

    private void stopCapture() {
        mScreenCapture.stop();
    }

    @Override
    public void onCreate() {
        mContext = getApplicationContext();
        initModules();
    }

    @Override
    public IBinder onBind(Intent intent) {
        setUpEngine(intent);
        setUpVideoConfig(intent);
        joinChannel(intent);
        return mBinder;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        deInitModules();
    }

    private void joinChannel(Intent intent) {
        mRtcEngine.joinChannel(null, intent.getStringExtra(Constant.CHANNEL_NAME),
                "Extra data info", intent.getIntExtra(Constant.UID, 0));
    }

    private void setUpEngine(Intent intent) {
        String appId = intent.getStringExtra(Constant.APP_ID);
        try {
            mRtcEngine = RtcEngine.create(getApplicationContext(), appId, new IRtcEngineEventHandler() {
                @Override
                public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
                    Log.d(LOG_TAG, "onJoinChannelSuccess " + channel + " " + elapsed);
                }

                @Override
                public void onWarning(int warn) {
                    Log.d(LOG_TAG, "onWarning " + warn);
                }

                @Override
                public void onError(int err) {
                    Log.d(LOG_TAG, "onError " + err);

                    final int N = mCallbacks.beginBroadcast();
                    for (int i=0; i<N; i++) {
                        try {
                            mCallbacks.getBroadcastItem(i).onError(err);
                        } catch (RemoteException e) {
                            // The RemoteCallbackList will take care of removing
                            // the dead object for us.
                        }
                    }
                    mCallbacks.finishBroadcast();
                }
            });
        } catch (Exception e) {
            Log.e(LOG_TAG, Log.getStackTraceString(e));

            throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
        }

        mRtcEngine.setLogFile("/sdcard/sdklog.txt");
        mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        mRtcEngine.enableVideo();

        if (mRtcEngine.isTextureEncodeSupported()) {
            mSCS = new ScreenCaptureSource();
            mRtcEngine.setVideoSource(mSCS);
        } else {
            throw new RuntimeException("Can not work on device do not supporting texture" + mRtcEngine.isTextureEncodeSupported());
        }

        mRtcEngine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);

        mRtcEngine.muteAllRemoteAudioStreams(true);
        mRtcEngine.muteAllRemoteVideoStreams(true);
    }

    private void setUpVideoConfig(Intent intent) {
        int width = intent.getIntExtra(Constant.WIDTH, 0);
        int height = intent.getIntExtra(Constant.HEIGHT, 0);
        int frameRate = intent.getIntExtra(Constant.FRAME_RATE, 15);
        int bitRate = intent.getIntExtra(Constant.BIT_RATE, 0);
        int orientationMode = intent.getIntExtra(Constant.ORIENTATION_MODE, 0);
        VideoEncoderConfiguration.FRAME_RATE fr;
        VideoEncoderConfiguration.ORIENTATION_MODE om;

        switch (frameRate) {
            case 1 :
                fr = VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_1;
                break;
            case 7 :
                fr = VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_7;
                break;
            case 10 :
                fr = VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_10;
                break;
            case 15 :
                fr = VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
                break;
            case 24 :
                fr = VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_24;
                break;
            case 30 :
                fr = VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_30;
                break;
            default :
                fr = VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
                break;
        }

        switch (orientationMode) {
            case 0 :
                om = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
                break;
            case 1 :
                om = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_LANDSCAPE;
                break;
            case 2 :
                om = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT;
                break;
            default :
                om = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
                break;
        }

        mRtcEngine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                new VideoEncoderConfiguration.VideoDimensions(width, height), fr, bitRate, om));
    }
}
