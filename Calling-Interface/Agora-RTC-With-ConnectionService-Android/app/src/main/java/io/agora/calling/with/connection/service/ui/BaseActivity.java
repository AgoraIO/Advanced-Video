package io.agora.calling.with.connection.service.ui;


import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.Toast;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Arrays;

import io.agora.calling.with.connection.service.AGApplication;
import io.agora.calling.with.connection.service.BuildConfig;
import io.agora.calling.with.connection.service.model.ConstantApp;
import io.agora.calling.with.connection.service.model.EngineConfig;
import io.agora.calling.with.connection.service.model.MyEngineEventHandler;
import io.agora.calling.with.connection.service.model.WorkerThread;
import io.agora.rtc.RtcEngine;

import static io.agora.calling.with.connection.service.model.AGEventHandler.EVENT_TYPE_ON_RELOGIN_NEEDED;

public abstract class BaseActivity extends AppCompatActivity {
    private final static Logger log = LoggerFactory.getLogger(BaseActivity.class);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final View layout = findViewById(Window.ID_ANDROID_CONTENT);
        ViewTreeObserver vto = layout.getViewTreeObserver();
        vto.addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                    layout.getViewTreeObserver().removeOnGlobalLayoutListener(this);
                } else {
                    layout.getViewTreeObserver().removeGlobalOnLayoutListener(this);
                }
                initUIandEvent();
            }
        });
    }

    protected abstract void initUIandEvent();

    protected abstract void deInitUIandEvent();

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (isFinishing()) {
                    return;
                }

                boolean checkPermissionResult = checkSelfPermissions();

                if ((Build.VERSION.SDK_INT < Build.VERSION_CODES.M)) {
                    // so far we do not use OnRequestPermissionsResultCallback
                }
            }
        }, 500);
    }

    private boolean checkSelfPermissions() {
        return checkSelfPermission(Manifest.permission.RECORD_AUDIO, ConstantApp.PERMISSION_REQ_ID_RECORD_AUDIO) &&
                checkSelfPermission(Manifest.permission.CAMERA, ConstantApp.PERMISSION_REQ_ID_CAMERA) &&
                checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, ConstantApp.PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE) &&
                checkSelfPermission(Manifest.permission.PROCESS_OUTGOING_CALLS, ConstantApp.PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL) &&
                checkSelfPermission(Manifest.permission.READ_CALL_LOG, ConstantApp.PERMISSION_REQ_ID_READ_CALL_LOG) &&
                checkSelfPermission(Manifest.permission.WRITE_CALL_LOG, ConstantApp.PERMISSION_REQ_ID_WRITE_CALL_LOG);
    }

    @Override
    protected void onDestroy() {
        deInitUIandEvent();
        super.onDestroy();
    }

    public final void closeIME(View v) {
        InputMethodManager mgr = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
        mgr.hideSoftInputFromWindow(v.getWindowToken(), 0); // 0 force close IME
        v.clearFocus();
    }

    public final void closeIMEWithoutFocus(View v) {
        InputMethodManager mgr = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
        mgr.hideSoftInputFromWindow(v.getWindowToken(), 0); // 0 force close IME
    }

    public void openIME(final EditText v) {
        final boolean focus = v.requestFocus();
        if (v.hasFocus()) {
            final Handler handler = new Handler(Looper.getMainLooper());
            handler.post(new Runnable() {
                @Override
                public void run() {
                    InputMethodManager mgr = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
                    boolean result = mgr.showSoftInput(v, InputMethodManager.SHOW_FORCED);
                    log.debug("openIME " + focus + " " + result);
                }
            });
        }
    }

    public boolean checkSelfPermission(String permission, int requestCode) {
        log.debug("checkSelfPermission " + permission + " " + requestCode);
        if (ContextCompat.checkSelfPermission(this,
                permission)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                    new String[]{permission},
                    requestCode);
            return false;
        }

        if (Manifest.permission.WRITE_CALL_LOG.equals(permission)) {
            ((AGApplication) getApplication()).initWorkerThread();
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

    public final void showLongToast(final String msg) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_LONG).show();
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String permissions[], @NonNull int[] grantResults) {
        log.debug("onRequestPermissionsResult " + requestCode + " " + Arrays.toString(permissions) + " " + Arrays.toString(grantResults));
        switch (requestCode) {
            case ConstantApp.PERMISSION_REQ_ID_RECORD_AUDIO: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.CAMERA, ConstantApp.PERMISSION_REQ_ID_CAMERA);
                } else {
                    finish();
                }
                break;
            }
            case ConstantApp.PERMISSION_REQ_ID_CAMERA: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, ConstantApp.PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE);
                } else {
                    finish();
                }
                break;
            }
            case ConstantApp.PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.PROCESS_OUTGOING_CALLS, ConstantApp.PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL);
                } else {
                    finish();
                }
                break;
            }
            case ConstantApp.PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.READ_CALL_LOG, ConstantApp.PERMISSION_REQ_ID_READ_CALL_LOG);
                } else {
                    finish();
                }
                break;

            }
            case ConstantApp.PERMISSION_REQ_ID_READ_CALL_LOG: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.WRITE_CALL_LOG, ConstantApp.PERMISSION_REQ_ID_WRITE_CALL_LOG);

                } else {
                    finish();
                }
                break;
            }
            case ConstantApp.PERMISSION_REQ_ID_WRITE_CALL_LOG: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    ((AGApplication) getApplication()).initWorkerThread();
                } else {
                    finish();
                }
                break;

            }
        }
    }

    final void needReLogin(int type, Object... data) {
        if (EVENT_TYPE_ON_RELOGIN_NEEDED == type) {
            final boolean banned = (boolean) data[0];

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    showLongToast("Logout, " + (banned ? "try again later" : "banned by server"));

                    Intent intent = new Intent();
                    intent.putExtra("result", "finish");
                    setResult(RESULT_OK, intent);

                    finish();
                }
            });
        }
    }

    protected void initVersionInfo() {
        String version = "V " + BuildConfig.VERSION_NAME + "(Build: " + BuildConfig.VERSION_CODE
                + ", " + ConstantApp.APP_BUILD_DATE + ", SDK: " + RtcEngine.getSdkVersion() + ")";
//        TextView textVersion = (TextView) findViewById(R.id.app_version);
//        textVersion.setText(version);
    }
}
