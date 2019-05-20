package io.agora.switchlive.rtc;

import io.agora.rtc.video.VideoEncoderConfiguration;

public class EngineConfig {
    public int mClientRole;

    public VideoEncoderConfiguration.VideoDimensions mVideoDimension;

    public int mUid;

    public String mChannel;

    public void reset() {
        mChannel = null;
    }

    EngineConfig() {
    }
}
