package io.agora.switchlive.ui;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;

import io.agora.openlive.BuildConfig;
import io.agora.openlive.R;
import io.agora.switchlive.AGApplication;
import io.agora.switchlive.Constant;
import io.agora.switchlive.rtc.ConstantApp;

public class MainActivity extends BaseActivity {

    private ArrayList<String> channelList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initUIAndEvent();
    }

    protected void initUIAndEvent() {
        channelList.add("channel1");
        channelList.add("channel2");
        channelList.add("channel3");

        LinearLayout viewGroup = findViewById(R.id.channelContainer);
        for (int i = 0; i < channelList.size(); i++) {
            final TextView textView = new TextView(this);
            textView.setText(String.format("Channel: %s", channelList.get(i)));
            textView.setTag(i);
            textView.setGravity(Gravity.CENTER);
            textView.setBackgroundColor(ContextCompat.getColor(this, R.color.colorPrimary));
            textView.setTextColor(Color.WHITE);
            textView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    forwardToLiveRoom((Integer) textView.getTag());
                }
            });

            LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.MATCH_PARENT, 1.0f);
            layoutParams.setMargins(0, 0, 10, 0);

            viewGroup.addView(textView, layoutParams);
        }

        String version = "V " + BuildConfig.VERSION_NAME + "(Build: " + BuildConfig.VERSION_CODE
                + ", " + ConstantApp.APP_BUILD_DATE + ", SDK: " + Constant.MEDIA_SDK_VERSION + ")";
        TextView textVersion = findViewById(R.id.app_version);
        textVersion.setText(version);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        ((AGApplication) getApplication()).deInitWorkerThread();
    }

    @Override
    public boolean onCreateOptionsMenu(final Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case R.id.action_settings:
                forwardToSettings();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    public void forwardToLiveRoom(int index) {
        Intent i = new Intent(MainActivity.this, LiveRoomActivity.class);
        i.putExtra(ConstantApp.ACTION_CURRENT_INDEX, index);
        i.putStringArrayListExtra(ConstantApp.ACTION_CHANNEL_LIST, channelList);

        startActivity(i);
    }

    public void forwardToSettings() {
        Intent i = new Intent(this, SettingsActivity.class);
        startActivity(i);
    }
}
