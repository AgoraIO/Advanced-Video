package io.agora.advancedvideo.videopush;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.RelativeLayout;

import java.io.IOException;

import io.agora.advancedvideo.activities.BaseLiveActivity;
import io.agora.advancedvideo.gles.ProgramTextureOES;
import io.agora.advancedvideo.gles.core.EglCore;
import io.agora.advancedvideo.gles.core.GlUtil;
import io.agora.rtc.Constants;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.AgoraVideoFrame;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class VideoPushActivity extends BaseLiveActivity
        implements TextureView.SurfaceTextureListener, SurfaceTexture.OnFrameAvailableListener {
    private static final String TAG = VideoPushActivity.class.getSimpleName();
    private static final int DEFAULT_CAPTURE_WIDTH = 640;
    private static final int DEFAULT_CAPTURE_HEIGHT = 480;

    private int mPreviewTexture;
    private SurfaceTexture mPreviewSurfaceTexture;
    private EglCore mEglCore;
    private EGLSurface mDummySurface;
    private EGLSurface mDrawSurface;
    private ProgramTextureOES mProgram;
    private float[] mTransform = new float[16];
    private float[] mMVPMatrix = new float[16];
    private boolean mMVPMatrixInit = false;
    private byte[] mBuffer = new byte[DEFAULT_CAPTURE_WIDTH * DEFAULT_CAPTURE_HEIGHT * 3 / 2];

    private Camera mCamera;
    private int mFacing = Camera.CameraInfo.CAMERA_FACING_BACK;
    private boolean mPreviewing = false;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private boolean mTextureDestroyed;

    private volatile boolean mChannelJoined;
    private int mRemoteWidth;
    private int mRemoteHeight;
    private int mRemoteTop;
    private int mRemoteEnd;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onInitializeVideo() {
        if (mIsBroadcaster) {
            setupLocalPreview();
        }

        mRemoteWidth = getResources().getDimensionPixelSize(R.dimen.remote_preview_width);
        mRemoteHeight = getResources().getDimensionPixelSize(R.dimen.remote_preview_height);
        mRemoteTop = getResources().getDimensionPixelSize(R.dimen.remote_preview_margin_top);
        mRemoteEnd = getResources().getDimensionPixelSize(R.dimen.remote_preview_margin_end);

        initRtcEngine();
    }

    private void initRtcEngine() {
        rtcEngine().setParameters("{\"rtc.log_filter\":65535}");
        rtcEngine().setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                new VideoEncoderConfiguration.VideoDimensions(DEFAULT_CAPTURE_WIDTH, DEFAULT_CAPTURE_HEIGHT),
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT
        ));

        rtcEngine().setExternalVideoSource(true, true, true);
        rtcEngine().muteLocalAudioStream(true);
        rtcEngine().joinChannel(null, config().getChannelName(), null, 0);
    }

    private void setupLocalPreview() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TextureView textureView = new TextureView(VideoPushActivity.this);
                textureView.setSurfaceTextureListener(VideoPushActivity.this);
                videoContainer.addView(textureView, 0);
            }
        });
    }

    private void removeLocalPreview() {
        videoContainer.removeViewAt(0);
    }

    private void setupRemotePreview(final int uid) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                SurfaceView surfaceView = RtcEngine.CreateRendererView(VideoPushActivity.this);
                surfaceView.setZOrderOnTop(true);

                RelativeLayout.LayoutParams params =
                        new RelativeLayout.LayoutParams(mRemoteWidth, mRemoteHeight);
                params.topMargin = mRemoteTop;
                params.rightMargin = mRemoteEnd;
                params.addRule(RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.TRUE);
                params.addRule(RelativeLayout.ALIGN_PARENT_END, RelativeLayout.TRUE);
                videoContainer.addView(surfaceView, 1, params);

                rtcEngine().setupRemoteVideo(new VideoCanvas(
                        surfaceView, VideoCanvas.RENDER_MODE_HIDDEN, uid));
            }
        });
    }

    private void removeRemotePreview(int uid) {
        videoContainer.removeViewAt(1);
        rtcEngine().setupRemoteVideo(new VideoCanvas(
                null, VideoCanvas.RENDER_MODE_HIDDEN, uid));
    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        mChannelJoined = true;
        Log.i(TAG, "onJoinChannelSuccess channel:" + channel + " uid:" + (0xFFFFFFFFL));
    }

    @Override
    public void onRemoteVideoStateChanged(int uid, int state, int reason, int elapsed) {
        if (state == Constants.REMOTE_VIDEO_STATE_DECODING) {
            Log.i(TAG, "remote video decoded:" + (uid & 0xFFFFFFFFL));
            setupRemotePreview(uid);
        } else if (state == Constants.REMOTE_VIDEO_STATE_STOPPED) {
            Log.i(TAG, "remote video stopped:" + (uid & 0xFFFFFFFFL));
            removeRemotePreview(uid);
        } else if (state == Constants.REMOTE_VIDEO_STATE_FROZEN) {
            Log.i(TAG, "remote video frozen:" + (uid & 0xFFFFFFFFL));
        }
    }

    @Override
    public void onUserOffline(int uid, int reason) {
        Log.i(TAG, "onUserOffline channel uid:" + (uid & 0xFFFFFFFFL));
    }

    @Override
    public void onUserJoined(int uid, int elapsed) {
        Log.i(TAG, "onUserJoined channel: uid:" + (uid & 0xFFFFFFFFL));
    }

    @Override
    protected void onLeaveButtonClicked(View view) {

    }

    @Override
    public void finish() {
        rtcEngine().leaveChannel();
        super.finish();
    }

    @Override
    protected void onSwitchCameraButtonClicked(View view) {

    }

    @Override
    protected void onMuteAudioButtonClicked(View view) {

    }

    @Override
    protected void onMuteVideoButtonClicked(View view) {

    }

    @Override
    protected void onMoreButtonClicked(View view) {

    }

    private void initOpenGL(SurfaceTexture surfaceTexture, int width, int height) {
        mTextureDestroyed = false;
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        mEglCore = new EglCore();
        mDummySurface = mEglCore.createOffscreenSurface(1, 1);
        mEglCore.makeCurrent(mDummySurface);
        mPreviewTexture = GlUtil.createTextureObject(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
        mPreviewSurfaceTexture = new SurfaceTexture(mPreviewTexture);
        mPreviewSurfaceTexture.setOnFrameAvailableListener(this);

        mDrawSurface = mEglCore.createWindowSurface(surfaceTexture);
        mProgram = new ProgramTextureOES();
    }

    private void openCamera() {
        if (mCamera != null || mPreviewing) {
            Log.i(TAG, "Camera preview has been started");
            return;
        }

        try {
            mCamera = Camera.open(mFacing);

            // It is assumed to capture images of resolution 640x480.
            // During development, it should be set the most suitable
            // supported resolution that best fits the scenario.
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewSize(DEFAULT_CAPTURE_WIDTH, DEFAULT_CAPTURE_HEIGHT);
            mCamera.setParameters(parameters);
            mCamera.setPreviewTexture(mPreviewSurfaceTexture);

            // The display orientation is 90 for both front and back
            // facing cameras using a surface texture for the preview
            // when the screen is in portrait mode.
            mCamera.setDisplayOrientation(90);
            startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void startPreview() {
        if (mCamera != null && !mPreviewing) {
            mCamera.startPreview();
            mPreviewing = true;
        }
    }

    private void stopPreview() {
        if (mCamera != null && mPreviewing) {
            mCamera.stopPreview();
            mPreviewing = false;
        }
    }

    private void closeCamera() {
        if (mCamera != null) {
            stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }

    private int changeFacing() {
        return mFacing == Camera.CameraInfo.CAMERA_FACING_FRONT
                ? Camera.CameraInfo.CAMERA_FACING_BACK
                : Camera.CameraInfo.CAMERA_FACING_FRONT;
    }

    private void switchCamera() {
        closeCamera();
        mFacing = changeFacing();
        // Front and back facing cameras have
        // different transformation matrices.
        mMVPMatrixInit = false;
        openCamera();
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
        Log.i(TAG, "onSurfaceTextureAvailable:" + width + "x" + height);
        initOpenGL(surfaceTexture, width, height);
        openCamera();
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
        Log.i(TAG, "onSurfaceTextureDestroyed");
        mTextureDestroyed = true;
        closeCamera();
        mProgram.release();
        mEglCore.releaseSurface(mDummySurface);
        mEglCore.releaseSurface(mDrawSurface);
        mEglCore.release();
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {
        
    }

    /**
     * To notify when the preview surface texture has been updated.
     * Called by the system camera module.
     * @param surfaceTexture camera preview surface texture.
     */
    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (mTextureDestroyed) return;

        if (!mEglCore.isCurrent(mDrawSurface)) {
            mEglCore.makeCurrent(mDrawSurface);
        }

        try {
            mPreviewSurfaceTexture.updateTexImage();
            mPreviewSurfaceTexture.getTransformMatrix(mTransform);
        } catch (Exception e) {
            e.printStackTrace();
        }

        // The rectangle ratio of frames and the screen surface
        // may be different, so cropping may happen when display
        // frames to the screen.
        calculateDisplayTransform();
        GLES20.glViewport(0, 0, mSurfaceWidth, mSurfaceHeight);
        mProgram.drawFrame(mPreviewTexture, mTransform, mMVPMatrix);
        mEglCore.swapBuffers(mDrawSurface);

        if (mChannelJoined) {
            rtcEngine().pushExternalVideoFrame(
                    buildVideoFrame(mPreviewTexture, mTransform));
        }
    }

    private void calculateDisplayTransform() {
        // The display transformation matrix does not change
        // for the same camera when the screen orientation
        // remains the same.
        if (mMVPMatrixInit) return;

        // For simplicity, we only consider the activity as
        // portrait mode. In this case, the captured images
        // should be rotated 90 degrees (left or right).
        // Thus the frame width and height should be swapped.
        float frameRatio = DEFAULT_CAPTURE_HEIGHT / (float) DEFAULT_CAPTURE_WIDTH;
        float surfaceRatio = mSurfaceWidth / (float) mSurfaceHeight;
        Matrix.setIdentityM(mMVPMatrix, 0);

        if (frameRatio >= surfaceRatio) {
            float w = DEFAULT_CAPTURE_WIDTH * surfaceRatio;
            float scaleW = DEFAULT_CAPTURE_HEIGHT / w;
            Matrix.scaleM(mMVPMatrix, 0, scaleW, 1, 1);
        } else {
            float h = DEFAULT_CAPTURE_HEIGHT / surfaceRatio;
            float scaleH = DEFAULT_CAPTURE_WIDTH / h;
            Matrix.scaleM(mMVPMatrix, 0, 1, scaleH, 1);
        }

        mMVPMatrixInit = true;
    }

    private AgoraVideoFrame buildVideoFrame(int textureId, float[] transform) {
        AgoraVideoFrame frame = new AgoraVideoFrame();
        frame.textureID = textureId;
        frame.format = AgoraVideoFrame.FORMAT_TEXTURE_OES;
        frame.transform = transform;
        frame.stride = DEFAULT_CAPTURE_HEIGHT;
        frame.height = DEFAULT_CAPTURE_WIDTH;
        frame.eglContext14 = mEglCore.getEGLContext();
        frame.timeStamp = System.currentTimeMillis();
        return frame;
    }
}
