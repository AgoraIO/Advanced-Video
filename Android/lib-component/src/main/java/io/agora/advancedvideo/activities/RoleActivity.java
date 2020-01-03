package io.agora.advancedvideo.activities;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.RelativeLayout;

import java.lang.annotation.Annotation;

import io.agora.advancedvideo.R;
import io.agora.advancedvideo.annotations.DisplayActivity;
import io.agora.rtc.Constants;

public class RoleActivity extends BaseActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choose_role);
    }

    @Override
    protected void onGlobalLayoutCompleted() {
        RelativeLayout layout = findViewById(R.id.role_title_layout);
        RelativeLayout.LayoutParams params =
                (RelativeLayout.LayoutParams) layout.getLayoutParams();
        params.height += mStatusBarHeight;
        layout.setLayoutParams(params);

        layout = findViewById(R.id.role_content_layout);
        params = (RelativeLayout.LayoutParams) layout.getLayoutParams();
        params.topMargin = (mDisplayMetrics.heightPixels -
                layout.getMeasuredHeight()) * 3 / 7;
        layout.setLayoutParams(params);
    }

    public void onJoinAsBroadcaster(View view) {
        gotoLiveActivity(Constants.CLIENT_ROLE_BROADCASTER);
    }

    public void onJoinAsAudience(View view) {
        gotoLiveActivity(Constants.CLIENT_ROLE_AUDIENCE);
    }

    private void gotoLiveActivity(int role) {
        Annotation[] annotations = BaseLiveActivity.class.getDeclaredAnnotations();
        Class<?> targetActivity = null;
        for (Annotation annotation : annotations) {
            if (annotation instanceof DisplayActivity) {
                boolean found;
                String[] targets = ((DisplayActivity) annotation).SubClasses();
                for (String className : targets) {
                    try {
                        targetActivity = Class.forName(className);
                        found = true;
                    } catch (ClassNotFoundException e) {
                        found = false;
                    }

                    if (targetActivity != null && found) {
                        break;
                    }
                }
            }
        }

        if (targetActivity != null) {
            Intent intent = new Intent(getIntent());
            intent.putExtra(io.agora.advancedvideo.Constants.KEY_CLIENT_ROLE, role);
            intent.setClass(getApplicationContext(), targetActivity);
            startActivity(intent);
        }
    }

    public void onBackArrowPressed(View view) {
        finish();
    }
}
