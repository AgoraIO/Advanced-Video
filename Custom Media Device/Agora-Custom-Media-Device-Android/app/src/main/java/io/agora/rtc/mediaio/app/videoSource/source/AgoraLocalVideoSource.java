package io.agora.rtc.mediaio.app.videoSource.source;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;

import io.agora.rtc.mediaio.TextureSource;

import static android.os.SystemClock.elapsedRealtime;
import static io.agora.rtc.mediaio.MediaIO.PixelFormat.TEXTURE_OES;

/**
 * Created by keke on 2018/1/11.
 */

public class AgoraLocalVideoSource extends TextureSource {
    private static final String TAG = "AgoraLocalVideoSource";
    private String videoPath = null;
    private Context mContext;
    private VideoDecoder mVideoDecoder;

    public AgoraLocalVideoSource(Context context, int width, int height, String videoPath) {
        super(null, width, height);
        this.videoPath = videoPath;
        mContext = context;
        Log.i(TAG, "AgoraLocalVideoSource");
    }

    @Override
    public void onTextureFrameAvailable(int oesTextureId, float[] transformMatrix, long timestampNs) {
        Log.i(TAG, "onTextureFrameAvailable, oesTextureId = " + oesTextureId + " ts = " + timestampNs);

        timestampNs = elapsedRealtime();

        super.onTextureFrameAvailable(oesTextureId, transformMatrix, timestampNs);

        int rotation = 0;

        if (mConsumer != null && mConsumer.get() != null) {
            mConsumer.get().consumeTextureFrame(oesTextureId, TEXTURE_OES.intValue(), mWidth, mHeight, rotation, timestampNs, transformMatrix);
        }


    }

    @Override
    protected boolean onCapturerOpened() {
        Log.i(TAG, "onCapturerOpened");
        try {
            mVideoDecoder = new VideoDecoder();
            mVideoDecoder.setDataSource("file://" + this.videoPath);
            mVideoDecoder.setVideoSurface(new Surface(getSurfaceTexture()));
        } catch (IOException ie) {
            ie.printStackTrace();
            return false;
        } catch (RuntimeException re) {
            re.printStackTrace();
            return false;
        }
        return true;
    }

    @Override
    protected boolean onCapturerStarted() {
        Log.i(TAG, "onCapturerStarted");
        mVideoDecoder.startAsync();
        return true;
    }

    @Override
    protected void onCapturerStopped() {
        Log.i(TAG, "onCapturerStopped");
        mVideoDecoder.stop();
    }

    @Override
    protected void onCapturerClosed() {
        Log.i(TAG, "onCapturerClosed");
        mVideoDecoder.stop();
    }

    // video decoder
    public class VideoDecoder {

        // MediaExtractor related
        private boolean eos = false;
        private MediaExtractor mExtractor;
        private String mFilePath;
        private MediaFormat mMediaFormat;
        private int mVideoTrackIndex = -1;
        private String mimeType;
        private int mVideoWidth;
        private int mVideoHeight;

        // MediaCodec related
        private MediaCodec mDecoder;
        private Surface mSurface;

        private Thread playBackThread;


        public VideoDecoder() {
            Log.e(TAG, "VideoDecoder");
        }

        protected void finalize() {
            Log.i(TAG, "finalize");
            release();
        }

        public void setDataSource(String filePath) throws IOException, RuntimeException {

            mFilePath = new String(filePath);

            try {
                mExtractor = new MediaExtractor();

                mExtractor.setDataSource(mFilePath);

                // select first video track we find
                int nTracks = mExtractor.getTrackCount();
                for (int i = 0; i < nTracks; i++) {
                    mMediaFormat = mExtractor.getTrackFormat(i);
                    mimeType = mMediaFormat.getString(MediaFormat.KEY_MIME);
                    if (mimeType.startsWith("video/")) {
                        Log.d(TAG, "Extractor selected track " + i + " (" + mimeType + ")" + mMediaFormat);
                        mVideoTrackIndex = i;
                        break;
                    }
                }
                if (mVideoTrackIndex == -1) {
                    throw new RuntimeException("No video track found");
                }
                mExtractor.selectTrack(mVideoTrackIndex);

                mVideoWidth = mMediaFormat.getInteger(MediaFormat.KEY_WIDTH);
                mVideoHeight = mMediaFormat.getInteger(MediaFormat.KEY_HEIGHT);
                Log.e(TAG, "mVideoWidth = " + mVideoWidth + " mVideoHeight = " + mVideoHeight);

                mDecoder = MediaCodec.createDecoderByType(mimeType);
                if (mDecoder == null) {
                    Log.e(TAG, "can't create mediaCodec");
                    throw new RuntimeException("can't create mediCodec");
                }
            } catch (IOException ie) {
                ie.printStackTrace();
                throw ie;
            }
        }

        public void setVideoSurface(Surface surface) {
            Log.i(TAG, "setVideoSurface");
            mSurface = surface;
            mDecoder.configure(mMediaFormat, mSurface, null, 0);
            mDecoder.start();
        }

        public void start() {
            Log.i(TAG, "mediaSource start");
            eos = false;
            playVideo();
        }

        public void startAsync() {
            Log.i(TAG, "mediaSource startAsync");
            playBackThread = new Thread(new VideoDecoder.VideoDecodeThread());
            playBackThread.start();
        }

        public void stop() {
            Log.i(TAG, "stop()");
            eos = true;
            release();
        }

