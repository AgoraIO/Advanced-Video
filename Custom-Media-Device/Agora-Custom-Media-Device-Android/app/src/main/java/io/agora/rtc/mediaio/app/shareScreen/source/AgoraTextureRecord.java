package io.agora.rtc.mediaio.app.shareScreen.source;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.projection.MediaProjection;
import android.util.Log;
import android.view.Surface;

import io.agora.rtc.mediaio.TextureSource;

import static io.agora.rtc.mediaio.MediaIO.PixelFormat.TEXTURE_OES;

public class AgoraTextureRecord extends TextureSource {
    private static final String TAG = AgoraTextureRecord.class.getSimpleName();
    private Context mContext;
    private Camera camera;
    private Camera.CameraInfo info;
    private int width = 320;
    private int height = 240;
    private int dpi;
    private int rotation = 0;

    private VirtualDisplay virtualDisplay;
    private MediaProjection mediaProjection;

    public AgoraTextureRecord(Context context, int width, int height, int dpi, MediaProjection mediaProjection) {
        super(null, width, height);
        Log.i(TAG, "init AgoraTextureRecord");
        this.width = width;
        this.height = height;
        this.mContext = context;
        this.dpi = dpi;
        this.mediaProjection = mediaProjection;
    }

    @Override
    public void onTextureFrameAvailable(int oesTextureId, float[] transformMatrix, long timestampNs) {
        super.onTextureFrameAvailable(oesTextureId, transformMatrix, timestampNs);
        if (mConsumer != null && mConsumer.get() != null) {
            mConsumer.get().consumeTextureFrame(oesTextureId, TEXTURE_OES.intValue(), mWidth, mHeight, rotation, System.currentTimeMillis(), transformMatrix);
        }
    }

    @Override
    protected boolean onCapturerOpened() {
        Log.i(TAG, "onCapturerOpened");
        createVirtualDisplay();
        return true;
    }

    @Override
    protected boolean onCapturerStarted() {
        Log.i(TAG, "onCapturerStarted");
        return true;
    }

    @Override
    protected void onCapturerStopped() {
        Log.i(TAG, "onCapturerStopped");
    }

    @Override
    protected void onCapturerClosed() {

        Log.i(TAG, "onCapturerClosed");
    }

    public void sourceRelease() {
        Log.i(TAG, "sourceRelease");
        releaseProjection();
        release();
    }


    // record operations
    private void createVirtualDisplay() {
        Log.i(TAG, "createVirtualDisplay:" + mediaProjection);
        Surface inputSurface = new Surface(getSurfaceTexture());
        if (virtualDisplay == null) {
            virtualDisplay = mediaProjection.createVirtualDisplay("MainScreen", width, height, dpi,
                    DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR, inputSurface, null, null);
        }
    }

    public void releaseProjection() {
        Log.i(TAG, "releaseProjection");
        if (virtualDisplay != null) {
            virtualDisplay.release();
        }
        virtualDisplay = null;
    }

}
