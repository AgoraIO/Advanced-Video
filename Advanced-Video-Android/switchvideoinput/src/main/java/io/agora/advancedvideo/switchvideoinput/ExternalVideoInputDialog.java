package io.agora.advancedvideo.switchvideoinput;

import android.app.Dialog;
import android.content.Context;
import android.content.res.Resources;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;

import io.agora.advancedvideo.externvideosource.ExternalVideoInputManager;
import io.agora.advancedvideo.utils.WindowUtil;

public class ExternalVideoInputDialog extends Dialog implements View.OnClickListener {

    public interface OnSelectVideoInputListener {
        /**
         * Called when an external video input is selected
         * @param type TYPE_LOCAL_VIDEO, TYPE_SCREEN_SHARE or
         *             TYPE_AR_CORE defined in ExternalVideoInputManager
         */
        void onVideoInputSelected(int type);
    }

    private View mReference;
    private boolean mLocalVideoEnabled;
    private TextView mLocalVideoText;
    private ImageView mArrow;
    private OnSelectVideoInputListener mListener;

    ExternalVideoInputDialog(@NonNull Context context, View reference,
                             OnSelectVideoInputListener listener) {
        super(context, R.style.no_background_dialog);
        String[] text = context.getResources().getStringArray(R.array.video_input_array);
        mReference = reference;
        mListener = listener;

        setContentView(R.layout.choose_video_input_dialog_layout);

        TextView textView = findViewById(R.id.input_local_video);
        textView.setText(text[0]);
        textView.setOnClickListener(this);
        mLocalVideoText = textView;

        textView = findViewById(R.id.input_screen_share);
        textView.setText(text[1]);
        textView.setOnClickListener(this);

        textView = findViewById(R.id.input_ar_core);
        textView.setText(text[2]);
        textView.setOnClickListener(this);

        mArrow = findViewById(R.id.video_input_dialog_arrow);
    }

    @Override
    public void onClick(View view) {
        int type = 0;
        switch (view.getId()) {
            case R.id.input_local_video:
                type = ExternalVideoInputManager.TYPE_LOCAL_VIDEO;
                break;
            case R.id.input_screen_share:
                type = ExternalVideoInputManager.TYPE_SCREEN_SHARE;
                break;
            case R.id.input_ar_core:
                type = ExternalVideoInputManager.TYPE_AR_CORE;
                break;
        }

        if (type != 0 && mListener != null) {
            mListener.onVideoInputSelected(type);
        }
        dismiss();
    }

    void setLocalVideoEnabled(boolean enabled) {
        mLocalVideoEnabled = enabled;
    }

    @Override
    public void show() {
        doShow();
        super.show();
    }

    private void doShow() {
        WindowManager.LayoutParams params = getWindow().getAttributes();
        params.gravity = Gravity.START | Gravity.BOTTOM;

        int [] location = new int[2];
        mReference.getLocationOnScreen(location);
        params.x = location[0];
        DisplayMetrics metrics = new DisplayMetrics();
        getWindow().getWindowManager().getDefaultDisplay().getMetrics(metrics);
        Resources resources = getContext().getResources();
        params.width = resources.getDimensionPixelSize(R.dimen.video_input_dialog_width);
        params.height = resources.getDimensionPixelSize(R.dimen.video_input_dialog_height);
        int statusBarH = WindowUtil.getSystemStatusBarHeight(getContext());
        params.y = metrics.heightPixels - location[1] + statusBarH + 20;
        getWindow().setAttributes(params);

        LinearLayout.LayoutParams arrowParams =
                (LinearLayout.LayoutParams) mArrow.getLayoutParams();
        int refHalf = mReference.getMeasuredWidth() / 2;
        arrowParams.leftMargin += refHalf / 2;
        mArrow.setLayoutParams(arrowParams);

        mLocalVideoText.setEnabled(mLocalVideoEnabled);
    }
}