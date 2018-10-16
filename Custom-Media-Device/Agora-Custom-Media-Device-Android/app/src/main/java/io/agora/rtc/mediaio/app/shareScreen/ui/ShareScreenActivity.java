package io.agora.rtc.mediaio.app.shareScreen.ui;

import android.Manifest;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;

import io.agora.rtc.mediaio.app.BaseActivity;
import io.agora.rtc.mediaio.app.R;

public class ShareScreenActivity extends BaseActivity implements SurfaceReadyListener {
    private static final int RECORD_REQUEST_CODE = 101;
    private static final int STORAGE_REQUEST_CODE = 102;
    private static final int AUDIO_REQUEST_CODE = 103;
    private static int[] PICTURE_ARRAY = null;
    private static final String TAG = "NewInterfaceActivity";
    private MediaProjectionManager projectionManager;
    private MediaProjection mediaProjection;
    private RecordService recordService;
    private Button startFullBtn;
    private LinearLayout viewLayout;
    private CheckBox enableViewBox;
    private ImageView recordView;
    private EditText channelName;
    private String channelNameString;
    private CheckBox enableLocal;
    private Boolean isEnableLocal = false;
    private View localView;
    private int pictureCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "onCreate：");
        projectionManager = (MediaProjectionManager) getSystemService(MEDIA_PROJECTION_SERVICE);

        setContentView(R.layout.activity_new_interface);
    }

    @Override
    protected void initUIandEvent() {
        viewLayout = (LinearLayout) findViewById(R.id.view_record_layout);
        recordView = (ImageView) findViewById(R.id.recordView);
        startFullBtn = (Button) findViewById(R.id.start_full_record);
        enableViewBox = (CheckBox) findViewById(R.id.enable_view_record);
        channelName = (EditText) findViewById(R.id.channel_name);
        enableLocal = (CheckBox) findViewById(R.id.local_preview);
        PICTURE_ARRAY = new int[]{
                R.drawable.tex_0,
                R.drawable.tex_1,
                R.drawable.tex_2,
                R.drawable.tex_3};
        enableLocal.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                if (isChecked) {
                    showLocalView();
                    isEnableLocal = true;
                } else {
                    removeLocalView();
                    isEnableLocal = false;
                }
            }
        });

        startFullBtn.setEnabled(false);
        startFullBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (recordService.isRunning()) {
                    recordService.stopRecord();
                    startFullBtn.setText(R.string.start_full_record);
                } else {
                    if (channelName.getText().toString().length() <= 0) {
                        return;
                    }
                    Intent captureIntent = projectionManager.createScreenCaptureIntent();
                    startActivityForResult(captureIntent, RECORD_REQUEST_CODE);
                }
            }
        });

        enableViewBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                if (isChecked) {
                    viewLayout.setVisibility(View.VISIBLE);
                    recordService.setRecordView(recordView);
                    recordService.setEnableViewRecord(true);
                } else {
                    recordService.setEnableViewRecord(false);
                    viewLayout.setVisibility(View.GONE);
                }
            }
        });

        if (ContextCompat.checkSelfPermission(ShareScreenActivity.this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, STORAGE_REQUEST_CODE);
        }

        if (ContextCompat.checkSelfPermission(ShareScreenActivity.this, Manifest.permission.RECORD_AUDIO)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.RECORD_AUDIO}, AUDIO_REQUEST_CODE);
        }

        Intent intent = new Intent(this, RecordService.class);
        bindService(intent, connection, BIND_AUTO_CREATE);
    }


    @Override
    protected void deInitUIandEvent() {

    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(connection);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == RECORD_REQUEST_CODE && resultCode == RESULT_OK) {
            channelNameString = channelName.getText().toString();
            recordService.setChannelName(channelNameString);
            mediaProjection = projectionManager.getMediaProjection(resultCode, data);
            recordService.setMediaProject(mediaProjection);
            recordService.setWorkerThread(worker());
            recordService.startRecord();
            startFullBtn.setText(R.string.stop_full_record);
        }
    }


    private ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            DisplayMetrics metrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(metrics);
            RecordService.RecordBinder binder = (RecordService.RecordBinder) service;
            recordService = binder.getRecordService();
            recordService.setConfig(metrics.widthPixels, metrics.heightPixels, metrics.densityDpi);
            recordService.setSurfaceReadyListener(ShareScreenActivity.this);
            startFullBtn.setEnabled(true);
            startFullBtn.setText(recordService.isRunning() ? R.string.stop_full_record : R.string.start_full_record);
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
        }
    };

    @Override
    public void surfaceIsReady(View previewSurface) {
        localView = previewSurface;
        if (isEnableLocal) {
            showLocalView();
        } else {
            removeLocalView();
        }
    }

    public void showLocalView() {
        if (localView != null && enableLocal.isChecked()) {
            removeLocalView();
            FrameLayout container = (FrameLayout) findViewById(R.id.local_video_view_container);
            container.addView(localView);
            container.setVisibility(View.VISIBLE);
        }

    }

    public void removeLocalView() {
        FrameLayout container = (FrameLayout) findViewById(R.id.local_video_view_container);
        if (container.getChildCount() > 0) {
            container.removeView(container.getChildAt(0));
        }
        container.setVisibility(View.INVISIBLE);
    }

    public boolean checkSelfPermission(String permission, int requestCode) {
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

    public void onChangePicture(View view) {
        int temp = (pictureCount % PICTURE_ARRAY.length);
        recordView.setImageResource(PICTURE_ARRAY[temp]);
        pictureCount++;
    }
}
