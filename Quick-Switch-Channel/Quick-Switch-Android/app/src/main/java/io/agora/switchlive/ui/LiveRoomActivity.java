package io.agora.switchlive.ui;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import io.agora.openlive.R;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;
import io.agora.switchlive.rtc.AGEventHandler;
import io.agora.switchlive.rtc.ConstantApp;

import static io.agora.rtc.Constants.CLIENT_ROLE_AUDIENCE;
import static io.agora.switchlive.rtc.ConstantApp.ACTION_CHANNEL_LIST;
import static io.agora.switchlive.rtc.ConstantApp.ACTION_CURRENT_INDEX;

public class LiveRoomActivity extends BaseActivity implements AGEventHandler {
    private static final String LOG_TAG = "LiveRoomActivity";

    private ViewPager viewPager;
    private StreamPagerAdapter pagerAdapter;
    private int currentIndex = 0;
    private int lastIndex = -1;

    ArrayList<String> channelRooms = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live_room);

        initUIAndEvent();
    }

    protected void initUIAndEvent() {
        channelRooms = getIntent().getStringArrayListExtra(ACTION_CHANNEL_LIST);
        currentIndex = getIntent().getIntExtra(ACTION_CURRENT_INDEX, 0);

        viewPager = findViewById(R.id.viewPager);

        pagerAdapter = new StreamPagerAdapter(this);
        viewPager.setAdapter(pagerAdapter);

        pagerAdapter.setChannelRooms(channelRooms);
        pagerAdapter.notifyDataSetChanged();

        viewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int i, float v, int i1) {
                if (v == 0f && i != currentIndex) {
                    Log.i(LOG_TAG, "onPageScrolled index: " + i);

                    final int index = i;
                    viewPager.post(new Runnable() {
                        @Override
                        public void run() {
                            currentIndex = index;

                            if (lastIndex >= 0) {
                                pagerAdapter.removeSurfaceViewByIndex(lastIndex);
                            }

                            switchChannel(channelRooms.get(index));

                            lastIndex = currentIndex;
                        }
                    });
                }
            }

            @Override
            public void onPageSelected(int i) {
            }

            @Override
            public void onPageScrollStateChanged(int i) {
            }
        });

        event().addEventHandler(this);

        doConfigEngine(CLIENT_ROLE_AUDIENCE);

        viewPager.setCurrentItem(currentIndex);

        joinChannel(channelRooms.get(currentIndex));

        lastIndex = currentIndex;
    }

    @Override
    protected void onDestroy() {
        leaveChannel(channelRooms.get(currentIndex));

        event().removeEventHandler(this);

        super.onDestroy();
    }

    private void doConfigEngine(int cRole) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        int prefIndex = pref.getInt(ConstantApp.PrefManager.PREF_PROPERTY_PROFILE_IDX, ConstantApp.DEFAULT_PROFILE_IDX);
        if (prefIndex > ConstantApp.VIDEO_DIMENSIONS.length - 1) {
            prefIndex = ConstantApp.DEFAULT_PROFILE_IDX;
        }
        VideoEncoderConfiguration.VideoDimensions dimension = ConstantApp.VIDEO_DIMENSIONS[prefIndex];

        worker().configEngine(cRole, dimension);
    }

    private void joinChannel(String channelName) {
        Log.i(LOG_TAG, "join channel: " + channelName);

        worker().joinChannel(channelName, config().mUid);
    }

    private void switchChannel(String channelName) {
        Log.i(LOG_TAG, "switch channel: " + channelName);

        worker().switchChannel(channelName, config().mUid);
    }

    private void leaveChannel(String channelName) {
        Log.i(LOG_TAG, "leave channel: " + channelName);

        worker().leaveChannel(channelName);
    }

    @Override
    public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
    }

    @Override
    public void onJoinChannelSuccess(final String channel, final int uid, final int elapsed) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isFinishing()) {
                    return;
                }

                Log.i(LOG_TAG, "onJoinChannelSuccess uid: " + uid);

                worker().getEngineConfig().mUid = uid;
            }
        });
    }

    @Override
    public void onUserJoined(final int uid, int elapsed) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isFinishing()) return;

                Log.i(LOG_TAG, "onUserJoined uid: " + uid);

                SurfaceView surfaceV = RtcEngine.CreateRendererView(getApplicationContext());

                if (config().mUid == uid) {
                    rtcEngine().setupLocalVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, uid));
                } else {
                    rtcEngine().setupRemoteVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, uid));
                }

                pagerAdapter.setSurfaceView(currentIndex, uid, surfaceV);
            }
        });
    }

    @Override
    public void onUserOffline(final int uid, int reason) {
        Log.i("SwitchLive", "onUserOffline " + (uid & 0xFFFFFFFFL) + " " + reason);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isFinishing()) return;

                Log.i(LOG_TAG, "onUserOffline uid: " + uid);

                pagerAdapter.removeSurfaceView(uid);
            }
        });
    }

    public class StreamPagerAdapter extends PagerAdapter {

        private SparseArray<ViewGroup> viewList = new SparseArray<>();
        private Context context;
        private List<String> roomNameList = new ArrayList<>();

        public StreamPagerAdapter(Context context) {
            this.context = context;
        }

        @Override
        public Object instantiateItem(ViewGroup collection, int position) {
            ViewGroup layout = viewList.get(position);
            if (layout == null) {
                LayoutInflater inflater = LayoutInflater.from(context);
                layout = (ViewGroup) inflater.inflate(R.layout.layout_channel_room, collection, false);
                viewList.put(position, layout);

                TextView textView = layout.findViewById(R.id.channelName);
                textView.setText(String.format("channel: %s", roomNameList.get(position)));
            }

            collection.addView(layout);

            return layout;
        }

        @Override
        public void destroyItem(ViewGroup collection, int position, Object view) {
            collection.removeView((View) view);
        }

        @Override
        public int getCount() {
            return roomNameList.size();
        }

        public void setChannelRooms(List<String> channelRooms) {
            roomNameList = channelRooms;
        }

        private void setSurfaceView(int position, final int uid, final SurfaceView view) {
            final ViewGroup viewGroup = viewList.get(position);
            if (viewGroup != null) {
                ViewGroup surfaceContainer = viewGroup.findViewById(R.id.surfaceViewContainer);
                surfaceContainer.removeAllViews();
                surfaceContainer.addView(view, new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

                TextView uidTextView = viewGroup.findViewById(R.id.channelUid);
                uidTextView.setText(String.format("uid: %d", uid));

                viewGroup.setTag(uid);
            }
        }

        private void removeSurfaceView(int uid) {
            for (int i = 0; i < viewList.size(); i++) {
                ViewGroup viewGroup = viewList.get(i);

                if (viewGroup.getTag() != null && ((Integer) viewGroup.getTag()) == uid) {
                    removeSurfaceView(viewGroup);
                }
            }
        }

        private void removeSurfaceViewByIndex(int index) {
            ViewGroup viewGroup = viewList.get(index);
            if (viewGroup != null) {
                removeSurfaceView(viewGroup);
            }
        }

        private void removeSurfaceView(ViewGroup viewGroup) {
            ViewGroup surfaceContainer = viewGroup.findViewById(R.id.surfaceViewContainer);
            surfaceContainer.removeAllViews();

            TextView uidTextView = viewGroup.findViewById(R.id.channelUid);
            uidTextView.setText("");
        }

        @Override
        public boolean isViewFromObject(@NonNull View view, @NonNull Object object) {
            return view == object;
        }

        @Override
        public CharSequence getPageTitle(int position) {
            return "";
        }
    }
}
