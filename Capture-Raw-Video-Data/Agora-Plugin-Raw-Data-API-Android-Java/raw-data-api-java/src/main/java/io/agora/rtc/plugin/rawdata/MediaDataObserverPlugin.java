package io.agora.rtc.plugin.rawdata;

import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * Created by yt on 2018/2/1/001.
 */
public class MediaDataObserverPlugin implements MediaPreProcessing.ProgressCallback {

    private final List<MediaDataVideoObserver> videoObserverList = new ArrayList<>();
    private final List<MediaDataAudioObserver> audioObserverList = new ArrayList<>();

    private static final int VIDEO_DEFAULT_BUFFER_SIZE = 1920 * 720; // 720P
    private static final int AUDIO_DEFAULT_BUFFER_SIZE = 2048;

    public ByteBuffer byteBufferCapture = ByteBuffer.allocateDirect(VIDEO_DEFAULT_BUFFER_SIZE);
    public ByteBuffer byteBufferRender = ByteBuffer.allocateDirect(VIDEO_DEFAULT_BUFFER_SIZE);
    public ByteBuffer byteBufferAudioRecord = ByteBuffer.allocateDirect(AUDIO_DEFAULT_BUFFER_SIZE);
    public ByteBuffer byteBufferAudioPlay = ByteBuffer.allocateDirect(AUDIO_DEFAULT_BUFFER_SIZE);
    public ByteBuffer byteBufferBeforeAudioMix = ByteBuffer.allocateDirect(AUDIO_DEFAULT_BUFFER_SIZE);
    public ByteBuffer byteBufferAudioMix = ByteBuffer.allocateDirect(AUDIO_DEFAULT_BUFFER_SIZE);

    private final List<DecodeDataBuffer> decodeBufferList = new ArrayList<>();

    private static MediaDataObserverPlugin myAgent = null;

    private boolean beCaptureVideoShot = false;
    private boolean beRenderVideoShot = false;
    private String captureFilePath = null;
    private String renderFilePath = null;
    private int renderVideoShotUid;

    public static MediaDataObserverPlugin the() {
        if (myAgent == null) {
            synchronized (MediaDataObserverPlugin.class) {
                if (myAgent == null)
                    myAgent = new MediaDataObserverPlugin();
            }
        }
        return myAgent;
    }

    public void addVideoObserver(MediaDataVideoObserver observer) {
        videoObserverList.add(observer);
    }

    public void removeVideoObserver(MediaDataVideoObserver observer) {
        videoObserverList.remove(observer);
    }

    public void addAudioObserver(MediaDataAudioObserver observer) {
        audioObserverList.add(observer);
    }

    public void removeAudioObserver(MediaDataAudioObserver observer) {
        audioObserverList.remove(observer);
    }

    public void saveCaptureVideoSnapshot(String filePath) {
        beCaptureVideoShot = true;
        captureFilePath = filePath;
    }

    public void saveRenderVideoSnapshot(String filePath, int uid) {
        beRenderVideoShot = true;
        renderFilePath = filePath;
        renderVideoShotUid = uid;
    }

    public void addDecodeBuffer(int uid, int bufferLength) {
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(bufferLength); // 720P
        decodeBufferList.add(new DecodeDataBuffer(uid, byteBuffer));
        MediaPreProcessing.setVideoDecodeByteBuffer(uid, byteBuffer);
    }

    public void removeDecodeBuffer(int uid) {
        int index = getDecodeIndex(uid);
        if (index != -1) {
            decodeBufferList.remove(index);
        }

        MediaPreProcessing.setVideoDecodeByteBuffer(uid, null);
    }

    private int getDecodeIndex(int uid) {
        for (int i = 0; i < decodeBufferList.size(); i++) {
            if (decodeBufferList.get(i).getUid() == uid) {
                return i;
            }
        }
        return -1;
    }

    public void removeAllBuffer() {
        decodeBufferList.removeAll(decodeBufferList);
        releaseBuffer();
    }

