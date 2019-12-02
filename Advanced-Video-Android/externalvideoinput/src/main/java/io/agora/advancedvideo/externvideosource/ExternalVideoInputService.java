package io.agora.advancedvideo.externvideosource;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;

import androidx.annotation.Nullable;

import io.agora.advancedvideo.AgoraApplication;

public class ExternalVideoInputService extends Service {
    private ExternalVideoInputManager mSourceManager;
    private IExternalVideoInputService mService;

    @Override
    public void onCreate() {
        super.onCreate();
        mSourceManager = new ExternalVideoInputManager((AgoraApplication) getApplication());
        mService = new IExternalVideoInputService.Stub() {
            @Override
            public void setExternalVideoInput(int type, Intent intent) throws RemoteException {
                mSourceManager.setExternalVideoInput(type, intent);
            }
        };
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        startSourceManager();
        return mService.asBinder();
    }

    private void startSourceManager() {
        mSourceManager.start();
    }

    @Override
    public boolean onUnbind(Intent intent) {
        stopSourceManager();
        return true;
    }

    private void stopSourceManager() {
        if (mSourceManager != null) {
            mSourceManager.stop();
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
