package io.agora.csopenduo;

/**
 * Created by beryl on 2017/11/6.
 */

public class Constant {

    public static final int CALL_IN = 0x01;
    public static final int CALL_OUT = 0x02;
    public static final String PHONEACCOUNT_NAME_IN = "Agora OpenDuo";
    public static final String PHONEACCOUNT_NAME_OUT = "Agora Duo";
    public static final String ACCOUNT_NAME = "AccountName";
    public static final String SUBSCRIBER_NAME = "SubscriberName";
    public static final String CHANNEL_NAME = "ChannelName";
    public static final String CALL_TYPE = "CallType";

    private static long timeLast;

    public static boolean isFastlyClick() {
        if (System.currentTimeMillis() - timeLast < 1500) {
            timeLast = System.currentTimeMillis();
            return true;
        } else {
            timeLast = System.currentTimeMillis();
            return false;
        }
    }
}
