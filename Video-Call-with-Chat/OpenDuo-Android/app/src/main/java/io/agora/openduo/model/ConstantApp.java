package io.agora.openduo.model;


import io.agora.rtc.video.VideoEncoderConfiguration;

public class ConstantApp {
    public static final String APP_BUILD_DATE = "today";

    public static final int BASE_VALUE_PERMISSION = 0X0001;
    public static final int PERMISSION_REQ_ID_RECORD_AUDIO = BASE_VALUE_PERMISSION + 1;
    public static final int PERMISSION_REQ_ID_CAMERA = BASE_VALUE_PERMISSION + 2;
    public static final int PERMISSION_REQ_ID_WRITE_EXTERNAL_STORAGE = BASE_VALUE_PERMISSION + 3;

    public static VideoEncoderConfiguration.VideoDimensions VIDEO_DIMENSION = VideoEncoderConfiguration.VD_640x360;

    public static int CALL_IN = 0x01;
    public static int CALL_OUT = 0x02;
    public static int UNKNOWN = -99;

    public static class PrefManager {
        public static final String PREF_PROPERTY_UID = "pOCXx_uid";
    }

    public static final String ACTION_KEY_CHANNEL_NAME = "ecHANEL";
    public static final String ACTION_KEY_UID = "ecUID";
    public static final String ACTION_KEY_SUBSCRIBER = "exSubscriber";
    public static final String ACTION_KEY_REMOTE_INVITATION = "exSurEMOTE_INVITATIONr";
    public static final String ACTION_KEY_MakeOrReceive = "ecxxMakeOrRece";

    public static class AppError {
        public static final int NO_NETWORK_CONNECTION = 3;
    }
}
