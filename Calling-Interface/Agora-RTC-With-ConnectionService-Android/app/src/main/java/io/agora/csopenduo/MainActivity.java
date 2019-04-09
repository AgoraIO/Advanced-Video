package io.agora.csopenduo;

import android.Manifest;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import io.agora.AgoraAPI;
import io.agora.IAgoraAPI;
import io.agora.rtc.RtcEngine;

/**
 * @author yangting
 * app desigine account must be number
 */
public class MainActivity extends Activity {
    private static final String TAG = MainActivity.class.getSimpleName();
    private static boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    private EditText textAccountName;
    private String appId;
    private int uid;
    private String account;

    private static final int PERMISSION_REQ_ID_RECORD_AUDIO = 22;
    private static final int PERMISSION_REQ_ID_CAMERA = PERMISSION_REQ_ID_RECORD_AUDIO + 1;
    private static final int PERMISSION_REQ_ID_STORAGE = PERMISSION_REQ_ID_CAMERA + 1;
    private static final int PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL = PERMISSION_REQ_ID_STORAGE + 1;
    private static final int PERMISSION_REQ_ID_READ_CALL_LOG = PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL +1;
    private static final int PERMISSION_REQ_ID_WRITE_CALL_LOG = PERMISSION_REQ_ID_READ_CALL_LOG +1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        appId = getString(R.string.agora_app_id);

        textAccountName = (EditText) findViewById(R.id.account_name);
        textAccountName.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                boolean isEmpty = s.toString().isEmpty();
                findViewById(R.id.button_login).setEnabled(!isEmpty);
            }
        });

        if (checkSelfPermission(Manifest.permission.RECORD_AUDIO, PERMISSION_REQ_ID_RECORD_AUDIO)
                && checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA)
                && checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_STORAGE)
                && checkSelfPermission(Manifest.permission.PROCESS_OUTGOING_CALLS, PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL)
                && checkSelfPermission(Manifest.permission.READ_CALL_LOG, PERMISSION_REQ_ID_READ_CALL_LOG)
                && checkSelfPermission(Manifest.permission.WRITE_CALL_LOG, PERMISSION_REQ_ID_WRITE_CALL_LOG)) {
            registerNewPhoneAccount(getApplicationContext());
        }
    }

    public boolean checkSelfPermission(String permission, int requestCode) {
        Log.i(TAG, "checkSelfPermission " + permission + " " + requestCode);
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
        Log.i(TAG, "onRequestPermissionsResult " + grantResults[0] + " " + requestCode);

        switch (requestCode) {
            case PERMISSION_REQ_ID_RECORD_AUDIO: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.CAMERA, PERMISSION_REQ_ID_CAMERA);
                } else {
                    finish();
                }
                break;
            }
            case PERMISSION_REQ_ID_CAMERA: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, PERMISSION_REQ_ID_STORAGE);
                } else {
                    finish();
                }
                break;
            }
            case PERMISSION_REQ_ID_STORAGE: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.PROCESS_OUTGOING_CALLS, PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL);
                } else {
                    finish();
                }
                break;
            }
            case PERMISSION_REQ_ID_PROCESS_OUTGOING_CALL: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.READ_CALL_LOG, PERMISSION_REQ_ID_READ_CALL_LOG);

                } else {
                    finish();
                }
                break;

            }
            case PERMISSION_REQ_ID_READ_CALL_LOG: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkSelfPermission(Manifest.permission.WRITE_CALL_LOG, PERMISSION_REQ_ID_WRITE_CALL_LOG);

                } else {
                    finish();
                }
                break;

            }
            case PERMISSION_REQ_ID_WRITE_CALL_LOG: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    registerNewPhoneAccount(getApplicationContext());
                } else {
                    finish();
                }
                break;

            }
        }
    }

    // login signaling
    public void onClickLogin(View v) {
        Log.i(TAG, "onClickLogin");
        account = textAccountName.getText().toString().trim();

        AGApplication.the().getmAgoraAPI().login2(appId, account, "_no_need_token", 0, "", 5, 1);
    }

    private void addCallback() {
        Log.i(TAG, "addCallback enter.");
        AGApplication.the().getmAgoraAPI().callbackSet(new AgoraAPI.CallBack() {

            @Override
            public void onLoginSuccess(int i, int i1) {
                Log.i(TAG, "onLoginSuccess " + i + "  " + i1);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Intent intent = new Intent(MainActivity.this, NumberCallActivity.class);
                        intent.putExtra("uid", uid);
                        intent.putExtra("account", account);
                        startActivity(intent);

                    }
                });
            }

            @Override
            public void onLogout(int i) {
                Log.i(TAG, "onLogout  i = " + i);

            }

            @Override
            public void onLoginFailed(final int i) {
                Log.i(TAG, "onLoginFailed " + i);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (i == IAgoraAPI.ECODE_LOGIN_E_NET) {
                            Toast.makeText(MainActivity.this, "Login Failed for the network is not available", Toast.LENGTH_SHORT).show();
                        }
                    }
                });
            }

            @Override
            public void onError(String s, int i, String s1) {
                Log.i(TAG, "onError s:" + s + " s1:" + s1);
            }

        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        addCallback();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "onDestroy");
        RtcEngine.destroy();
    }

    /**
     * Create PhoneAccounts and register with the system.
     */
    private void registerNewPhoneAccount(Context context) {
        if(DEBUG) {
            Log.d(TAG, "register new phone accounts");
        }
        PhoneAccountHandle accountHandleIn = new PhoneAccountHandle(
                new ComponentName(context, AgoraConnectionService.class), Constant.PHONEACCOUNT_NAME_IN);
        PhoneAccountHandle accountHandleOut = new PhoneAccountHandle(
                new ComponentName(context, AgoraConnectionService.class), Constant.PHONEACCOUNT_NAME_OUT);

        PhoneAccount phoneIn = PhoneAccount.builder(accountHandleIn, Constant.PHONEACCOUNT_NAME_IN)
                .setCapabilities(PhoneAccount.CAPABILITY_CALL_PROVIDER)
                .build();

        Bundle extra = new Bundle();
        extra.putBoolean(PhoneAccount.EXTRA_LOG_SELF_MANAGED_CALLS, true);
        PhoneAccount phoneOut = PhoneAccount.builder(accountHandleOut, Constant.PHONEACCOUNT_NAME_OUT)
                .setCapabilities(PhoneAccount.CAPABILITY_SELF_MANAGED)
                .setExtras(extra)
                .build();
        TelecomManager telecomManager = (TelecomManager) context.getSystemService(Context.TELECOM_SERVICE);

        telecomManager.registerPhoneAccount(phoneIn);
        telecomManager.registerPhoneAccount(phoneOut);

        CallSession cs = CallSession.getInstance();
        cs.setPhoneAccountIn(phoneIn);
        cs.setPhoneAccountOut(phoneOut);
        cs.setContext(getApplicationContext());

    }
}
