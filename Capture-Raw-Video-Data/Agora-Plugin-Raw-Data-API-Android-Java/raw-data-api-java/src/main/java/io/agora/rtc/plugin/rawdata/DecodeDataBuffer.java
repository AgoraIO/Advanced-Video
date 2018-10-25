package io.agora.rtc.plugin.rawdata;

import java.nio.ByteBuffer;

/**
 * Created by yt on 2018/4/4/004.
 */

public class DecodeDataBuffer {
    private int uid;
    private ByteBuffer byteBuffer;

    public DecodeDataBuffer(int uid, ByteBuffer byteBuffer) {
        this.uid = uid;
        this.byteBuffer = byteBuffer;
    }

    public int getUid() {
        return uid;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public ByteBuffer getByteBuffer() {
        return byteBuffer;
    }

    public void setByteBuffer(ByteBuffer byteBuffer) {
        this.byteBuffer = byteBuffer;
    }
}
