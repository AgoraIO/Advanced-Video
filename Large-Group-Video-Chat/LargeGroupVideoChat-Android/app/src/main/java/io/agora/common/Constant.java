package io.agora.common;

import io.agora.rtc.RtcEngine;

public class Constant {

    public static final String MEDIA_SDK_VERSION;

    static {
        String sdk = "undefined";
        try {
            sdk = RtcEngine.getSdkVersion();
        } catch (Throwable e) {
        }
        MEDIA_SDK_VERSION = sdk;
    }

    public static float PRP_DEFAULT_LIGHTNESS = .65f;
    public static float PRP_DEFAULT_SMOOTHNESS = 1.0f;

    public static int SMALL_SURFACE_WIDTH;
    public static float SCREEN_PRECENT;

}
