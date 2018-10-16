package io.agora.rtc.mediaio.app.shareScreen.source;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Handler;
import android.util.Log;
import android.view.View;

import java.nio.ByteBuffer;

import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.video.AgoraVideoFrame;

public class ViewSharingCapturer implements IVideoSource {
    private static final String TAG = ViewSharingCapturer.class.getSimpleName();
    private static final int FPS = 10;
    private IVideoFrameConsumer mIVideoFrameConsumer;
    private boolean mCapturerIsPaused = false;
    private ViewCaptureSetting mViewCapturerSettings;
    private View mContentView;
    private Handler mFrameProducerHandler;
    private long mFrameProducerIntervalMillis = 1000 / FPS;
    private Runnable mFrameProducer = new Runnable() {
        @Override
        public void run() {
            if (mContentView != null) {
                mContentView.setDrawingCacheEnabled(true);
                mContentView.buildDrawingCache();
                Bitmap bmp = mContentView.getDrawingCache();

                byte[] byteTemp = getRGBFromBMP(bmp);
                ByteBuffer byteBufferTemp = ByteBuffer.allocateDirect(byteTemp.length);
                byteBufferTemp.put(byteTemp);
                if (bmp != null && mIVideoFrameConsumer != null && !mCapturerIsPaused) {

                    mIVideoFrameConsumer.consumeByteBufferFrame(
                            byteBufferTemp
                            , AgoraVideoFrame.FORMAT_RGBA, bmp.getWidth(), bmp.getHeight(), 0, System.currentTimeMillis());

                }
                mContentView.setDrawingCacheEnabled(false);
                byteBufferTemp = null;
                if (!mCapturerIsPaused) {
                    mFrameProducerHandler.postDelayed(mFrameProducer, mFrameProducerIntervalMillis);
                }
            } else {
                mFrameProducerHandler.postDelayed(mFrameProducer, mFrameProducerIntervalMillis);
            }

        }
    };

    public ViewSharingCapturer(View view) {
        mContentView = view;
        mFrameProducerHandler = new Handler();
        mViewCapturerSettings = new ViewCaptureSetting();
        mViewCapturerSettings.setFps(FPS);
        mViewCapturerSettings.setFormat(AgoraVideoFrame.BUFFER_TYPE_BUFFER);
    }

    public boolean onInitialize(IVideoFrameConsumer var1) {
        try {
            this.mIVideoFrameConsumer = var1;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public boolean onStart() {
        try {
            mCapturerIsPaused = false;
            mFrameProducerHandler.postDelayed(mFrameProducer, mFrameProducerIntervalMillis);
        } catch (Throwable throwable) {
            throwable.printStackTrace();
            return false;
        }
        return true;
    }

    public void onStop() {
        mCapturerIsPaused = true;
        mFrameProducerHandler.removeCallbacks(mFrameProducer);
    }

    public void onDispose() {
        Log.i(TAG, " onDispose");
    }

    public int getBufferType() {
        return mViewCapturerSettings.getFormat();
    }

    public static byte[] getRGBFromBMP(Bitmap bmp) {

        int w = bmp.getWidth();
        int h = bmp.getHeight();

        byte[] pixels = new byte[w * h * 4]; // Allocate for RGB

        int k = 0;

        for (int x = 0; x < h; x++) {
            for (int y = 0; y < w; y++) {
                int color = bmp.getPixel(y, x);
                // rgba
                pixels[k * 4 + 0] = (byte) Color.red(color);
                pixels[k * 4 + 1] = (byte) Color.green(color);
                pixels[k * 4 + 2] = (byte) Color.blue(color);
                pixels[k * 4 + 3] = (byte) Color.alpha(color);
                k++;
            }
        }

        return pixels;
    }
}