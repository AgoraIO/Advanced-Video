package com.agora.arcore;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.design.widget.BaseTransientBottomBar;
import android.support.design.widget.Snackbar;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import com.agora.arcore.rendering.BackgroundRenderer;
import com.agora.arcore.rendering.ObjectRenderer;
import com.agora.arcore.rendering.PeerRenderer;
import com.agora.arcore.rendering.PlaneRenderer;
import com.agora.arcore.rendering.PointCloudRenderer;
import com.google.ar.core.Anchor;
import com.google.ar.core.ArCoreApk;
import com.google.ar.core.Camera;
import com.google.ar.core.Config;
import com.google.ar.core.Frame;
import com.google.ar.core.HitResult;
import com.google.ar.core.Plane;
import com.google.ar.core.Point;
import com.google.ar.core.PointCloud;
import com.google.ar.core.Session;
import com.google.ar.core.Trackable;
import com.google.ar.core.TrackingState;
import com.google.ar.core.exceptions.UnavailableApkTooOldException;
import com.google.ar.core.exceptions.UnavailableArcoreNotInstalledException;
import com.google.ar.core.exceptions.UnavailableSdkTooOldException;
import com.google.ar.core.exceptions.UnavailableUserDeclinedInstallationException;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.mediaio.MediaIO;

/**
 * Created by wyylling@gmail.com on 03/01/2018.
 */

public class AgoraARCoreActivity extends AppCompatActivity implements GLSurfaceView.Renderer {
    private static final String TAG = AgoraARCoreActivity.class.getSimpleName();

    // Rendering. The Renderers are created here, and initialized when the GL surface is created.
    private GLSurfaceView mSurfaceView;

    private boolean installRequested;

    private Session mSession;
    private GestureDetector mGestureDetector;
    private Snackbar mMessageSnackbar;
    private DisplayRotationHelper mDisplayRotationHelper;

    private final BackgroundRenderer mBackgroundRenderer = new BackgroundRenderer();
    private final ObjectRenderer mVirtualObject = new ObjectRenderer();
    private final ObjectRenderer mVirtualObjectShadow = new ObjectRenderer();
    private final PlaneRenderer mPlaneRenderer = new PlaneRenderer();
    private final PointCloudRenderer mPointCloud = new PointCloudRenderer();

    // Temporary matrix allocated here to reduce number of allocations for each frame.
    private final float[] mAnchorMatrix = new float[16];

    // Tap handling and UI.
    private final ArrayBlockingQueue<MotionEvent> queuedSingleTaps = new ArrayBlockingQueue<>(16);
    private final ArrayList<Anchor> anchors = new ArrayList<>();

    private RtcEngine mRtcEngine;
    private Handler mSenderHandler;
    private final static int SEND_AR_VIEW = 1;
    private AgoraVideoSource mSource;
    private AgoraVideoRender mRender;
    private ByteBuffer mSendBuffer;

    private boolean mIsARMode;
    private boolean mHidePoint;
    private boolean mHidePlane;
    private float mScaleFactor = 1.0f;

    private IRtcEngineEventHandler mRtcEventHandler;

    private PeerRenderer mPeerObject = new PeerRenderer();

    private List<AgoraVideoRender> mRemoteRenders = new ArrayList<>(20);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mSurfaceView = findViewById(R.id.surfaceview);
        mDisplayRotationHelper = new DisplayRotationHelper(/*context=*/ this);

        // Set up tap listener.
        mGestureDetector =
                new GestureDetector(
                        this,
                        new GestureDetector.SimpleOnGestureListener() {
                            @Override
                            public boolean onSingleTapUp(MotionEvent e) {
                                onSingleTap(e);
                                return true;
                            }

                            @Override
                            public boolean onDown(MotionEvent e) {
                                return true;
                            }
                        });

