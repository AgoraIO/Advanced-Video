package io.agora.calling.with.connection.service.cs;

import android.telecom.PhoneAccount;

/**
 * Created by minming on 2019/04/08.
 */
public class CallSession {
    private PhoneAccount mPAOut;
    private PhoneAccount mPAIn;
    private AgoraConnection mAC;
    private static volatile CallSession mInstance = null;

    public static CallSession getInstance() {
        if (mInstance == null) {
            synchronized (CallSession.class) {
                if (mInstance == null) {
                    mInstance = new CallSession();
                }
            }
        }
        return mInstance;
    }

    private CallSession() {

    }

    public void setPhoneAccountIn(PhoneAccount pa) {
        mPAIn = pa;
    }

    public void setPhoneAccountOut(PhoneAccount pa) {
        mPAOut = pa;
    }

    public PhoneAccount getPhoneAccountIn() {
        return mPAIn;
    }

    public PhoneAccount getPhoneAccountOut() {
        return mPAOut;
    }

    public void setConnection(AgoraConnection ac) {
        mAC = ac;
    }

    public AgoraConnection getConnection() {
        return mAC;
    }

}
