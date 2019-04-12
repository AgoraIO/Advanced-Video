package io.agora.cs;

import android.content.Context;
import android.telecom.PhoneAccount;

/**
 * Created by minming on 2019/04/08.
 */
public class CallSession {
    private PhoneAccount mPAOut;
    private PhoneAccount mPAIn;
    private AgoraConnection mAC;
    private static volatile CallSession mInstance = null;
    private Context mContext;

    public static CallSession getInstance() {
        if (mInstance == null) {
            synchronized (CallSession.class) {
                if(mInstance == null) {
                    mInstance = new CallSession();
                }
            }
        }
        return  mInstance;
    }

    private CallSession() {

    }

    public void setContext (Context context) {
        mContext = context;
    }
    public Context getContext() {
        return mContext;
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
