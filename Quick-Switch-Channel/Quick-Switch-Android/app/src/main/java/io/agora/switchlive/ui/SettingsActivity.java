package io.agora.switchlive.ui;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import io.agora.openlive.R;
import io.agora.switchlive.rtc.ConstantApp;

public class SettingsActivity extends AppCompatActivity {
    private VideoProfileAdapter mVideoProfileAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        initUi();
    }

    private void initUi() {
        RecyclerView v_profiles = findViewById(R.id.profiles);
        v_profiles.setHasFixedSize(true);

        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        int prefIndex = pref.getInt(ConstantApp.PrefManager.PREF_PROPERTY_PROFILE_IDX, ConstantApp.DEFAULT_PROFILE_IDX);

        mVideoProfileAdapter = new VideoProfileAdapter(this, prefIndex);
        mVideoProfileAdapter.setHasStableIds(true);

        RecyclerView.LayoutManager layoutManager = new LinearLayoutManager(this, LinearLayoutManager.VERTICAL, false);
        v_profiles.setLayoutManager(layoutManager);

        v_profiles.setAdapter(mVideoProfileAdapter);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_settings, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle item selection
        switch (item.getItemId()) {
            case R.id.confirm:
                doSaveProfile();

                onBackPressed();

                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void doSaveProfile() {
        int profileIndex = mVideoProfileAdapter.getSelected();

        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        SharedPreferences.Editor editor = pref.edit();
        editor.putInt(ConstantApp.PrefManager.PREF_PROPERTY_PROFILE_IDX, profileIndex);
        editor.apply();
    }

    public static class VideoProfileAdapter extends RecyclerView.Adapter {
        private Context context;

        private int selected;

        VideoProfileAdapter(Context context, int selected) {
            this.context = context;
            this.selected = selected;
        }

        @NonNull
        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.video_profile_item, parent, false);
            // set the view's size, margins, paddings and layout parameters

            ProfileHolder ph = new ProfileHolder(v);
            return ph;
        }

        @Override
        public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
            String resolution = context.getResources().getStringArray(R.array.string_array_resolutions)[position];
            ((ProfileHolder) holder).resolution.setText(resolution);

            String frameRate = context.getResources().getStringArray(R.array.string_array_frame_rate)[position];
            ((ProfileHolder) holder).frameRate.setText(frameRate);

            holder.itemView.setBackgroundResource(position == selected ? R.color.lightColorAccent : android.R.color.transparent);
        }

        @Override
        public int getItemCount() {
            return context.getResources().getStringArray(R.array.string_array_resolutions).length;
        }

        class ProfileHolder extends RecyclerView.ViewHolder {
            TextView resolution;
            TextView frameRate;

            ProfileHolder(View itemView) {
                super(itemView);

                resolution = itemView.findViewById(R.id.resolution);
                frameRate = itemView.findViewById(R.id.frame_rate);

                itemView.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        selected = getLayoutPosition();
                        notifyDataSetChanged();
                    }
                });
            }
        }

        int getSelected() {
            return selected;
        }
    }
}
