package io.agora.rtc.ss;

import android.annotation.TargetApi;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import io.agora.rtc.ss.aidl.INotification;
import io.agora.rtc.ss.aidl.IScreenShare;
import io.agora.rtc.ss.capture.ScreenSharingService;
import io.agora.rtc.video.VideoEncoderConfiguration;

public class ScreenShare {
    private static final String TAG = ScreenShare.class.getSimpleName();
    private static IScreenShare mScreenShareSvc;
    private IStateListener mListener;
    private static volatile ScreenShare mInstance;

    private ScreenShare() {}

    public static ScreenShare getInstance() {
        if (mInstance == null) {
            synchronized (ScreenShare.class) {
                if (mInstance == null) {
                    mInstance = new ScreenShare();
                }
            }
        }

        return mInstance;
    }

    private ServiceConnection mScreenShareConn = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            mScreenShareSvc = IScreenShare.Stub.asInterface(service);

            try {
                mScreenShareSvc.registerCallback(mNotification);
                mScreenShareSvc.startShare();
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            }

        }
        public void onServiceDisconnected(ComponentName className) {
            mScreenShareSvc = null;
        }
    };

    private INotification mNotification = new INotification.Stub() {
        /**
         * This is called by the remote service to tell us about error happened.
         * Note that IPC calls are dispatched through a thread
         * pool running in each process, so the code executing here will
         * NOT be running in our main thread like most other things -- so,
         * if to update the UI, we need to use a Handler to hop over there.
         */
        public void onError(int error) {
            Log.e(TAG, "screen share service error happened: " + error);
            mListener.onError(error);
        }

        public void onTokenWillExpire() {
            Log.d(TAG, "screen share service token will expire");
            mListener.onTokenWillExpire();
        }
    };

    @TargetApi(21)
    public void start(Context context, String appId, String token, String channelName, int uid, VideoEncoderConfiguration vec) {
        if (mScreenShareSvc == null) {
            Intent intent = new Intent(context, ScreenSharingService.class);
            intent.putExtra(Constant.APP_ID, appId);
            intent.putExtra(Constant.TOKEN, token);
            intent.putExtra(Constant.CHANNEL_NAME, channelName);
            intent.putExtra(Constant.UID, uid);
            intent.putExtra(Constant.WIDTH, vec.dimensions.width);
            intent.putExtra(Constant.HEIGHT, vec.dimensions.height);
            intent.putExtra(Constant.FRAME_RATE, vec.frameRate);
            intent.putExtra(Constant.BIT_RATE, vec.bitrate);
            intent.putExtra(Constant.ORIENTATION_MODE, vec.orientationMode.getValue());
            context.bindService(intent, mScreenShareConn, Context.BIND_AUTO_CREATE);
        } else {
            try {
                mScreenShareSvc.startShare();
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            }
        }

    }

    @TargetApi(21)
    public void stop(Context context) {
        if (mScreenShareSvc != null) {
            try {
                mScreenShareSvc.stopShare();
                mScreenShareSvc.unregisterCallback(mNotification);
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            } finally {
                mScreenShareSvc = null;
            }
        }
        context.unbindService(mScreenShareConn);
    }

    @TargetApi(21)
    public void renewToken(String token) {
        if (mScreenShareSvc != null) {
            try {
                mScreenShareSvc.renewToken(token);
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.e(TAG, Log.getStackTraceString(e));
            }
        } else {
            Log.e(TAG, "screen share service not exist");
        }
    }

    @TargetApi(21)
    public void setListener(IStateListener listener) {
        mListener = listener;
    }

    public interface IStateListener {
        void onError(int error);
        void onTokenWillExpire();
    }
}
