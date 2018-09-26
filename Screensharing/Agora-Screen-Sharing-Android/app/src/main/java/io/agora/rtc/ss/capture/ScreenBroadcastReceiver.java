package io.agora.rtc.ss.capture;

import android.annotation.TargetApi;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.media.projection.MediaProjectionManager;
import android.os.Build;

/**
 * start media projection activity
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class ScreenBroadcastReceiver extends BroadcastReceiver {
    ScreenCapture mSender;

    public ScreenBroadcastReceiver(ScreenCapture sender) {
        mSender = sender;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent.getAction().equalsIgnoreCase(ScreenCapture.ASSISTANT_ACTIVITY_CREATED_INTENT)) {
            ScreenCapture screenCapture = mSender;
            if (ScreenCapture.mScreenCaptureActivity != null) {
                ScreenCapture.ScreenCaptureAssistantActivity screenActivity =
                        ScreenCapture.mScreenCaptureActivity;
                ScreenCapture.mScreenCaptureActivity.mScreenCapture = screenCapture;

                if (screenActivity.mScreenCapture.mMediaProjectManager == null) {
                    screenActivity.mScreenCapture.mMediaProjectManager =
                            (MediaProjectionManager) screenActivity.getSystemService(Context.MEDIA_PROJECTION_SERVICE);
                }

                screenActivity.startActivityForResult(
                        screenActivity.mScreenCapture.mMediaProjectManager.createScreenCaptureIntent(),
                        ScreenCapture.MEDIA_PROJECTION_REQUEST_CODE);
            }
        }

    }
}
