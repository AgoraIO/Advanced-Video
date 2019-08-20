package io.agora.switchlive.ui;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import io.agora.rtc.RtcEngine;
import io.agora.switchlive.AGApplication;
import io.agora.switchlive.rtc.AGEventHandler;
import io.agora.switchlive.rtc.EngineConfig;
import io.agora.switchlive.rtc.MyEngineEventHandler;
import io.agora.switchlive.rtc.WorkerThread;

public abstract class BaseActivity extends AppCompatActivity implements AGEventHandler {
    private static final int PERMISSION_REQUEST_CODE = 0X0001;

    private String[] permissions = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.CAMERA
    };

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        if (checkSelfPermissions()) {
            ((AGApplication) getApplication()).initWorkerThread();
        }
    }

    private boolean checkSelfPermissions() {
        List<String> needList = new ArrayList<>();
        for (String perm : permissions) {
            if (ContextCompat.checkSelfPermission(this, perm) != PackageManager.PERMISSION_GRANTED) {
                needList.add(perm);
            }
        }

        if (!needList.isEmpty()) {
            ActivityCompat.requestPermissions(this, needList.toArray(new String[needList.size()]), PERMISSION_REQUEST_CODE);
            return false;
        }

        return true;
    }

    protected RtcEngine rtcEngine() {
        return ((AGApplication) getApplication()).getWorkerThread().getRtcEngine();
    }

    protected final WorkerThread worker() {
        return ((AGApplication) getApplication()).getWorkerThread();
    }

    protected final EngineConfig config() {
        return ((AGApplication) getApplication()).getWorkerThread().getEngineConfig();
    }

    protected final MyEngineEventHandler event() {
        return ((AGApplication) getApplication()).getWorkerThread().eventHandler();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String permissions[], @NonNull int[] grantResults) {
        Log.i("SwitchLive","onRequestPermissionsResult " + requestCode + " " + Arrays.toString(permissions) + " " + Arrays.toString(grantResults));

        if (requestCode == PERMISSION_REQUEST_CODE) {
            int deniedCount = 0;

            for (int i = 0; i < grantResults.length; i++) {
                if (grantResults[i] == PackageManager.PERMISSION_DENIED) {
                    deniedCount++;
                }
            }

            if (deniedCount == 0) {
                ((AGApplication) getApplication()).initWorkerThread();
            } else {
                Log.e("SwitchLive", "permission denied");
                finish();
            }
        }
    }

    @Override
    public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {

    }

    @Override
    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {

    }

    @Override
    public void onUserOffline(int uid, int reason) {

    }

    @Override
    public void onUserJoined(int uid, int elapsed) {

    }

    @Override
    public void onLastmileQuality(final int quality) {

    }
}
