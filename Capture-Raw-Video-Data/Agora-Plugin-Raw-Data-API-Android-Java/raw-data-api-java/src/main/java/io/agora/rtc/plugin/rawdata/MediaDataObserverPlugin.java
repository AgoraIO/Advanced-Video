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
import java.util.List;


/**
 * Created by yt on 2018/2/1/001.
 */

public class MediaDataObserverPlugin implements MediaPreProcessing.ProgressCallback {

    private List<MediaDataVideoObserver> videoObserverList = new ArrayList<>();
    private List<MediaDataAudioObserver> audioObserverList = new ArrayList<>();
    public ByteBuffer byteBufferCapture = ByteBuffer.allocateDirect(1382400);//720P
    public ByteBuffer byteBufferRender = ByteBuffer.allocateDirect(1382400);
    public ByteBuffer byteBufferAudioRecord = ByteBuffer.allocateDirect(2048);
    public ByteBuffer byteBufferAudioPlay = ByteBuffer.allocateDirect(2048);
    public ByteBuffer byteBufferBeforeAudioMix = ByteBuffer.allocateDirect(2048);
    public ByteBuffer byteBufferAudioMix = ByteBuffer.allocateDirect(2048);
    private List<DecodeDataBuffer> decodeBufferList = new ArrayList<>();

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

    public void saveCaptureVideoShot(String filePath) {
        beCaptureVideoShot = true;
        captureFilePath = filePath;
    }

    public void saveRenderVideoShot(String filePath, int uid) {
        beRenderVideoShot = true;
        renderFilePath = filePath;
        renderVideoShotUid = uid;
    }

    public void addDecodeBuffer(int uid, int bufferLength) {
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(bufferLength);//720P
        decodeBufferList.add(new DecodeDataBuffer(uid, byteBuffer));
        MediaPreProcessing.setVideoDecodeByteBUffer(uid, byteBuffer);
    }

    public void removeDecodeBuffer(int uid) {
        int index = getDecodeIndex(uid);
        if (index != -1) {
            decodeBufferList.remove(index);
        }

        MediaPreProcessing.setVideoDecodeByteBUffer(uid, null);
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
    public void onCaptureVideoFrame(int uid, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {

        byte[] buf = new byte[bufferLength];
        byteBufferCapture.get(buf);
        byteBufferCapture.flip();

        for (MediaDataVideoObserver observer : videoObserverList) {
            observer.onCaptureVideoFrame(buf, frameType, width, height, bufferLength, yStride, uStride, vStride, rotation, renderTimeMs);
        }
        if (beCaptureVideoShot) {
            beCaptureVideoShot = false;

            getVideoShot(width, height, bufferLength, buf, captureFilePath);

        }
    }

    @Override
    public void onRenderVideoFrame(int uid, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {

        for (MediaDataVideoObserver observer : videoObserverList) {
            for (int i = 0; i < decodeBufferList.size(); i++) {
                if (decodeBufferList.get(i).getUid() == uid) {

                    byte[] buf = new byte[bufferLength];
                    decodeBufferList.get(i).getByteBuffer().get(buf);
                    decodeBufferList.get(i).getByteBuffer().flip();

                    observer.onRenderVideoFrame(uid, buf, frameType, width, height, bufferLength, yStride, uStride, vStride, rotation, renderTimeMs);

                    if (beRenderVideoShot) {
                        if (uid == renderVideoShotUid) {
                            beRenderVideoShot = false;

                            getVideoShot(width, height, bufferLength, buf, renderFilePath);
                        }
                    }
                }

            }
        }
    }

    @Override
    public void onRecordAudioFrame(int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferAudioRecord.get(buf);
        byteBufferAudioRecord.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onRecordAudioFrame(buf, audioType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }
    }

    @Override
    public void onPlaybackAudioFrame(int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferAudioPlay.get(buf);
        byteBufferAudioPlay.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onPlaybackAudioFrame(buf, audioType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }
    }

    @Override
    public void onPlaybackAudioFrameBeforeMixing(int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferBeforeAudioMix.get(buf);
        byteBufferBeforeAudioMix.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onPlaybackAudioFrameBeforeMixing(buf, audioType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }
    }

    @Override
    public void onMixedAudioFrame(int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        byte[] buf = new byte[bufferLength];
        byteBufferAudioMix.get(buf);
        byteBufferAudioMix.flip();

        for (MediaDataAudioObserver observer : audioObserverList) {
            observer.onMixedAudioFrame(buf, audioType, samples, bytesPerSample, channels, samplesPerSec, renderTimeMs, bufferLength);
        }
    }

    private void getVideoShot(int width, int height, int bufferLength, byte[] buffer, String filePath) {
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
