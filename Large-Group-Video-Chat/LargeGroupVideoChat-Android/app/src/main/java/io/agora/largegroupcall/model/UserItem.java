package io.agora.largegroupcall.model;

import android.view.SurfaceView;

/**
 * Created by beryl on 2017/10/20.
 */

public class UserItem {

    private int uid;
    private SurfaceView surfaceView;
    private boolean isFullScreen = false;
    private boolean mIsMuteStream = true;
    private boolean mIsUserSelf = false;

    public UserItem(int uid , SurfaceView surfaceView , boolean isFullScreen){
        this.uid = uid;
        this.surfaceView = surfaceView;
        this.isFullScreen = isFullScreen;
    }

    public int getUid() {
        return uid;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public SurfaceView getSurfaceView() {
        return surfaceView;
    }

    public void setSurfaceView(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
    }

    public boolean isFullScreen() {
        return isFullScreen;
    }

    public void setFullScreen(boolean fullScreen) {
        isFullScreen = fullScreen;
    }

    public boolean ismIsMuteStream() {
        return mIsMuteStream;
    }

    public void setmIsMuteStream(boolean mIsMuteStream) {
        this.mIsMuteStream = mIsMuteStream;
    }

    public boolean ismIsUserSelf() {
        return mIsUserSelf;
    }

    public void setmIsUserSelf(boolean mIsUserSelf) {
        this.mIsUserSelf = mIsUserSelf;
    }
}
