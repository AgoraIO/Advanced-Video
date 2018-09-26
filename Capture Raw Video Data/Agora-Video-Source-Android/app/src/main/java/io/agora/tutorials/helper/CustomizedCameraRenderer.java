package io.agora.tutorials.helper;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

public class CustomizedCameraRenderer extends GLSurfaceView implements
        GLSurfaceView.Renderer,
        SurfaceTexture.OnFrameAvailableListener {

    private static final String LOG_TAG = "CustomizedRenderer";

    private static final boolean DBG = false;
    private boolean isSetViewHidden = false;
    private EGLContext mEGLCurrentContext;

    private static class MyContextFactory implements EGLContextFactory {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        private CustomizedCameraRenderer mRenderer;

        public MyContextFactory(CustomizedCameraRenderer renderer) {
            Log.d(LOG_TAG, "MyContextFactory " + renderer);
            this.mRenderer = renderer;
        }

        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            Log.d(LOG_TAG, "createContext " + egl + " " + display + " " + eglConfig);
            checkEglError("before createContext", egl);
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE};

            EGLContext ctx;

            if (mRenderer.mEGLCurrentContext == null) {
                mRenderer.mEGLCurrentContext = egl.eglCreateContext(display, eglConfig,
                        EGL10.EGL_NO_CONTEXT, attrib_list);
                ctx = mRenderer.mEGLCurrentContext;
            } else {
                ctx = mRenderer.mEGLCurrentContext;
            }
            checkEglError("after createContext", egl);
            return ctx;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            Log.d(LOG_TAG, "destroyContext " + egl + " " + display + " " + context + " " + mRenderer.mEGLCurrentContext);
            if (mRenderer.mEGLCurrentContext == null) {
                egl.eglDestroyContext(display, context);
            }
        }

        private static void checkEglError(String prompt, EGL10 egl) {
            int error;
            while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
                Log.d(LOG_TAG, String.format(Locale.US, "%s: EGL error: 0x%x", prompt, error));
            }
        }
    }

    public interface OnFrameAvailableListener {
        void onFrameAvailable(int texture, EGLContext eglContext, int rotation, float[] matrix);
    }

    private OnFrameAvailableListener mOnFrameAvailableHandler;

    public interface OnEGLContextListener {
        void onEGLContextReady(EGLContext eglContext);
    }

    private OnEGLContextListener mOnEGLContextHandler;

    private Context mContext;

    private Camera mCamera;

    private int mCameraIndex = Camera.CameraInfo.CAMERA_FACING_FRONT;
    // mount angle, 270 degrees; back: 90 degrees
    // content: back camera's top row is aligned to left bar; front camera's top row is aligned to right bar
    // counter-clock wise

    private int mCameraRotation = 0;

    private volatile SurfaceTexture mSurfaceTexture;
    private volatile SurfaceTexture mDstSurfaceTexture;

    public final OESTexture mSrcTexture = new OESTexture();
    private final Shader mOffscreenShader = new Shader();
    private int mViewWidth, mViewHeight;
    private volatile boolean mUpdateTexture = false;

    private ByteBuffer mFullQuadVertices;
    private float[] mOrientationM = new float[16];
    private float[] mRatio = new float[2];

    private boolean mPreviewing = false;

    public CustomizedCameraRenderer(Context context) {
        super(context);
        mContext = context;
        init();
    }

    public CustomizedCameraRenderer(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    private void init() {
        // Create full scene quad buffer
        final byte FULL_QUAD_COORDS[] = {-1, 1, -1, -1, 1, 1, 1, -1};
        mFullQuadVertices = ByteBuffer.allocateDirect(4 * 2);
        mFullQuadVertices.put(FULL_QUAD_COORDS).position(0);

        setEGLContextFactory(new MyContextFactory(this));

        setPreserveEGLContextOnPause(true);
        setEGLContextClientVersion(2);
        setRenderer(this);

        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        setDebugFlags(DEBUG_LOG_GL_CALLS);
    }

    public void setOnEGLContextHandler(OnEGLContextListener listener) {
        this.mOnEGLContextHandler = listener;
    }

    public void setOnFrameAvailableHandler(OnFrameAvailableListener listener) {
        this.mOnFrameAvailableHandler = listener;
    }

    @Override
    public synchronized void onFrameAvailable(SurfaceTexture surfaceTexture) {
        mUpdateTexture = true;
        requestRender();
    }

    @Override
    public synchronized void onSurfaceCreated(GL10 gl, EGLConfig config) {
        if (DBG) {
            Log.d(LOG_TAG, "onSurfaceCreated " + gl);
        }
        initCameraTexture();
        Log.d(LOG_TAG, "onSurfaceCreated " + gl + " end");
    }

    public void initCameraTexture() {
        try {
            mOffscreenShader.setProgram(io.agora.tutorials.customizedvideosource.R.raw.vshader, io.agora.tutorials.customizedvideosource.R.raw.fshader, mContext);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (mCamera == null) {
            mCamera = Camera.open(mCameraIndex);
        }

        if (mOnEGLContextHandler != null) {
            if (mEGLCurrentContext != null) {
                mOnEGLContextHandler.onEGLContextReady(mEGLCurrentContext);
            }
        }

        // Generate camera texture
        mSrcTexture.init();

        // Set up SurfaceTexture
        SurfaceTexture oldSurfaceTexture = mSurfaceTexture;
        mSurfaceTexture = new SurfaceTexture(mSrcTexture.getTextureId());
        mSurfaceTexture.setOnFrameAvailableListener(this);
        if (oldSurfaceTexture != null) {
            oldSurfaceTexture.release();
        }
    }

    private int mCameraPreviewWidth = 1080;
    private int mCameraPreviewHeight = 720;

    @SuppressLint("NewApi")
    @Override
    public synchronized void onSurfaceChanged(GL10 gl, int width, int height) {
        if (DBG) {
            Log.d(LOG_TAG, "onSurfaceChanged " + gl + " " + width + " " + height);
        }

        mViewWidth = width;
        mViewHeight = height;

        if (mPreviewing) {
            mCamera.stopPreview();
        }

        // set camera paras
        int camera_width = 0;
        int camera_height = 0;

        try {
            mCamera.setPreviewTexture(mSurfaceTexture);
        } catch (IOException ioe) {
            Log.w(LOG_TAG, "setPreviewTexture " + Log.getStackTraceString(ioe));
        }

        Camera.Parameters param = mCamera.getParameters();
        List<Size> psize = param.getSupportedPreviewSizes();
        if (psize.size() > 0) {
            boolean supports_640_480 = false;
            for (int i = 0; i < psize.size(); i++) {
                if ((psize.get(i).width == 640) && (psize.get(i).height == 480)) {
                    supports_640_480 = true;
                }
            }
            if (supports_640_480) {
                mCameraPreviewWidth = 640;
                mCameraPreviewHeight = 480;
            } else {
                mCameraPreviewWidth = param.getSupportedPreviewSizes().get(0).width;
                mCameraPreviewHeight = param.getSupportedPreviewSizes().get(0).height;
            }

            List<int[]> supportedFPSRange = param.getSupportedPreviewFpsRange();

            for (int[] item : supportedFPSRange) {
                Log.d(LOG_TAG, "supported fps range " + item[0] + " " + item[1]);
            }

            int defaultMaxFps = (supportedFPSRange.get(0)[1] + supportedFPSRange.get(0)[1]) / 2;
            if (supportedFPSRange.size() > 0) {
                defaultMaxFps = ((defaultMaxFps <= supportedFPSRange.get(0)[1]) ? defaultMaxFps : supportedFPSRange.get(0)[1]);
                Log.d(LOG_TAG, "setPreviewFpsRange " + supportedFPSRange.get(0)[0] + " " + defaultMaxFps);
                param.setPreviewFpsRange(supportedFPSRange.get(0)[0], defaultMaxFps);
            }
            Log.d(LOG_TAG, "setPreviewSize " + mCameraPreviewWidth + " " + mCameraPreviewHeight);
            param.setPreviewSize(mCameraPreviewWidth, mCameraPreviewHeight);
        }

        // get the camera orientation and display dimension
        if (mContext.getResources().getConfiguration().orientation ==
                Configuration.ORIENTATION_PORTRAIT) {
            Matrix.setRotateM(mOrientationM, 0, 90.0f, 0f, 0f, 1f);
            mRatio[1] = camera_width * 1.0f / height;
            mRatio[0] = camera_height * 1.0f / width;
        } else {
            Matrix.setRotateM(mOrientationM, 0, 0.0f, 0f, 0f, 1f);
            mRatio[1] = camera_height * 1.0f / height;
            mRatio[0] = camera_width * 1.0f / width;
        }

        // start camera
        mCamera.setParameters(param);

        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(mCameraIndex, info);
        mCameraRotation = info.orientation;

        mCamera.startPreview();

        param = mCamera.getParameters();
        String dumpedParam = param.flatten();
        mPreviewing = true;

        Log.d(LOG_TAG, "onSurfaceChanged end " + gl + " " + dumpedParam + " " + mCameraRotation + " " + mPreviewing);
    }

    private TextureRenderer mFboToView;
    private TextureRenderer mCameraToFbo;

    public int getDisplayRotation() {
        int rotation = ((WindowManager) (getContext().getSystemService(Context.WINDOW_SERVICE))).getDefaultDisplay()
                .getRotation();
        switch (rotation) {
            case Surface.ROTATION_0:
                return 0;
            case Surface.ROTATION_90:
                return 90;
            case Surface.ROTATION_180:
                return 180;
            case Surface.ROTATION_270:
                return 270;
        }
        return 0;
    }

    public void setViewHiddenStatus(boolean isHidden) {
        this.isSetViewHidden = isHidden;
    }

    @Override
    public synchronized void onDrawFrame(GL10 gl) {
        if (DBG) {
            Log.d(LOG_TAG, "onDrawFrame " + mUpdateTexture + " " + mCameraPreviewWidth + " " + mCameraPreviewHeight);
        }

        if (mFbo == 0) {
            createFbo(1080, 1920);

            if (mDstSurfaceTexture != null) {
                mDstSurfaceTexture.release();
            }
            mDstSurfaceTexture = new SurfaceTexture(mDstTexture);
        }

        if (mCameraToFbo == null) {
            mCameraToFbo = new TextureRenderer(true);
        }

        if (mFboToView == null) {
            mFboToView = new TextureRenderer(false);
        }

        // Calculate rotated degrees (camera to view)
        int degrees = getDisplayRotation();

        // render the texture to FBO if new frame is available
        if (mUpdateTexture) {
            GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            // Latch surface texture
            mSurfaceTexture.updateTexImage();

            // Render to FBO (TODO: resize/crop to target size)
            GLES20.glFinish();
            GLES20.glViewport(0, 0, 1080, 1920);

            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFbo);

            mCameraToFbo.rotate(mCameraRotation);

            mCameraToFbo.draw(mSrcTexture.getTextureId());
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

            // Render to this view
            int targetWidth;
            int targetHeight;
            int paddingHeight;
            if (isSetViewHidden) {
                targetWidth = mViewWidth;
                targetHeight = (int) ((mCameraPreviewWidth * targetWidth * 100.0f / mCameraPreviewHeight) / 100);
                paddingHeight = (mViewHeight - targetHeight) / 2;
            } else {
                targetHeight = mViewHeight;
                targetWidth = (int) ((mCameraPreviewHeight * targetHeight * 100.0f / mCameraPreviewWidth) / 100);
                paddingHeight = 0;
            }

            // FIXME should calculate the rotation dynamically and apply mirror for correctly video displaying
            int rotation;
            if (mCameraIndex == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                rotation = (-degrees + 180) % 360;
            } else {
                rotation = (degrees + 180) % 360;
            }

            if (DBG) {
                Log.d(LOG_TAG, "glViewport " + targetWidth + " " + targetHeight + " " + mViewWidth + " " + mViewHeight + " " + mCameraPreviewWidth + " " + mCameraPreviewHeight + " " + rotation);
            }
            GLES20.glViewport(0, paddingHeight, targetWidth, targetHeight);

            mFboToView.rotate(rotation);
            if (mCameraIndex == Camera.CameraInfo.CAMERA_FACING_BACK) {
                mFboToView.flip(true, false);
            }

            mFboToView.draw(mDstTexture);

            float[] matrix = new float[16];
            mDstSurfaceTexture.getTransformMatrix(matrix);

            mUpdateTexture = false;

            if (mOnFrameAvailableHandler != null) {
                mOnFrameAvailableHandler.onFrameAvailable(mDstTexture, mEGLCurrentContext, rotation, matrix);
            }
        }

        if (DBG) {
            Log.d(LOG_TAG, "onDrawFrame " + gl + " end orientation " + mCameraRotation + " " + degrees);
        }
    }

    private int mFbo = 0;
    private int mDstTexture = 0;

    private int createFbo(int width, int height) {
        int[] texture = new int[1];
        int[] fbo = new int[1];
        GLES20.glGenFramebuffers(1, fbo, 0);
        GLES20.glGenTextures(1, texture, 0);
        mFbo = fbo[0];
        mDstTexture = texture[0];

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mDstTexture);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);

        // Bind the framebuffer
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFbo);
        // Specify texture as color attachment
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, mDstTexture, 0);

        // Check for framebuffer complete
        int status = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER);
        if (status != GLES20.GL_FRAMEBUFFER_COMPLETE) {
            Log.e(LOG_TAG, "Failed to create framebuffer!!!");
        }

        return 0;
    }

    public void deinitCameraTexture() {
        mUpdateTexture = false;

        if (mDstSurfaceTexture != null) {
            mDstSurfaceTexture.release();
        }

        mSurfaceTexture.release();

        mPreviewing = false;

        mEGLCurrentContext = null;

        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mCamera.release();
        }

        mCamera = null;
    }

}
