package io.agora.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

import io.agora.openduo.AGApplication;
import io.agora.openduo.R;
import io.agora.rtc.RtcEngine;
import io.agora.rtm.ErrorInfo;
import io.agora.rtm.ResultCallback;

/**
 * @author yangting
 * app desigine account must be number
 */
public class MainActivity extends AppCompatActivity {
    private final String TAG = MainActivity.class.getSimpleName();

    private EditText textAccountName;
    private String uid;
    ResultCallback<Void> rtmLoginCallback = new ResultCallback<Void>() {
        @Override
        public void onSuccess(Void aVoid) {
            Log.i(TAG, "Login Success");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Intent intent = new Intent(MainActivity.this, NumberCallActivity.class);
                    intent.putExtra("uid", uid);
                    startActivity(intent);
                }
            });
        }

        @Override
        public void onFailure(ErrorInfo errorInfo) {
            Log.i(TAG, "Login Error error_code = " + errorInfo.getErrorCode()
                    + "and error_description " + errorInfo.getErrorDescription());
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);


        textAccountName = findViewById(R.id.account_name);
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
    }

    // login signaling
    public void onClickLogin(View v) {
        Log.i(TAG, "onClickLogin");
        uid = textAccountName.getText().toString().trim();
        AGApplication.the().getRTMClient().login(null, uid, rtmLoginCallback);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "onDestroy");
        RtcEngine.destroy();
    }
}