        mSurfaceView.setOnTouchListener(
                new View.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        return mGestureDetector.onTouchEvent(event);
                    }
                });

        // Set up renderer.
        mSurfaceView.setPreserveEGLContextOnPause(true);
        mSurfaceView.setEGLContextClientVersion(2);
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        mSurfaceView.setRenderer(this);
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        installRequested = false;
        initRtcEngine();
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (mSession == null) {
            Exception exception = null;
            String message = null;
            try {
                switch (ArCoreApk.getInstance().requestInstall(this, !installRequested)) {
                    case INSTALL_REQUESTED:
                        installRequested = true;
                        return;
                    case INSTALLED:
                        break;
                }

                // ARCore requires camera permissions to operate. If we did not yet obtain runtime
                // permission on Android M and above, now is a good time to ask the user for it.
                if (!CameraPermissionHelper.hasCameraPermission(this)) {
                    CameraPermissionHelper.requestCameraPermission(this);
                    return;
                }

                mSession = new Session(/* context= */ this);
            } catch (UnavailableArcoreNotInstalledException
                    | UnavailableUserDeclinedInstallationException e) {
                message = "Please install ARCore";
                exception = e;
            } catch (UnavailableApkTooOldException e) {
                message = "Please update ARCore";
                exception = e;
            } catch (UnavailableSdkTooOldException e) {
                message = "Please update this app";
                exception = e;
            } catch (Exception e) {
                message = "This device does not support AR";
                exception = e;
            }

            if (message != null) {
                showSnackbarMessage(message, true);
                Log.e(TAG, "Exception creating session", exception);
                return;
            }

            // Create default config and check if supported.
            Config config = new Config(mSession);
            if (!mSession.isSupported(config)) {
                showSnackbarMessage("This device does not support AR", true);
            }
            mSession.configure(config);
        }

        showLoadingMessage();
        // Note that order matters - see the note in onPause(), the reverse applies here.
        mSession.resume();
        mSurfaceView.onResume();
        mDisplayRotationHelper.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        // Note that the order matters - GLSurfaceView is paused first so that it does not try
        // to query the session. If Session is paused before GLSurfaceView, GLSurfaceView may
        // still call mSession.update() and get a SessionPausedException.
        mDisplayRotationHelper.onPause();
        mSurfaceView.onPause();
        if (mSession != null) {
            mSession.pause();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        mSendBuffer = null;
        for (int i = 0; i < mRemoteRenders.size(); ++i) {
            AgoraVideoRender render = mRemoteRenders.get(i);
            //mRtcEngine.setRemoteVideoRenderer(render.getPeer().uid, null);
        }
        mRemoteRenders.clear();
        mSenderHandler.getLooper().quit();

        RtcEngine.destroy();

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        if (!CameraPermissionHelper.hasCameraPermission(this)) {
            Toast.makeText(this,
                "Camera permission is needed to run this application", Toast.LENGTH_LONG).show();
            if (!CameraPermissionHelper.shouldShowRequestPermissionRationale(this)) {
                // Permission denied with checking "Do not ask again".
                CameraPermissionHelper.launchPermissionSettings(this);
            }
            finish();
        }

        switch (requestCode) {
            case PERMISSION_REQ_ID_RECORD_AUDIO: {
                if (results.length > 0 && results[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA);
                } else {
                    finish();
                }
                break;
            }
            case PERMISSION_REQ_ID_CAMERA: {
                if (results.length > 0 && results[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE);
                    //((AGApplication) getApplication()).initWorkerThread();
                } else {
                    finish();
                }
                break;
            }
            case PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE: {
                if (results.length > 0 && results[0] == PackageManager.PERMISSION_GRANTED) {
                } else {
                    finish();
                }
                break;
            }
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            // Standard Android full-screen functionality.
            getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
    }

    private void onSingleTap(MotionEvent e) {
        // Queue tap if there is space. Tap is lost if queue is full.
        queuedSingleTaps.offer(e);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Create the texture and pass it to ARCore session to be filled during update().
        mBackgroundRenderer.createOnGlThread(/*context=*/ this);
        if (mSession != null) {
            mSession.setCameraTextureName(mBackgroundRenderer.getTextureId());
        }

        // Prepare the other rendering objects.
        try {
            mVirtualObject.createOnGlThread(/*context=*/this, "andy.obj", "andy.png");
            mVirtualObject.setMaterialProperties(0.0f, 3.5f, 1.0f, 6.0f);

            mVirtualObjectShadow.createOnGlThread(/*context=*/this,
                "andy_shadow.obj", "andy_shadow.png");
            mVirtualObjectShadow.setBlendMode(ObjectRenderer.BlendMode.Shadow);
            mVirtualObjectShadow.setMaterialProperties(1.0f, 0.0f, 0.0f, 1.0f);
        } catch (IOException e) {
            Log.e(TAG, "Failed to read obj file");
        }
        try {
            mPlaneRenderer.createOnGlThread(/*context=*/this, "trigrid.png");
        } catch (IOException e) {
            Log.e(TAG, "Failed to read plane texture");
        }
        mPointCloud.createOnGlThread(/*context=*/this);

        try {
            mPeerObject.createOnGlThread(this);
        } catch (IOException ex) {
            printLog(ex.toString());
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mDisplayRotationHelper.onSurfaceChanged(width, height);
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Clear screen to notify driver it should not load any pixels from previous frame.
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);

        if (mSession == null) {
            return;
        }
        // Notify ARCore session that the view size changed so that the perspective matrix and
        // the video background can be properly adjusted.
        mDisplayRotationHelper.updateSessionIfNeeded(mSession);

        try {
            // Obtain the current frame from ARSession. When the configuration is set to
            // UpdateMode.BLOCKING (it is by default), this will throttle the rendering to the
            // camera framerate.
            Frame frame = mSession.update();
            Camera camera = frame.getCamera();

            // Handle taps. Handling only one tap per frame, as taps are usually low frequency
            // compared to frame rate.
            MotionEvent tap = queuedSingleTaps.poll();
            if (tap != null && camera.getTrackingState() == TrackingState.TRACKING) {
                for (HitResult hit : frame.hitTest(tap)) {
                    // Check if any plane was hit, and if it was hit inside the plane polygon
                    Trackable trackable = hit.getTrackable();
                    // Creates an anchor if a plane or an oriented point was hit.
                    if ((trackable instanceof Plane && ((Plane) trackable).isPoseInPolygon(hit.getHitPose()))
                            || (trackable instanceof Point
                            && ((Point) trackable).getOrientationMode()
                            == Point.OrientationMode.ESTIMATED_SURFACE_NORMAL)) {
                        // Hits are sorted by depth. Consider only closest hit on a plane or oriented point.
                        // Cap the number of objects created. This avoids overloading both the
                        // rendering system and ARCore.
                        if (anchors.size() >= 20) {
                            anchors.get(0).detach();
                            anchors.remove(0);
                        }
                        // Adding an Anchor tells ARCore that it should track this position in
                        // space. This anchor is created on the Plane to place the 3D model
                        // in the correct position relative both to the world and to the plane.
                        anchors.add(hit.createAnchor());
                        break;
                    }
                }
            }

            // Draw background.
            mBackgroundRenderer.draw(frame);

            // If not tracking, don't draw 3d objects.
            if (camera.getTrackingState() == TrackingState.PAUSED) {
                return;
            }

            // Get projection matrix.
            float[] projmtx = new float[16];
            camera.getProjectionMatrix(projmtx, 0, 0.1f, 100.0f);

            // Get camera matrix and draw.
            float[] viewmtx = new float[16];
            camera.getViewMatrix(viewmtx, 0);

            // Compute lighting from average intensity of the image.
            final float lightIntensity = frame.getLightEstimate().getPixelIntensity();

            if (isShowPointCloud()) {
                // Visualize tracked points.
                PointCloud pointCloud = frame.acquirePointCloud();
                mPointCloud.update(pointCloud);
                mPointCloud.draw(viewmtx, projmtx);

                // Application is responsible for releasing the point cloud resources after
                // using it.
                pointCloud.release();
            }

            // Check if we detected at least one plane. If so, hide the loading message.
            if (mMessageSnackbar != null) {
                for (Plane plane : mSession.getAllTrackables(Plane.class)) {
                    if (plane.getType() == Plane.Type.HORIZONTAL_UPWARD_FACING
                            && plane.getTrackingState() == TrackingState.TRACKING) {
                        hideLoadingMessage();
                        break;
                    }
                }
            }

            if (isShowPlane()) {
                // Visualize planes.
                mPlaneRenderer.drawPlanes(
                        mSession.getAllTrackables(Plane.class), camera.getDisplayOrientedPose(), projmtx);
            }

            // Visualize anchors created by touch.
            float scaleFactor = 1.0f;

            int i = 0;
            for (Anchor anchor : anchors) {
                if (anchor.getTrackingState() != TrackingState.TRACKING) {
                    continue;
                }
                // Get the current pose of an Anchor in world space. The Anchor pose is updated
                // during calls to session.update() as ARCore refines its estimate of the world.
                anchor.getPose().toMatrix(mAnchorMatrix, 0);

                if (mIsARMode) {
                    // Update and draw the model and its shadow.
                    mVirtualObject.updateModelMatrix(mAnchorMatrix, mScaleFactor);
                    mVirtualObjectShadow.updateModelMatrix(mAnchorMatrix, scaleFactor);
                    mVirtualObject.draw(viewmtx, projmtx, lightIntensity);
                    mVirtualObjectShadow.draw(viewmtx, projmtx, lightIntensity);
                } else {
                    if (mRemoteRenders.size() > i) {
                        AgoraVideoRender render = mRemoteRenders.get(i);
                        ++i;
                        if (render == null) continue;

                        Peer peer = render.getPeer();
                        if (peer.data == null || peer.data.capacity() == 0) continue;

                        mPeerObject.updateModelMatrix(mAnchorMatrix, scaleFactor);
                        mPeerObject.draw(viewmtx, projmtx, peer);
                    }
                }
            }

            sendARViewMessage(gl);
        } catch (Throwable t) {
            // Avoid crashing the application due to unhandled exceptions.
            Log.e(TAG, "Exception on the OpenGL thread", t);
        }
    }

    private void initRtcEngine() {
        mIsARMode = true;
        mHidePoint = true;
        mHidePlane = true;

        Button modeButton = findViewById(R.id.switch_mode);
        modeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                switchMode((Button)view);
            }
        });

        Button hidePointButton = findViewById(R.id.show_point_cloud);
        hidePointButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showPointCloud((Button)v);
            }
        });

        Button hidePlaneButton = findViewById(R.id.show_plane);
        hidePlaneButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showPlane((Button)v);
            }
        });

        Button zoomInButton = findViewById(R.id.zoom_in);
        zoomInButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mScaleFactor > 5.0f) {
                    return;
                }
                mScaleFactor += 0.2f;
            }
        });

        Button zoomOutButton = findViewById(R.id.zoom_out);
        zoomOutButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mScaleFactor < 0.1f) {
                    return;
                }
                mScaleFactor -= 0.2f;
            }
        });

        try {
            mRtcEventHandler = new IRtcEngineEventHandler() {
                @Override
                public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
                    printLog("joined channel " + channel);
                }

                @Override
                public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
                    addRemoteRender(uid);
                }

                @Override
                public void onUserOffline(int uid, int reason) {
                    for (int i = 0; i < mRemoteRenders.size(); ++i) {
                        AgoraVideoRender render = mRemoteRenders.get(i);
                        if (render.getPeer().uid == uid) {
                            mRemoteRenders.remove(uid);
                            mRtcEngine.setRemoteVideoRenderer(uid, null);
                        }
                    }
                }

                @Override
                public void onUserJoined(int uid, int elapsed) {
                }

                @Override
                public void onError(int err) {
                    printLog("Error: " + err);
                }

                @Override
                public void onWarning(int warn) {
                    printLog("Warning: " + warn);
                }
            };

            mRtcEngine = RtcEngine.create(this, getString(R.string.private_broadcasting_app_id), mRtcEventHandler);
            mRtcEngine.setParameters("{\"rtc.log_filter\": 65535}");
            mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
            mRtcEngine.enableVideo();
            mRtcEngine.enableDualStreamMode(true);

            mRtcEngine.setVideoProfile(Constants.VIDEO_PROFILE_480P, false);
            mRtcEngine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);

            mSource = new AgoraVideoSource();
            mRender = new AgoraVideoRender(0, true);
            mRtcEngine.setVideoSource(mSource);
            mRtcEngine.setLocalVideoRenderer(mRender);

            //mRtcEngine.startPreview();

            mRtcEngine.joinChannel(null, "arcore", "ARCore with RtcEngine", 0);

        } catch (Exception ex) {
            printLog(ex.toString());
        }

        HandlerThread thread = new HandlerThread("ArSendThread");
        thread.start();
        mSenderHandler = new Handler(thread.getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case SEND_AR_VIEW:
                        sendARView((Bitmap)msg.obj);
                }
            }
        };
    }

    private void showSnackbarMessage(String message, boolean finishOnDismiss) {
        mMessageSnackbar = Snackbar.make(
            AgoraARCoreActivity.this.findViewById(android.R.id.content),
            message, Snackbar.LENGTH_INDEFINITE);
        mMessageSnackbar.getView().setBackgroundColor(0xbf323232);
        if (finishOnDismiss) {
            mMessageSnackbar.setAction(
                "Dismiss",
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        mMessageSnackbar.dismiss();
                    }
                });
            mMessageSnackbar.addCallback(
                new BaseTransientBottomBar.BaseCallback<Snackbar>() {
                    @Override
                    public void onDismissed(Snackbar transientBottomBar, int event) {
                        super.onDismissed(transientBottomBar, event);
                        finish();
                    }
                });
        }
        mMessageSnackbar.show();
    }

    private void showLoadingMessage() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                showSnackbarMessage("Searching for surfaces...", false);
            }
        });
    }

    private void hideLoadingMessage() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mMessageSnackbar != null) {
                    mMessageSnackbar.dismiss();
                }
                mMessageSnackbar = null;
            }
        });
    }

    private void printLog(String message) {
        Log.e("ARCore", message);
    }

    private void sendARViewMessage(GL10 gl) {
        int w = mSurfaceView.getWidth();
        int h = mSurfaceView.getHeight();
        int sceneSize = w * h;
        if (mSendBuffer == null) {
            mSendBuffer = ByteBuffer.allocateDirect(sceneSize * 4);
            mSendBuffer.order(ByteOrder.nativeOrder());
        }
        mSendBuffer.position(0);

        gl.glReadPixels(0, 0, w, h, GL10.GL_RGBA, GL10.GL_UNSIGNED_BYTE, mSendBuffer);
        int pixelsBuffer[] = new int[sceneSize];
        mSendBuffer.asIntBuffer().get(pixelsBuffer);
        Bitmap bitmap = Bitmap.createBitmap(w, h, Bitmap.Config.RGB_565);
        bitmap.setPixels(pixelsBuffer, sceneSize - w, -w, 0, 0, w, h);
        pixelsBuffer = null;

        short sBuffer[] = new short[sceneSize];
        ShortBuffer tmpBuffer = ShortBuffer.wrap(sBuffer);
        bitmap.copyPixelsToBuffer(tmpBuffer);

        // Making created bitmap (from OpenGL points) compatible with
        // Android bitmap
        for (int i = 0; i < sceneSize; ++i) {
            short v = sBuffer[i];
            sBuffer[i] = (short) (((v & 0x1f) << 11) | (v & 0x7e0) | ((v & 0xf800) >> 11));
        }
        tmpBuffer.rewind();
        bitmap.copyPixelsFromBuffer(tmpBuffer);
        Bitmap result = bitmap.copy(Bitmap.Config.ARGB_8888,false);

        Message message = Message.obtain();
        message.what = SEND_AR_VIEW;
        message.obj = result;
        mSenderHandler.sendMessage(message);
    }

    private void sendARView(Bitmap bitmap) {
        if (bitmap == null) return;

        if (mSource.getConsumer() == null) return;

        //Bitmap bitmap = source.copy(Bitmap.Config.ARGB_8888,true);
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();

        int size = bitmap.getRowBytes() * bitmap.getHeight();
        ByteBuffer byteBuffer = ByteBuffer.allocate(size);
        bitmap.copyPixelsToBuffer(byteBuffer);
        byte[] data = byteBuffer.array();

        mSource.getConsumer().consumeByteArrayFrame(data, MediaIO.PixelFormat.RGBA.intValue(), width, height, 270, System.currentTimeMillis());
    }

    private void addRemoteRender(int uid) {
        AgoraVideoRender render = new AgoraVideoRender(uid, false);
        mRemoteRenders.add(render);
        mRtcEngine.setRemoteVideoRenderer(uid, render);
    }

    private static final int BASE_VALUE_PERMISSION = 0X0001;
    private static final int PERMISSION_REQ_ID_RECORD_AUDIO = BASE_VALUE_PERMISSION + 1;
    private static final int PERMISSION_REQ_ID_CAMERA = BASE_VALUE_PERMISSION + 2;
    private static final int PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE = BASE_VALUE_PERMISSION + 3;

    private boolean checkSelfPermissions() {
        return checkSelfPermission(Manifest.permission.RECORD_AUDIO, PERMISSION_REQ_ID_RECORD_AUDIO) &&
                checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA) &&
                checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE);
    }

    public boolean checkSelfPermission(String permission, int requestCode) {
        //log.debug("checkSelfPermission " + permission + " " + requestCode);
        if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this, new String[]{permission}, requestCode);
            return false;
        }

        return true;
    }

    private void switchMode(Button button) {
        button.setText((mIsARMode = !mIsARMode) ? getString(R.string.ar_mode) :
                getString(R.string.agora_mode));
    }

    private void showPointCloud(Button button) {
        button.setText((mHidePoint = !mHidePoint) ? getString(R.string.show_point) :
                getString(R.string.hide_point));
    }

    private void showPlane(Button button) {
        button.setText((mHidePlane = !mHidePlane) ? getString(R.string.show_plane) :
                getString(R.string.hide_plane));
    }

    private boolean isARMode() {
        return mIsARMode;
    }

    private boolean isShowPointCloud() {
        return !mHidePoint;
    }

    private boolean isShowPlane() {
        return !mHidePlane;
    }
}
