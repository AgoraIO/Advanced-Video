package io.agora.rtc.ss.app;

import android.content.Intent;
import android.os.Bundle;
import android.app.Activity;
import android.view.View;

public class MainActivity extends Activity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
    }

    private void initView() {
        findViewById(R.id.btn_broadcaster).setOnClickListener(this);
        findViewById(R.id.btn_audience).setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        int vId = view.getId();
        if (vId == R.id.btn_broadcaster) {
            Intent i = new Intent(this, HelloAgoraScreenSharingActivity.class);
            startActivity(i);
        } else if (vId == R.id.btn_audience) {
            Intent i = new Intent(this, AudienceActivity.class);
            startActivity(i);
        }
    }
}
