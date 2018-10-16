package io.agora.rtc.mediaio.app.shareScreen.source;

/**
 * Created by yong on 2017/12/29.
 */

public class ViewCaptureSetting {
    private int fps;
    private int width;
    private int height;
    private int format;

    public int getFps() {
        return fps;
    }

    public void setFps(int fps) {
        this.fps = fps;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public int getFormat() {
        return format;
    }

    public void setFormat(int format) {
        this.format = format;
    }
}
