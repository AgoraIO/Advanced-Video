package io.agora.switchlive.rtc;

import io.agora.rtc.video.VideoEncoderConfiguration;

public class ConstantApp {
    public static final String APP_BUILD_DATE = "today";

    public static VideoEncoderConfiguration.VideoDimensions[] VIDEO_DIMENSIONS = new VideoEncoderConfiguration.VideoDimensions[] {
            VideoEncoderConfiguration.VD_160x120,
            VideoEncoderConfiguration.VD_320x180,
            VideoEncoderConfiguration.VD_320x240,
            VideoEncoderConfiguration.VD_640x360,
            VideoEncoderConfiguration.VD_640x480,
            VideoEncoderConfiguration.VD_1280x720
    };

    public static final int DEFAULT_PROFILE_IDX = 4; // default use 480P

    public static class PrefManager {
        public static final String PREF_PROPERTY_PROFILE_IDX = "pref_profile_index";
        public static final String PREF_PROPERTY_UID = "pOCXx_uid";
    }

    public static final String ACTION_CHANNEL_LIST = "CHANNEL_LIST";
    public static final String ACTION_CURRENT_INDEX = "CURRENT_INDEX";
}
