package io.agora.advancedvideo.localvideo;

import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;

import java.io.IOException;
import java.nio.ByteBuffer;

import io.agora.advancedvideo.gles.ProgramTextureOES;
import io.agora.advancedvideo.gles.core.EglCore;
import io.agora.advancedvideo.gles.core.GlUtil;

public class LocalVideoCodec implements TextureView.SurfaceTextureListener {
    public interface OnFrameTransferListener {
        void onFrameTransfer(int textureId, int width, int height, float[] matrix);
    }

    private static final String TAG = LocalVideoCodec.class.getSimpleName();

    private LocalVideoThread mVideoThread;
    private String mFilePath;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private volatile SurfaceTexture mLocalPreviewSurfaceTexture;

    private OnFrameTransferListener mListener;

    public LocalVideoCodec(String filePath, OnFrameTransferListener listener) {
        mFilePath = filePath;
        mListener = listener;
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        mLocalPreviewSurfaceTexture = surfaceTexture;
        mVideoThread = new LocalVideoThread(mFilePath);
        mVideoThread.start();
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int width, int height) {
        mSurfaceWidth = width;
        mSurfaceHeight = height;
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
        if (mVideoThread != null && mVideoThread.isAlive()) {
            mVideoThread.setStopped(true);
        }
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {
        // Do nothing
    }

    private class LocalVideoThread extends Thread {
        private static final int INDEX_NO_BUFFER = -1;

        private int mVideoWidth;
        private int mVideoHeight;
        private int mVideoTrackIdx = -1;
        private String mMineType;

        private MediaExtractor mExtractor;
        private MediaFormat mVideoFormat;
        private MediaCodec mDecoder;
        private MediaCodec.BufferInfo mCodecBufferInfo;

        private EglCore mEglCore;
        private EGLSurface mDefaultSurface;
        private EGLSurface mDrawSurface;
        private int mTextureId;
        private SurfaceTexture mSurfaceTexture;
        private Surface mSurface;
        private float[] mTransform = new float[16];
        private ProgramTextureOES mProgram;

        private volatile boolean mStopped;
        private VideoSync mVideoSync;

        LocalVideoThread(String filePath) {
            initMedia(filePath);
        }

        private void initMedia(String filePath) {
            mExtractor = new MediaExtractor();
            try {
                mExtractor.setDataSource(filePath);
            } catch (IOException e) {
                Log.e(TAG, "Wrong video file");
            }

            for (int i = 0; i < mExtractor.getTrackCount(); i++) {
                MediaFormat format = mExtractor.getTrackFormat(i);
                String mime = format.getString(MediaFormat.KEY_MIME);
                if (mime != null && mime.startsWith("video/")) {
                    mVideoFormat = format;
                    mMineType = mime;
                    mVideoTrackIdx = i;
                    break;
                }
            }

            if (mVideoTrackIdx == -1) {
                Log.e(TAG, "Cannot find a video track");
                return;
            }

            mExtractor.selectTrack(mVideoTrackIdx);
            mVideoWidth = mVideoFormat.getInteger(MediaFormat.KEY_WIDTH);
            mVideoHeight = mVideoFormat.getInteger(MediaFormat.KEY_HEIGHT);

            try {
                mDecoder = MediaCodec.createDecoderByType(mMineType);
                mCodecBufferInfo = new MediaCodec.BufferInfo();
            } catch (IOException e) {
                Log.e(TAG, "Failed to create decoder of mime type " + mMineType);
            }
        }

        private void initOpenGLES() {
            mEglCore = new EglCore();
            mDefaultSurface = mEglCore.createOffscreenSurface(1, 1);
            mEglCore.makeCurrent(mDefaultSurface);
            mTextureId = GlUtil.createTextureObject(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
            mSurfaceTexture = new SurfaceTexture(mTextureId);
            mSurfaceTexture.setDefaultBufferSize(mSurfaceWidth, mSurfaceHeight);
            mSurface = new Surface(mSurfaceTexture);
            mProgram = new ProgramTextureOES();
        }

        private void startDecodeWithSurface() {
            mDecoder.configure(mVideoFormat, mSurface, null, 0);
            mDecoder.start();
        }

        private void initSync() {
            mVideoSync = new VideoSync();
        }

        @Override
        public void run() {
            initOpenGLES();
            startDecodeWithSurface();
            initSync();

            while (!mStopped && !isInterrupted()) {
                int inputBufferIndex = mDecoder.dequeueInputBuffer(0);

                long presentTS = 0;

                if (inputBufferIndex != INDEX_NO_BUFFER) {
                    presentTS = mExtractor.getSampleTime();
                    ByteBuffer[] inputBuffers = mDecoder.getInputBuffers();
                    int size = mExtractor.readSampleData(inputBuffers[inputBufferIndex], 0);
                    boolean isEndOfStream = false;

                    if (size == -1) {
                        isEndOfStream = true;
                        size = 0;
                    }

                    mDecoder.queueInputBuffer(inputBufferIndex, 0, size, presentTS,
                            isEndOfStream ? MediaCodec.BUFFER_FLAG_END_OF_STREAM : 0);
                    mExtractor.advance();
                }

                int outputBufferIndex = mDecoder.dequeueOutputBuffer(mCodecBufferInfo, 0);
                if (isValidOutputBuffer(outputBufferIndex)) {
                    try {
                        mDecoder.releaseOutputBuffer(outputBufferIndex, mCodecBufferInfo.size > 0);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                if ((mCodecBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    break;
                }

                try {
                    mSurfaceTexture.updateTexImage();
                    mSurfaceTexture.getTransformMatrix(mTransform);
                } catch (Exception e) {
                    e.printStackTrace();
                    break;
                }

                if (mLocalPreviewSurfaceTexture != null && mDrawSurface == null) {
                    mDrawSurface = mEglCore.createWindowSurface(mLocalPreviewSurfaceTexture);
                }

                if (mDrawSurface != null) {
                    if (!mEglCore.isCurrent(mDrawSurface)) {
                        mEglCore.makeCurrent(mDrawSurface);
                        setViewPort(mVideoWidth, mVideoHeight, mSurfaceWidth, mSurfaceHeight);
                    }

                    mProgram.drawFrame(mTextureId, mTransform);
                    mEglCore.swapBuffers(mDrawSurface);
                }

                if (mListener != null) {
                    mListener.onFrameTransfer(mTextureId, mVideoWidth, mVideoHeight, mTransform);
                }

                try {
                    long timeToWait = mVideoSync.timeToWait(presentTS);
                    Thread.sleep(timeToWait);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            releaseDecoder();
            releaseOpenGLES();
        }

        private boolean isValidOutputBuffer(int index) {
            return index != MediaCodec.INFO_TRY_AGAIN_LATER &&
                    index != MediaCodec.INFO_OUTPUT_FORMAT_CHANGED &&
                    index != MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED;

        }

        private void setViewPort(int videoW, int videoH, int surfaceW, int surfaceH) {
            float videoRatio = videoW / (float) videoH;
            float surfaceRatio = surfaceW / (float) surfaceH;
            if (videoRatio == surfaceRatio) {
                GLES20.glViewport(0, 0, videoW, videoH);
                return;
            }

            int startX;
            int startY;
            int viewPortW;
            int viewPortH;
            if (videoRatio > surfaceRatio) {
                // the video is wider than the surface
                viewPortW = surfaceW;
                viewPortH = (int) (surfaceW / videoRatio);
                startX = 0;
                startY = (surfaceH - viewPortH) / 2;
            } else {
                // surface is wider than the video
                viewPortH = surfaceH;
                viewPortW = (int) (viewPortH * videoRatio);
                startX = (surfaceW - viewPortW) / 2;
                startY = 0;
            }

            Log.i(TAG, startX + ":" + startY + ":" + viewPortW + ":" + viewPortH);
            GLES20.glViewport(startX, startY, viewPortW, viewPortH);
        }

        void setStopped(boolean stopped) {
            mStopped = stopped;
        }

        private void releaseDecoder() {
            if (mDecoder != null) {
                mDecoder.flush();
                mDecoder.stop();
            }

            if (mExtractor != null) {
                mExtractor.release();
            }
        }

        private void releaseOpenGLES() {
            mSurface.release();
            mProgram.release();
            mEglCore.makeNothingCurrent();
            mEglCore.releaseSurface(mDefaultSurface);
            mSurfaceTexture.release();
            GlUtil.deleteTextureObject(mTextureId);
            mTextureId = 0;
            mEglCore.release();
        }
    }

    /**
     * Synchronizes the pacing of sending video frames
     * to decoder according to their presentation timestamps
     */
    private class VideoSync {
        private long mLastTS;
        private long mLastPresent;
        private long mLastWait;

        long timeToWait(long presentTS) {
            long diff = (presentTS - mLastPresent) / 1000;
            if (mLastWait == 0 || mLastTS == 0) {
                // First sync, take the first presentation
                // time in milliseconds as the wait time.
                mLastWait = diff;
            } else if (diff > 0 && diff <= mLastWait * 1.5) {
                // Normal diff of the presentation timestamps
                // between this and the last frames, calculate
                // the actual wait time.
                // If the frame is a B-frame, whose presentation
                // is still too far from now (specifically, more
                // than 1.5 times of last wait time), use the last
                // wait time.
                long next = mLastTS + diff;
                long stillHave = next - System.currentTimeMillis();
                mLastWait = stillHave > 0 ? stillHave : mLastWait;
            }

            mLastPresent = presentTS;
            mLastTS = System.currentTimeMillis();
            return mLastWait;
        }
    }
}
