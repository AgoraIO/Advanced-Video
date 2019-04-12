package io.agora.cs;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.provider.CallLog;
import android.util.Log;

/**
 * Created by minming on 2019/04/08.
 */
public class AgoraOutgoingCallReceiver extends BroadcastReceiver {
    private static final String TAG = AgoraOutgoingCallReceiver.class.getSimpleName();
    private static boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    @Override
    public void onReceive(Context context, Intent intent) {
        String phoneNumber = intent.getStringExtra(Intent.EXTRA_PHONE_NUMBER);

        if (DEBUG) {
            Log.d(TAG, "PROCESS_OUT_GOING_CALL received, Phone number: " + phoneNumber);
        }

        if (shouldIntercept(context, phoneNumber)) {
            if(NumberCallActivity.running == true) {
                Intent agIntent = new Intent(context, NumberCallActivity.class);
                agIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                agIntent.putExtra("subscriber", phoneNumber);
                context.startActivity(agIntent);
            }else {
                Intent agIntent = new Intent(context, MainActivity.class);
                agIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(agIntent);
            }
            setResultData(null);
        }
    }

    private  boolean shouldIntercept(Context context, String phoneNumber) {
        Cursor cursor = context.getContentResolver().query(CallLog.Calls.CONTENT_URI,
                null, CallLog.Calls.NUMBER + "=?", new String[] {phoneNumber},
                CallLog.Calls.DATE + " DESC");
        int phoneAccountID = cursor.getColumnIndex(CallLog.Calls.PHONE_ACCOUNT_ID);

        boolean result = false;
        while (cursor.moveToNext()) {
            String phoneAccID = cursor.getString(phoneAccountID);
            if (DEBUG) {
                Log.d(TAG, "phone number: " + phoneNumber + " phoneAccountID: " + phoneAccID);
            }
            if(phoneAccID.equals(Constant.PHONEACCOUNT_NAME_IN)||
                    phoneAccID.equals(Constant.PHONEACCOUNT_NAME_OUT)) {
                result = true;
                break;
            }
        }
        cursor.close();

        return result;
    }
}
