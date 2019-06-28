package io.agora.tutorials.stringified.account;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initUIandEvent();
    }

    private void initUIandEvent() {
        EditText v_account = (EditText) findViewById(R.id.stringified_account);
        v_account.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                boolean isEmpty = TextUtils.isEmpty(s.toString());
                findViewById(R.id.button_join).setEnabled(!isEmpty);
            }
        });
    }

    private void deInitUIandEvent() {
    }

    public void onClickJoin(View view) {
        forwardToRoom();
    }

    public void forwardToRoom() {
        EditText v_account = (EditText) findViewById(R.id.stringified_account);
        String account = v_account.getText().toString();

        Intent i = new Intent(MainActivity.this, CallActivity.class);
        i.putExtra(BuildConfig.KEY_STRINGIFIED_ACCOUNT, account);

        startActivity(i);
    }

    @Override
    protected void onDestroy() {
        deInitUIandEvent();
        super.onDestroy();
    }
}
