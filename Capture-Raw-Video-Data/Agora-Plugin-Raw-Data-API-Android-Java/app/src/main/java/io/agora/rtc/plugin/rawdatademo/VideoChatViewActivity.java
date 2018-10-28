package io.agora.rtc.plugin.rawdatademo;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.PorterDuff;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Toast;

import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.plugin.rawdata.MediaDataAudioObserver;
import io.agora.rtc.plugin.rawdata.MediaDataObserverPlugin;
import io.agora.rtc.plugin.rawdata.MediaDataVideoObserver;
import io.agora.rtc.plugin.rawdata.MediaPreProcessing;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class VideoChatViewActivity extends AppCompatActivity implements MediaDataAudioObserver, MediaDataVideoObserver {

    private static final String LOG_TAG = VideoChatViewActivity.class.getSimpleName();

    private static final int PERMISSION_REQ_ID_RECORD_AUDIO = 22;
    private static final int PERMISSION_REQ_ID_CAMERA = PERMISSION_REQ_ID_RECORD_AUDIO + 1;
    private static final int PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE = PERMISSION_REQ_ID_RECORD_AUDIO + 3;

    private MediaDataObserverPlugin mediaDataObserverPlugin;
    private int count = 0;

    private int mRemoteUid = 0;

    private RtcEngine mRtcEngine; // Tutorial Step 1
    private final IRtcEngineEventHandler mRtcEventHandler = new IRtcEngineEventHandler() { // Tutorial Step 1
        @Override
        public void onFirstRemoteVideoDecoded(final int uid, int width, int height, int elapsed) {

        }

        @Override
        public void onUserJoined(final int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(LOG_TAG, "onUserJoined " + (uid & 0xFFFFFFFFL));

            mRemoteUid = uid;

            runOnUiThread(new Runnable() {
                @Override
                public void run() {

                    if (mediaDataObserverPlugin != null) {
                        mediaDataObserverPlugin.addDecodeBuffer(uid, 1382400); // 720P
                    }
                }
            });
        }

        @Override
        public void onUserOffline(final int uid, int reason) {
            super.onUserOffline(uid, reason);

            if (mRemoteUid == uid) {
                mRemoteUid = 0;
            }

            runOnUiThread(new Runnable() {
                @Override
                public void run() {

                    if (mediaDataObserverPlugin != null) {
                        mediaDataObserverPlugin.removeDecodeBuffer(uid); // 720P
                    }
                }
            });
        }

    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_chat_view);

        Log.i(LOG_TAG, "agora sdk version: " + RtcEngine.getSdkVersion());

        if (checkSelfPermission(Manifest.permission.RECORD_AUDIO, PERMISSION_REQ_ID_RECORD_AUDIO)
                && checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA)
                && checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE)) {
            initAgoraEngineAndJoinChannel();
        }
    }

    private void initAgoraEngineAndJoinChannel() {
        initializeAgoraEngine();     // Tutorial Step 1
        setupVideoProfile();         // Tutorial Step 2
        setupLocalVideo();           // Tutorial Step 3
        joinChannel();               // Tutorial Step 4
    }

    public boolean checkSelfPermission(String permission, int requestCode) {
        Log.i(LOG_TAG, "checkSelfPermission " + permission + " " + requestCode);
        if (ContextCompat.checkSelfPermission(this,
                permission)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                    new String[]{permission},
                    requestCode);
            return false;
        }
        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String permissions[], @NonNull int[] grantResults) {
        Log.i(LOG_TAG, "onRequestPermissionsResult " + grantResults[0] + " " + requestCode);

        switch (requestCode) {
            case PERMISSION_REQ_ID_RECORD_AUDIO: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA);
                } else {
                    showLongToast("No permission for " + Manifest.permission.RECORD_AUDIO);
                    finish();
                }
                break;
            }
            case PERMISSION_REQ_ID_CAMERA: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE);
                } else {
                    showLongToast("No permission for " + Manifest.permission.CAMERA);
                    finish();
                }
                break;
            }
            case PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    initAgoraEngineAndJoinChannel();
                } else {
                    finish();
                }
                break;
            }
        }
    }

    public final void showLongToast(final String msg) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_LONG).show();
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        leaveChannel();
        RtcEngine.destroy();
        mRtcEngine = null;
        if (mediaDataObserverPlugin != null) {
            mediaDataObserverPlugin.removeAudioObserver(this);
            mediaDataObserverPlugin.removeVideoObserver(this);
            mediaDataObserverPlugin.removeAllBuffer();
        }
        MediaPreProcessing.releasePoint();
    }

    public void onLocalCaptureClicked(View view) {
        if (mediaDataObserverPlugin != null) {
            mediaDataObserverPlugin.saveCaptureVideoSnapshot("/sdcard/raw-data-test/capture" + count + ".jpg");
            Toast.makeText(this, "Picture saved success /sdcard/raw-data-test/capture" + count + ".jpg", Toast.LENGTH_SHORT).show();
            count++;
        }
    }

    public void onRemoteRenderClicked(View view) {
        if (mRemoteUid == 0) {
            return;
        }

        if (mediaDataObserverPlugin != null) {
            mediaDataObserverPlugin.saveRenderVideoSnapshot("/sdcard/raw-data-test/render" + count + ".jpg", mRemoteUid);
            Toast.makeText(this, "Picture saved success /sdcard/raw-data-test/render" + count + ".jpg", Toast.LENGTH_SHORT).show();
            count++;
        }
    }

    // Tutorial Step 9
    public void onLocalAudioMuteClicked(View view) {
        ImageView iv = (ImageView) view;
        if (iv.isSelected()) {
            iv.setSelected(false);
            iv.clearColorFilter();
        } else {
            iv.setSelected(true);
            iv.setColorFilter(getResources().getColor(R.color.colorPrimary), PorterDuff.Mode.MULTIPLY);
        }

        mRtcEngine.muteLocalAudioStream(iv.isSelected());
    }

    // Tutorial Step 5
    public void onSwitchCameraClicked(View view) {
        mRtcEngine.switchCamera();
    }

    // Tutorial Step 6
    public void onEncCallClicked(View view) {
        finish();
    }

    // Tutorial Step 1
    private void initializeAgoraEngine() {
        try {
            mRtcEngine = RtcEngine.create(getBaseContext(), getString(R.string.agora_app_id), mRtcEventHandler);
        } catch (Exception e) {
            Log.e(LOG_TAG, Log.getStackTraceString(e));

            throw new RuntimeException("NEED TO check rtc sdk init fatal error\n" + Log.getStackTraceString(e));
        }
        mRtcEngine.setLogFile("/sdcard/agora-rtc-raw-data-plugin.log");

        mRtcEngine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);

        mediaDataObserverPlugin = MediaDataObserverPlugin.the();
        MediaPreProcessing.setCallback(mediaDataObserverPlugin);
        MediaPreProcessing.setVideoCaptureByteBuffer(mediaDataObserverPlugin.byteBufferCapture);
        MediaPreProcessing.setAudioRecordByteBuffer(mediaDataObserverPlugin.byteBufferAudioRecord);
        MediaPreProcessing.setAudioPlayByteBuffer(mediaDataObserverPlugin.byteBufferAudioPlay);
        MediaPreProcessing.setBeforeAudioMixByteBuffer(mediaDataObserverPlugin.byteBufferBeforeAudioMix);
        MediaPreProcessing.setAudioMixByteBuffer(mediaDataObserverPlugin.byteBufferAudioMix);

        mediaDataObserverPlugin.addVideoObserver(this);
        mediaDataObserverPlugin.addAudioObserver(this);
    }

    // Tutorial Step 2
    private void setupVideoProfile() {
        mRtcEngine.enableVideo();
        // mRtcEngine.setVideoProfile(Constants.VIDEO_PROFILE_240P, false); // SDK version

        VideoEncoderConfiguration.VideoDimensions dimensions = VideoEncoderConfiguration.VD_320x240;
        VideoEncoderConfiguration configuration = new VideoEncoderConfiguration(dimensions,
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE);
        mRtcEngine.setVideoEncoderConfiguration(configuration);
    }

    // Tutorial Step 3
    private void setupLocalVideo() {
        FrameLayout container = (FrameLayout) findViewById(R.id.local_video_view_container);
        SurfaceView surfaceView = RtcEngine.CreateRendererView(getBaseContext());
        surfaceView.setZOrderMediaOverlay(true);
        container.addView(surfaceView);
        mRtcEngine.setupLocalVideo(new VideoCanvas(surfaceView, VideoCanvas.RENDER_MODE_FIT, 0));
    }

    // Tutorial Step 4
    private void joinChannel() {
        mRtcEngine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        mRtcEngine.joinChannel(null, "rawdatademo1", "Extra Optional Data", 0); // if you do not specify the uid, we will generate the uid for you
    }


    // Tutorial Step 7
    private void leaveChannel() {
        mRtcEngine.leaveChannel();
    }


    @Override
    public void onCaptureVideoFrame(byte[] data, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {
        Log.i(LOG_TAG, "onCaptureVideoFrame width: " + width + " height: " + height);
    }

    @Override
    public void onRenderVideoFrame(int uid, byte[] data, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {
        Log.i(LOG_TAG, "onRenderVideoFrame width: " + width + " height: " + height + " uid: " + (uid & 0xFFFFFFFFL));
    }

    @Override
    public void onRecordAudioFrame(byte[] data, int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        Log.i(LOG_TAG, "onRecordAudioFrame samples: " + samples + " bytesPerSample: " + bytesPerSample + " channels: " + channels + " samplesPerSec: " + samplesPerSec);
    }

    @Override
    public void onPlaybackAudioFrame(byte[] data, int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        Log.i(LOG_TAG, "onPlaybackAudioFrame samples: " + samples + " bytesPerSample: " + bytesPerSample + " channels: " + channels + " samplesPerSec: " + samplesPerSec + " bufferLength: " + bufferLength);
    }

    @Override
    public void onPlaybackAudioFrameBeforeMixing(int uid, byte[] data, int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        Log.i(LOG_TAG, "onPlaybackAudioFrameBeforeMixing samples: " + samples + " bytesPerSample: " + bytesPerSample + " channels: " + channels + " samplesPerSec: " + samplesPerSec + " bufferLength: " + bufferLength + " uid: " + (uid & 0xFFFFFFFFL));
    }

    @Override
    public void onMixedAudioFrame(byte[] data, int audioType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {
        Log.i(LOG_TAG, "onMixedAudioFrame samples: " + samples + " bytesPerSample: " + bytesPerSample + " channels: " + channels + " samplesPerSec: " + samplesPerSec + " bufferLength: " + bufferLength);
    }

}
