package io.agora.rtc.mediaio.app.videoSource.ui;

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;

import io.agora.rtc.Constants;
import io.agora.rtc.mediaio.app.BaseActivity;
import io.agora.rtc.mediaio.app.R;
import io.agora.rtc.mediaio.app.rtcEngine.ConstantApp;


public class MainActivity extends BaseActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    @Override
    protected void initUIandEvent() {
        EditText textRoomName = (EditText) findViewById(R.id.room_name);
        textRoomName.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                boolean isEmpty = s.toString().isEmpty();
                findViewById(R.id.button_join).setEnabled(!isEmpty);
            }
        });
    }

    @Override
    protected void deInitUIandEvent() {
    }


    public void onClickJoin(View view) {
        MainActivity.this.forwardToLiveRoom(Constants.CLIENT_ROLE_BROADCASTER);
    }

    public void forwardToLiveRoom(int cRole) {
        final EditText v_room = (EditText) findViewById(R.id.room_name);
        final EditText video_path = (EditText) findViewById(R.id.video_path);
        String room = v_room.getText().toString();
        String videoPath = video_path.getText().toString();

        Intent i = new Intent(MainActivity.this, PrivateTextureViewActivity.class);
        i.putExtra(ConstantApp.ACTION_KEY_CROLE, cRole);
        i.putExtra(ConstantApp.ACTION_KEY_ROOM_NAME, room);
        i.putExtra(ConstantApp.ACTION_KEY_VIDEO_PATH, videoPath);
        startActivity(i);
    }

}
