package io.agora.advancedvideo.activities;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.core.graphics.drawable.RoundedBitmapDrawable;
import androidx.core.graphics.drawable.RoundedBitmapDrawableFactory;

import io.agora.advancedvideo.R;
import io.agora.advancedvideo.annotations.DisplayActivity;
import io.agora.rtc.Constants;

@DisplayActivity(
    SubClasses = {
        "io.agora.advancedvideo.switchvideoinput.SwitchVideoInputActivity",
        "io.agora.advancedvideo.videoencryption.VideoActivity",
        "io.agora.advancedvideo.customrenderer.CustomRemoteRenderActivity" ,
        "io.agora.advancedvideo.videopush.VideoPushActivity",
        "io.agora.advancedvideo.rawdatasample.RawDataActivity"
    }
)
public abstract class BaseLiveActivity extends BaseActivity {
    protected RelativeLayout videoContainer;

    protected ImageView mMuteAudioBtn;
    protected ImageView mMuteVideoBtn;

    protected boolean mIsBroadcaster;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initUI();
    }

    private void initUI() {
        setContentView(R.layout.activity_live_room);

        TextView roomName = findViewById(R.id.live_room_name);
        roomName.setText(config().getChannelName());
        roomName.setSelected(true);

        initUserIcon();

        rtcEngine().setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        int role = getIntent().getIntExtra(
                io.agora.advancedvideo.Constants.KEY_CLIENT_ROLE,
                Constants.CLIENT_ROLE_AUDIENCE);
        rtcEngine().setClientRole(role);
        mIsBroadcaster =  (role == Constants.CLIENT_ROLE_BROADCASTER);

        mMuteVideoBtn = findViewById(R.id.live_btn_mute_video);
        mMuteVideoBtn.setActivated(mIsBroadcaster);

        mMuteAudioBtn = findViewById(R.id.live_btn_mute_audio);
        mMuteAudioBtn.setActivated(mIsBroadcaster);

        ImageView beautyBtn = findViewById(R.id.live_btn_beautification);
        beautyBtn.setActivated(true);
        rtcEngine().setBeautyEffectOptions(beautyBtn.isActivated(),
                io.agora.advancedvideo.Constants.DEFAULT_BEAUTY_OPTIONS);

        videoContainer = findViewById(R.id.live_video_container);
        onInitializeVideo();
    }

    private void initUserIcon() {
        Bitmap origin = BitmapFactory.decodeResource(getResources(), R.drawable.fake_user_icon);
        RoundedBitmapDrawable drawable = RoundedBitmapDrawableFactory.create(getResources(), origin);
        drawable.setCircular(true);
        ImageView iconView = findViewById(R.id.live_name_board_icon);
        iconView.setImageDrawable(drawable);
    }

    @Override
    protected void onGlobalLayoutCompleted() {
        RelativeLayout topLayout = findViewById(R.id.live_room_top_layout);
        RelativeLayout.LayoutParams params =
                (RelativeLayout.LayoutParams) topLayout.getLayoutParams();
        params.height = mStatusBarHeight + topLayout.getMeasuredHeight();
        topLayout.setLayoutParams(params);
        topLayout.setPadding(0, mStatusBarHeight, 0, 0);
    }

    public void onLeaveClicked(View view) {
        onLeaveButtonClicked(view);
        finish();
    }

    public void onSwitchCameraClicked(View view) {
        onSwitchCameraButtonClicked(view);
    }

    public void onBeautyClicked(View view) {

    }

    public void onMoreClicked(View view) {
        onMoreButtonClicked(view);
    }

    public void onPushStreamClicked(View view) {

    }

    public void onMuteAudioClicked(View view) {
        onMuteAudioButtonClicked(view);
    }

    public void onMuteVideoClicked(View view) {
        onMuteVideoButtonClicked(view);
    }

    protected abstract void onInitializeVideo();

    protected abstract void onLeaveButtonClicked(View view);

    protected abstract void onSwitchCameraButtonClicked(View view);

    protected abstract void onMuteAudioButtonClicked(View view);

    protected abstract void onMuteVideoButtonClicked(View view);

    protected abstract void onMoreButtonClicked(View view);
}
