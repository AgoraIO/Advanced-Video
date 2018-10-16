package io.agora.rtc.mediaio.app.shareScreen.ui;

import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.media.projection.MediaProjection;
import android.os.Binder;
import android.os.HandlerThread;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import io.agora.rtc.Constants;
import io.agora.rtc.gl.EglBase;
import io.agora.rtc.mediaio.AgoraSurfaceView;
import io.agora.rtc.mediaio.MediaIO;
import io.agora.rtc.mediaio.app.shareScreen.source.AgoraTextureRecord;
import io.agora.rtc.mediaio.app.shareScreen.source.ViewSharingCapturer;
import io.agora.rtc.mediaio.app.rtcEngine.ConstantApp;
import io.agora.rtc.mediaio.app.rtcEngine.WorkerThread;
import io.agora.rtc.video.VideoCanvas;

public class RecordService extends Service {

    private static final String TAG = RecordService.class.getSimpleName();
    private WorkerThread workerThread;
    private EglBase.Context mSharedContext;
    private MediaProjection mediaProjection;
    private AgoraTextureRecord textureSource;
    private ViewSharingCapturer viewSource;
    private SurfaceView previewSurfaceView;
    private boolean isEnableViewRecord = false;
    private static final String LOG_TAG = "RecordService";
    private boolean running = false;
    private String channelName;
    private int width = 720;
    private int height = 1080;
    private int dpi;
    private View recordView = null;
    private SurfaceReadyListener listener;

    @Override
    public IBinder onBind(Intent intent) {
        return new RecordBinder();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        HandlerThread serviceThread = new HandlerThread("service_thread",
                android.os.Process.THREAD_PRIORITY_BACKGROUND);
        serviceThread.start();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    public void setMediaProject(MediaProjection project) {
        mediaProjection = project;
    }

    public boolean isRunning() {
        return running;
    }

    public void setConfig(int width, int height, int dpi) {
        this.width = width;
        this.height = height;
        this.dpi = dpi;
    }

    public void setRecordView(View view) {

        this.recordView = view;
    }

    public boolean startRecord() {
        initRtcEngine();
        if (isEnableViewRecord) {
            initSurfaceRGBA();
        } else {
            initSurfaceTexture();
        }
        joinChannel();
        running = true;
        return true;
    }

    public boolean stopRecord() {
        if (!running) {
            return false;
        }
        Log.i(TAG, "stopRecord");
        running = false;
        releasTextureSource();
        releaseRGBASource();
        leaveChannel();
        return true;
    }

    public void initRtcEngine() {
        if (this.workerThread.getRtcEngine() != null) {
            this.doConfigEngine(Constants.CLIENT_ROLE_BROADCASTER);
        }
    }

    public void setSurfaceReadyListener(SurfaceReadyListener listener) {
        this.listener = listener;
    }

    public void initSurfaceTexture() {
        releasTextureSource();
        releaseRGBASource();
        this.workerThread.getRtcEngine().stopPreview();
        textureSource = new AgoraTextureRecord(this, this.width, this.height, this.dpi, this.mediaProjection);
        mSharedContext = textureSource.getEglContext();

        AgoraSurfaceView render = new AgoraSurfaceView(this);
        render.setZOrderOnTop(true);
        render.setZOrderMediaOverlay(true);
        render.init(mSharedContext);
        render.setBufferType(MediaIO.BufferType.TEXTURE);
        render.setPixelFormat(MediaIO.PixelFormat.TEXTURE_OES);

        this.workerThread.getRtcEngine().setLocalVideoRenderer(render);
        previewSurfaceView = render;
        listener.surfaceIsReady(previewSurfaceView);
        this.workerThread.getRtcEngine().setVideoSource(textureSource);
        this.workerThread.getRtcEngine().startPreview();
    }

    public void initSurfaceRGBA() {

        this.workerThread.getRtcEngine().stopPreview();
        releasTextureSource();
        releaseRGBASource();
        viewSource = new ViewSharingCapturer(this.recordView);

        AgoraSurfaceView render = new AgoraSurfaceView(this);
        render.setZOrderOnTop(true);
        render.setZOrderMediaOverlay(true);
        render.init(null);
        render.setBufferType(MediaIO.BufferType.BYTE_BUFFER);
        render.setPixelFormat(MediaIO.PixelFormat.RGBA);
        this.workerThread.getRtcEngine().setLocalRenderMode(VideoCanvas.RENDER_MODE_FIT);
        this.workerThread.getRtcEngine().setLocalVideoRenderer(render);
        previewSurfaceView = render;
        listener.surfaceIsReady(previewSurfaceView);
        this.workerThread.getRtcEngine().setVideoSource(viewSource);
        this.workerThread.getRtcEngine().startPreview();

    }

    public void releasTextureSource() {
        if (textureSource != null) {
            textureSource.sourceRelease();
            textureSource = null;
        }
    }

    public void releaseRGBASource() {
        if (viewSource != null) {
            viewSource = null;
        }

    }

    public void setChannelName(String channelName) {
        this.channelName = channelName;
    }

    public void joinChannel() {
        this.workerThread.joinChannel(channelName, 0);
    }

    public void leaveChannel() {
        this.workerThread.getRtcEngine().stopPreview();
        this.workerThread.getRtcEngine().leaveChannel();
    }

    public void setWorkerThread(WorkerThread workerThread) {
        this.workerThread = workerThread;
    }

    public class RecordBinder extends Binder {
        public RecordService getRecordService() {
            return RecordService.this;
        }
    }


    public boolean isEnableViewRecord() {
        return isEnableViewRecord;
    }

    public void setEnableViewRecord(boolean enableViewRecord) {
        isEnableViewRecord = enableViewRecord;
        if (running) {
            if (isEnableViewRecord) {
                initSurfaceRGBA();
            } else {
                initSurfaceTexture();
            }
        }

    }

    private void doConfigEngine(int cRole) {

        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        int prefIndex = pref.getInt(ConstantApp.PrefManager.PREF_PROPERTY_PROFILE_IDX, ConstantApp.DEFAULT_PROFILE_IDX);
        if (prefIndex > ConstantApp.VIDEO_PROFILES.length - 1) {
            prefIndex = ConstantApp.DEFAULT_PROFILE_IDX;
        }
        int vProfile = Constants.VIDEO_PROFILE_360P;
        this.workerThread.configEngine(cRole, vProfile, true);

    }
}