    @Override
    public void onCaptureVideoFrame(int videoFrameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {

        byte[] buf = new byte[bufferLength];
        byteBufferCapture.limit(bufferLength);
        byteBufferCapture.get(buf);
        byteBufferCapture.flip();

        for (MediaDataVideoObserver observer : videoObserverList) {
            observer.onCaptureVideoFrame(buf, videoFrameType, width, height, bufferLength, yStride, uStride, vStride, rotation, renderTimeMs);
        }

        byteBufferCapture.put(buf);
        byteBufferCapture.flip();

        if (beCaptureVideoShot) {
            beCaptureVideoShot = false;

            getVideoSnapshot(width, height, bufferLength, buf, captureFilePath);
        }
    }

    @Override
    public void onRenderVideoFrame(int uid, int videoFrameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {

        for (MediaDataVideoObserver observer : videoObserverList) {
            Iterator<DecodeDataBuffer> it = decodeBufferList.iterator();
            while (it.hasNext()) {
                DecodeDataBuffer tmp = it.next();
                if (tmp.getUid() == uid) {
                    byte[] buf = new byte[bufferLength];
                    tmp.getByteBuffer().limit(bufferLength);
                    tmp.getByteBuffer().get(buf);
                    tmp.getByteBuffer().flip();

                    observer.onRenderVideoFrame(uid, buf, videoFrameType, width, height, bufferLength, yStride, uStride, vStride, rotation, renderTimeMs);

                    tmp.getByteBuffer().put(buf);
                    tmp.getByteBuffer().flip();

                    if (beRenderVideoShot) {
                        if (uid == renderVideoShotUid) {
                            beRenderVideoShot = false;

                            getVideoSnapshot(width, height, bufferLength, buf, renderFilePath);
                        }
                    }
                }
            }
        }
    }

    @Override
    public void onRecordAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferAudioRecord.limit(bufferLength);
        byteBufferAudioRecord.get(buf);
        byteBufferAudioRecord.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onRecordAudioFrame(buf, audioFrameType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }

        byteBufferAudioRecord.put(buf);
        byteBufferAudioRecord.flip();
    }

    @Override
    public void onPlaybackAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferAudioPlay.limit(bufferLength);
        byteBufferAudioPlay.get(buf);
        byteBufferAudioPlay.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onPlaybackAudioFrame(buf, audioFrameType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }

        byteBufferAudioPlay.put(buf);
        byteBufferAudioPlay.flip();
    }

    @Override
    public void onPlaybackAudioFrameBeforeMixing(int uid, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferBeforeAudioMix.limit(bufferLength);
        byteBufferBeforeAudioMix.get(buf);
        byteBufferBeforeAudioMix.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onPlaybackAudioFrameBeforeMixing(uid, buf, audioFrameType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }

        byteBufferBeforeAudioMix.put(buf);
        byteBufferBeforeAudioMix.flip();
    }

    @Override
    public void onMixedAudioFrame(int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferAudioMix.limit(bufferLength);
        byteBufferAudioMix.get(buf);
        byteBufferAudioMix.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onMixedAudioFrame(buf, audioFrameType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }

        byteBufferAudioMix.put(buf);
        byteBufferAudioMix.flip();
    }

    private void getVideoSnapshot(int width, int height, int bufferLength, byte[] buffer, String filePath) {
        File file = new File(filePath);

        byte[] NV21 = new byte[bufferLength];
        swapYV12toYUV420SemiPlanar(buffer, NV21, width, height);

        YuvImage image = new YuvImage(NV21, ImageFormat.NV21, width, height, null);

        File fileParent = file.getParentFile();
        if (!fileParent.exists()) {
            fileParent.mkdirs();
        }
        if (file.exists()) {
            file.delete();
        }

        try {
            file.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }

        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(file);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        image.compressToJpeg(
                new Rect(0, 0, image.getWidth(), image.getHeight()),
                70, fos);
    }

    private void swapYV12toYUV420SemiPlanar(byte[] yv12bytes, byte[] i420bytes, int width, int height) {
        System.arraycopy(yv12bytes, 0, i420bytes, 0, width * height);
        int startPos = width * height;
        int yv_start_pos_u = startPos;
        int yv_start_pos_v = startPos + startPos / 4;
        for (int i = 0; i < startPos / 4; i++) {
            i420bytes[startPos + 2 * i + 0] = yv12bytes[yv_start_pos_v + i];
            i420bytes[startPos + 2 * i + 1] = yv12bytes[yv_start_pos_u + i];
        }
    }

    public void releaseBuffer() {
        byteBufferCapture.clear();
        byteBufferRender.clear();
        byteBufferAudioRecord.clear();
        byteBufferAudioPlay.clear();
        byteBufferBeforeAudioMix.clear();
        byteBufferAudioMix.clear();
    }

}