        public void release() {
            Log.i(TAG, "release()");
            if (playBackThread != null && playBackThread.isAlive()) {
                playBackThread.interrupt();
                while (playBackThread.isAlive()) Log.e(TAG, "release, playThread still Alive");
                playBackThread = null;
            }

            if (mDecoder != null) {
                mDecoder.release();
                mDecoder = null;
            }

            if (mExtractor != null) {
                mExtractor.release();
                mExtractor = null;
            }

        }

        private class VideoDecodeThread implements Runnable {
            public void run() {
                eos = false;
                playVideo();
            }
        }

        private void playVideo() {

            final int NO_BUFFER_INDEX = -1;
            final long TS_TOO_EARLY_WARN = 300; // if early than 300ms, give a waring
            final long TS_TOO_LATE_DROP = 200; // if later than 200ms, drop this frame
            boolean signaledEos = false;
            int outputBufferIndex = NO_BUFFER_INDEX;
            int inputBufferIndex = NO_BUFFER_INDEX;
            long timestampUs = 0;
            long nowPts = 0;

            try {

                SimpleSyncObj syncObj = new SimpleSyncObj();

                MediaCodec.BufferInfo outputBufferInfo = new MediaCodec.BufferInfo();

                while (!eos && !Thread.interrupted()) {
                    //get data from extractor and feed into decoder
                    if (!signaledEos) {
                        inputBufferIndex = mDecoder.dequeueInputBuffer(0);
                        int size = 0;
                        timestampUs = 0;
                        if (inputBufferIndex != NO_BUFFER_INDEX) {
                            ByteBuffer[] buffer = mDecoder.getInputBuffers();
                            size = mExtractor.readSampleData(buffer[inputBufferIndex], 0);
                            timestampUs = mExtractor.getSampleTime();
                            mExtractor.advance();
                            signaledEos = (size == -1);
                            mDecoder.queueInputBuffer(inputBufferIndex,
                                    0,
                                    size,
                                    timestampUs,
                                    signaledEos ? MediaCodec.BUFFER_FLAG_END_OF_STREAM : 0);
                        }
//                        Log.d(TAG, "inputBufferIndex = " + inputBufferIndex + " size = " +
//                                size + " ts = " + timestampUs);
                    }

                    // If don't have an output buffer, try to get one now
                    if (outputBufferIndex == NO_BUFFER_INDEX) {
                        outputBufferIndex = mDecoder.dequeueOutputBuffer(outputBufferInfo, 0);
                    }

                    if (outputBufferIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED
                            || outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED
                            || outputBufferIndex == MediaCodec.INFO_TRY_AGAIN_LATER) {
                        outputBufferIndex = NO_BUFFER_INDEX;
                    } else if (outputBufferIndex != NO_BUFFER_INDEX) {
                        eos = (outputBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0;

//                        Log.d(TAG, "outputBufferIndex = " + outputBufferIndex + " pts = " +
//                                outputBufferInfo.presentationTimeUs);

                        // here do video sync
                        if (syncObj.needUpdateSyncTime()) {
                            syncObj.updateSyncTimeMs(outputBufferInfo.presentationTimeUs / 1000);
                        }
                        nowPts = syncObj.getCurrentRelativeTimeMs();
                        if (outputBufferInfo.presentationTimeUs / 1000 > nowPts) {
                            long waitTimeMs = outputBufferInfo.presentationTimeUs / 1000 - nowPts;
                            if (waitTimeMs > TS_TOO_EARLY_WARN) {
                                Log.w(TAG, "time too early by " + waitTimeMs);
                            }
                            Thread.sleep(waitTimeMs);
                        } else if (nowPts - outputBufferInfo.presentationTimeUs / 1000 > TS_TOO_LATE_DROP) {
                            Log.w(TAG, "video too late, drop this frame, ts = " +
                                    outputBufferInfo.presentationTimeUs + " Us" + "nowPts = " + nowPts + " Ms");
                            continue;
                        }
                        // render the output buffer to output surface;
                        boolean render = outputBufferInfo.size > 0;
                        mDecoder.releaseOutputBuffer(outputBufferIndex, render);
                        outputBufferIndex = NO_BUFFER_INDEX;
                    }
                }
            } catch (InterruptedException ie) {
                eos = true;
                ie.printStackTrace();
                Log.d(TAG, "interrupt exception");
            } catch (IllegalStateException ile) {
                eos = true;
                ile.printStackTrace();
                Log.d(TAG, "not in Executing state");
            }

            Log.d(TAG, "decoder finish");
        }

    }

    public class SimpleSyncObj {
        private long timeLineStartMs;
        private long timeStampStartMs;
        private boolean needUpdateSync = true;

        public SimpleSyncObj() {
            Log.i(TAG, "SimpleSyncObj");
        }

        public boolean needUpdateSyncTime() {
            if (needUpdateSync) {
                needUpdateSync = false;
                return true;
            }
            return needUpdateSync;
        }

        public void updateSyncTimeMs(long timeMs) {
            timeStampStartMs = timeMs;
            timeLineStartMs = elapsedRealtime();
//            Log.d(TAG, "timeStart = " + timeStampStartMs + " timeLineStartUs = " + timeLineStartMs);
        }

        public long getCurrentRelativeTimeMs() {
            return elapsedRealtime() - timeLineStartMs + timeStampStartMs;
        }
    }
}
