package io.agora.openlive.ui;

import android.content.Intent;
import android.content.SharedPreferences;

import android.graphics.PorterDuff;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;

import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;

import java.util.List;


import io.agora.openlive.R;
import io.agora.openlive.model.AGEventHandler;
import io.agora.openlive.model.ConstantApp;
import io.agora.openlive.model.UserItem;
import io.agora.rtc.Constants;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;

public class LiveRoom17Activity extends BaseActivity implements AGEventHandler, LiveRoom17Adapter.onClickButtonInterface {

    private final static Logger log = LoggerFactory.getLogger(LiveRoom17Activity.class);

    private LiveRoom17Adapter adapter;
    private LiveRoomShowFourAdapter adapterShowFour;
    private List<UserItem> userItemList = new ArrayList<>(); //small 16 windows
    private RecyclerView recyclerView;
    private RelativeLayout mParentView;
    private UserItem mUserItem;//one big window
    private RecyclerViewNoBugLinearLayoutManager linearLayoutManager;
    private GridLayoutManager gridLayoutManager;
    private final int MODE_ONE_BIG = 0;
    private final int MODE_SHOW_FOUR = 1;
    private int stateMode = MODE_SHOW_FOUR; // 0 : one big with 16 small, 1: 17 small, show 4
    private int screenWidth;
    private int screenHeight;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live_room17);
        log.debug("sdk version：：" + rtcEngine().getSdkVersion());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return false;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        return false;
    }


    @Override
    protected void initUIandEvent() {
        event().addEventHandler(this);

        log.debug("initUIandEvent ");

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        screenWidth = displayMetrics.widthPixels;
        screenHeight = displayMetrics.heightPixels;


        Intent i = getIntent();
        int cRole = i.getIntExtra(ConstantApp.ACTION_KEY_CROLE, 0);

        if (cRole == 0) {
            throw new RuntimeException("Should not reach here");
        }


        String roomName = i.getStringExtra(ConstantApp.ACTION_KEY_ROOM_NAME);

        doConfigEngine(cRole);

        mParentView = (RelativeLayout) findViewById(R.id.parentview);
        recyclerView = (RecyclerView) findViewById(R.id.recycleview);
        recyclerView.addOnScrollListener(scrollListener);

        ImageView button2 = (ImageView) findViewById(R.id.btn_2);
        ImageView button3 = (ImageView) findViewById(R.id.btn_3);


        addBigSurfaceView(0);
        initModeState();
        broadcasterUI(button2, button3);

        worker().joinChannel(roomName, config().mUid);

        TextView textRoomName = (TextView) findViewById(R.id.room_name);
        textRoomName.setText(roomName);
    }

    private void addBigSurfaceView(int uid) {
        log.debug("addBigSurfaceView uid :" + uid);
        SurfaceView surfaceV = RtcEngine.CreateRendererView(getApplicationContext());
        mUserItem = new UserItem(uid, surfaceV, true);

        mUserItem.setmIsMuteStream(false);
        rtcEngine().setupLocalVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, uid));
        worker().preview(true, surfaceV, uid);


        surfaceV.setZOrderOnTop(false);
        surfaceV.setZOrderMediaOverlay(false);

        if (stateMode == MODE_ONE_BIG) {
            mParentView.addView(surfaceV, 0);

        } else if (stateMode == MODE_SHOW_FOUR) {
            userItemList.add(0, mUserItem);
        }


    }


    private void broadcasterUI(ImageView button2, ImageView button3) {
        log.debug("broadcasterUI ");

        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                worker().getRtcEngine().switchCamera();
            }
        });

        button3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Object tag = v.getTag();
                boolean flag = true;
                if (tag != null && (boolean) tag) {
                    flag = false;
                }
                worker().getRtcEngine().muteLocalAudioStream(flag);
                ImageView button = (ImageView) v;
                button.setTag(flag);
                if (flag) {
                    button.setColorFilter(getResources().getColor(R.color.agora_blue), PorterDuff.Mode.MULTIPLY);
                } else {
                    button.clearColorFilter();
                }
            }
        });
    }


    private void doConfigEngine(int cRole) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        int prefIndex = pref.getInt(ConstantApp.PrefManager.PREF_PROPERTY_PROFILE_IDX, ConstantApp.DEFAULT_PROFILE_IDX);
        if (prefIndex > ConstantApp.VIDEO_PROFILES.length - 1) {
            prefIndex = ConstantApp.DEFAULT_PROFILE_IDX;
        }
        int vProfile = ConstantApp.VIDEO_PROFILES[prefIndex];

        worker().configEngine(cRole, vProfile);

        rtcEngine().setParameters("{\"che.audio.live_for_comm\":true}");
        rtcEngine().setParameters("{\"che.video.moreFecSchemeEnable\":true}");

        log.debug("doConfigEngine vProfile : " + vProfile);
        if (vProfile == Constants.VIDEO_PROFILE_480P) {
            rtcEngine().setParameters("{\"che.video.lowBitRateStreamParameter\":{\"width\":240,\"height\":320,\"frameRate\":15,\"bitRate\":140}}");

        } else {
            rtcEngine().setParameters("{\"che.video.lowBitRateStreamParameter\":{\"width\":180,\"height\":320,\"frameRate\":15,\"bitRate\":140}}");
        }
        rtcEngine().enableDualStreamMode(true);
    }

    @Override
    protected void deInitUIandEvent() {
        doLeaveChannel();
        event().removeEventHandler(this);

        userItemList.clear();
    }

    private void doLeaveChannel() {
        worker().leaveChannel(config().mChannel);
        worker().preview(false, null, 0);

    }

    public void onClickClose(View view) {
        finish();
    }

    public void onShowHideClicked(View view) {
        boolean toHide = true;
        if (view.getTag() != null && (boolean) view.getTag()) {
            toHide = false;
        }
        view.setTag(toHide);

        doShowButtons(toHide);
    }

    public void onClickChangeMode(View view) {
        if (stateMode == MODE_ONE_BIG) {

            stateMode = MODE_SHOW_FOUR;
            mParentView.removeView(mUserItem.getSurfaceView());
            userItemList.add(0, mUserItem);

        } else if (stateMode == MODE_SHOW_FOUR) {
            stateMode = MODE_ONE_BIG;

            userItemList.remove(0);

            ((RelativeLayout) mUserItem.getSurfaceView().getParent()).removeAllViews();
            mParentView.addView(mUserItem.getSurfaceView(), 0);
        }

        initModeState();
    }

    private void doShowButtons(boolean hide) {
        View topArea = findViewById(R.id.top_area);
        topArea.setVisibility(hide ? View.INVISIBLE : View.VISIBLE);

        View button2 = findViewById(R.id.btn_2);
        View button3 = findViewById(R.id.btn_3);
        View button4 = findViewById(R.id.btn_change_mode);

        button2.setVisibility(hide ? View.INVISIBLE : View.VISIBLE);
        button3.setVisibility(hide ? View.INVISIBLE : View.VISIBLE);
        button4.setVisibility(hide ? View.INVISIBLE : View.VISIBLE);


    }

    @Override
    public void onFirstRemoteVideoDecoded(int uid, int width, int height, int elapsed) {
        log.debug("onFirstRemoteVideoDecoded " + width + " " + height + " " + (uid & 0XFFFFFFFFL) + " ");
    }


    private void doRenderRemoteUi(final int uid) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isFinishing()) {
                    return;
                }

                log.debug("doRenderRemoteUi   uid :" + uid + " mUserItem :" + mUserItem + "  size :" + userItemList.size());

                if (isExitUid(uid)) {

                    return;
                }

                SurfaceView surfaceV = RtcEngine.CreateRendererView(getApplicationContext());
                surfaceV.setZOrderMediaOverlay(true);
                userItemList.add(new UserItem(uid, surfaceV, false));
                log.debug("doRenderRemoteUi userItemList" + " " + userItemList.size() + " uid :" + uid);

                rtcEngine().setupRemoteVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, uid));
                rtcEngine().muteRemoteAudioStream(uid, true);

                if (stateMode == MODE_ONE_BIG) {
                    rtcEngine().setRemoteVideoStreamType(uid, Constants.VIDEO_STREAM_LOW);
                    adapter.notifyItemRangeChanged(userItemList.size() - 1, 1);
                } else if (stateMode == MODE_SHOW_FOUR) {
                    rtcEngine().setRemoteVideoStreamType(uid, Constants.VIDEO_STREAM_HIGH);
                    resetGridLayoutSpaceCount();
                    adapterShowFour.notifyItemRangeChanged(userItemList.size() - 1, 1);
                }

                recyclerView.scrollToPosition(0);


            }
        });
    }

    @Override
    public void onJoinChannelSuccess(final String channel, final int uid, final int elapsed) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isFinishing()) {
                    return;
                }

                if (isExitUid(uid)) {
                    return;
                }

                log.debug("onJoinChannelSuccess " + channel + " " + uid + " " + elapsed + " ");
                worker().getEngineConfig().mUid = uid;
                SurfaceView surfaceV = mUserItem.getSurfaceView();
                if (surfaceV != null) {
                    mUserItem = new UserItem(uid, surfaceV, true);
                }

            }
        });
    }


    @Override
    public void onUserOffline(int uid, int reason) {
        log.debug("onUserOffline " + (uid & 0xFFFFFFFFL) + " " + reason);
        doRemoveRemoteUi(uid);
    }

    @Override
    public void onUserJoined(int uid, int elapsed) {
        log.debug("onUserJoined " + (uid & 0XFFFFFFFFL) + " ");
        doRenderRemoteUi(uid);

    }

    private void doRemoveRemoteUi(final int uid) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isFinishing()) {
                    return;
                }
                int position = -1;
                log.debug("doRemoveRemoteUi isBroadcaster() :" + " uid :" + uid);
                if (stateMode == MODE_ONE_BIG) {

                    if (uid == mUserItem.getUid()) {
                        position = userItemList.size() - 1;
                        mUserItem.setUid(userItemList.get(position).getUid());

                        if (stateMode == MODE_ONE_BIG) {
                            refreshBigVideoView();
                        }

                    } else {
                        for (int i = 0; i < userItemList.size(); i++) {
                            if (userItemList.get(i).getUid() == uid) {
                                position = i;
                                break;
                            }
                        }

                    }
                    log.debug("doRemoveRemoteUi position :" + position);
                    if (position >= 0) {
                        userItemList.remove(position);

                        adapter.notifyItemRemoved(position);
                    }
                } else if (stateMode == MODE_SHOW_FOUR) {

                    for (int i = 0; i < userItemList.size(); i++) {
                        if (userItemList.get(i).getUid() == uid) {
                            position = i;
                            break;
                        }

                    }
                    resetGridLayoutSpaceCount();
                    if (position >= 0) {
                        userItemList.remove(position);
                        adapterShowFour.notifyItemRemoved(position);
                    }
                }

                log.debug("doRemoveRemoteUi " + (uid & 0xFFFFFFFFL));
            }
        });
    }


    private void refreshSmallVideoView(int position) {
        // list uid ,small window
        if (userItemList.size() > 0 && position < userItemList.size()) {

            if (config().mUid == userItemList.get(position).getUid()) {
                userItemList.get(position).setmIsUserSelf(true);
                rtcEngine().setupLocalVideo(new VideoCanvas(userItemList.get(position).getSurfaceView(), VideoCanvas.RENDER_MODE_HIDDEN, userItemList.get(position).getUid()));

            } else {
                log.debug("refreshSmallVideoView position:" + position);
                userItemList.get(position).setmIsUserSelf(false);
                rtcEngine().setupRemoteVideo(new VideoCanvas(userItemList.get(position).getSurfaceView(), VideoCanvas.RENDER_MODE_HIDDEN, userItemList.get(position).getUid()));
                requestRemoteStreamType(userItemList.get(position).getUid(), Constants.VIDEO_STREAM_LOW);
            }

        }
        if (stateMode == MODE_ONE_BIG) {
            adapter.notifyItemRangeChanged(position, 1);
        } else if (stateMode == MODE_SHOW_FOUR) {

            adapterShowFour.notifyItemRangeChanged(position, 1);
        }

    }

    private void refreshBigVideoView() {

        SurfaceView surfaceV = RtcEngine.CreateRendererView(getApplicationContext());
        mParentView.removeView(mUserItem.getSurfaceView());
        mParentView.addView(surfaceV, 0);
        mUserItem.setSurfaceView(surfaceV);

        //bit window
        if (config().mUid == mUserItem.getUid()) {

            rtcEngine().setupLocalVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, mUserItem.getUid()));
        } else {

            requestRemoteStreamType(mUserItem.getUid(), Constants.VIDEO_STREAM_HIGH);
            rtcEngine().setupRemoteVideo(new VideoCanvas(surfaceV, VideoCanvas.RENDER_MODE_HIDDEN, mUserItem.getUid()));

        }

    }

    private void requestRemoteStreamType(final int uid, final int streamtype) {
        log.debug("requestRemoteStreamType uid : " + uid);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {

                rtcEngine().setRemoteVideoStreamType(uid, streamtype);//Constants.VIDEO_STREAM_LOW

            }
        }, 500);
    }


    @Override
    public void onMuteClick(int position) {
        log.debug("onMuteClick position:" + position);
        if (position >= 0 && position < userItemList.size()) {
            userItemList.get(position).setmIsMuteStream(true);
            rtcEngine().muteRemoteAudioStream(userItemList.get(position).getUid(), true);
            rtcEngine().muteRemoteVideoStream(userItemList.get(position).getUid(), true);
        }

    }

    @Override
    public void onPlayClick(int position) {
        log.debug("onPlayClick position:" + position);
        if (position >= 0 && position < userItemList.size()) {
            userItemList.get(position).setmIsMuteStream(false);
            int uid = userItemList.get(position).getUid();
            rtcEngine().muteRemoteAudioStream(uid, false);
            rtcEngine().muteRemoteVideoStream(uid, false);
            requestRemoteStreamType(uid, Constants.VIDEO_STREAM_LOW);
        }

    }

    @Override
    public void onSurfaceDoubleClick(int position) {

        log.debug("onSurfaceDoubleClick position:" + position);

        if (position >= 0 && position < userItemList.size()) {

            //mute state unable change view
            if (!userItemList.get(position).ismIsMuteStream()) {

                int tempuid = mUserItem.getUid();
                mUserItem.setUid(userItemList.get(position).getUid());

                userItemList.get(position).setUid(tempuid);

                refreshBigVideoView();
                refreshSmallVideoView(position);
            }

        }

    }

    private boolean isExitUid(int uid) {
        for (int i = 0; i < userItemList.size(); i++) {

            if (userItemList.get(i).getUid() == uid) {
                log.debug("already added to UI, ignore it " + (uid & 0xFFFFFFFFL) + " ");
                return true;
            }
        }
        return false;
    }

    private void initModeState() {
        if (stateMode == MODE_ONE_BIG) {

            if (adapter == null) {
                linearLayoutManager = new RecyclerViewNoBugLinearLayoutManager(this);
                linearLayoutManager.setOrientation(LinearLayoutManager.HORIZONTAL);

                adapter = new LiveRoom17Adapter(this, userItemList, this);
                adapter.setHasStableIds(true);
            }
            recyclerView.setLayoutManager(linearLayoutManager);
            recyclerView.setAdapter(adapter);

        } else if (stateMode == MODE_SHOW_FOUR) {

            if (adapterShowFour == null) {

                gridLayoutManager = new GridLayoutManager(this, 1);
                gridLayoutManager.setOrientation(LinearLayoutManager.HORIZONTAL);

                adapterShowFour = new LiveRoomShowFourAdapter(this, userItemList, screenWidth, screenHeight);
                adapterShowFour.setHasStableIds(true);
            }

            resetGridLayoutSpaceCount();
            recyclerView.setAdapter(adapterShowFour);
        }
    }

    private void resetGridLayoutSpaceCount() {
        int spaceCount = 2;
        if (userItemList.size() < 3) {
            spaceCount = 1;
        }
        gridLayoutManager.setSpanCount(spaceCount);
        recyclerView.setLayoutManager(gridLayoutManager);

    }

    private int firstVisibleItem;
    private int visibleItemCount;
    private int allItems;
    private RecyclerView.OnScrollListener scrollListener = new RecyclerView.OnScrollListener() {
        @Override
        public void onScrolled(RecyclerView recyclerView, int dx, int dy) {
            super.onScrolled(recyclerView, dx, dy);

            if (stateMode == MODE_SHOW_FOUR) {
                GridLayoutManager l = (GridLayoutManager) recyclerView.getLayoutManager();
                int tempFirstVisibleItem = l.findFirstVisibleItemPosition();
                int tempVisibleItemCount = l.getChildCount();
                int tempAllItems = l.getItemCount();
                if (tempFirstVisibleItem == firstVisibleItem && tempVisibleItemCount == visibleItemCount && tempAllItems == allItems) {
                    return;
                }
                firstVisibleItem = tempFirstVisibleItem;
                visibleItemCount = tempVisibleItemCount;
                allItems = tempAllItems;
                muteInvisibleItem(firstVisibleItem, visibleItemCount, allItems);
            }

        }
    };

    private void muteInvisibleItem(int firstVisibleItem, int visibleItemCount, int allItems) {
        if (allItems == userItemList.size()) {
            for (int i = 0; i < allItems; i++) {
                UserItem mUserStatusData = userItemList.get(i);
                if (i >= firstVisibleItem && i < firstVisibleItem + visibleItemCount) {
                    rtcEngine().muteRemoteAudioStream(mUserStatusData.getUid(), false);
                    rtcEngine().muteRemoteVideoStream(mUserStatusData.getUid(), false);
                    requestRemoteStreamType(mUserStatusData.getUid(), Constants.VIDEO_STREAM_HIGH);
                } else {
                    rtcEngine().muteRemoteAudioStream(mUserStatusData.getUid(), true);
                    rtcEngine().muteRemoteVideoStream(mUserStatusData.getUid(), true);
                }
            }
        }
    }

}
