package io.agora.calling.with.connection.service.cs;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.provider.CallLog;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.agora.calling.with.connection.service.model.ConstantApp;
import io.agora.calling.with.connection.service.ui.DialerActivity;
import io.agora.calling.with.connection.service.ui.MainActivity;

/**
 * Created by minming on 2019/04/08.
 */
public class AgoraOutgoingCallReceiver extends BroadcastReceiver {
    private final static Logger log = LoggerFactory.getLogger(AgoraOutgoingCallReceiver.class);
    private static boolean DEBUG = true;

    @Override
    public void onReceive(Context context, Intent intent) {
        String subscriber = intent.getStringExtra(Intent.EXTRA_PHONE_NUMBER);

        if (DEBUG) {
            log.debug("PROCESS_OUT_GOING_CALL received, Phone number: " + subscriber);
        }

        if (shouldIntercept(context, subscriber)) {
            if (false) { // DialerActivity.running == true
                Intent agIntent = new Intent(context, DialerActivity.class);
                agIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                agIntent.putExtra(ConstantApp.ACTION_KEY_SUBSCRIBER, subscriber);
                context.startActivity(agIntent);
            } else {
                Intent agIntent = new Intent(context, MainActivity.class);
                agIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP);
                context.startActivity(agIntent);
            }
            setResultData(null);
        }
    }

    private boolean shouldIntercept(Context context, String phoneNumber) {
        Cursor cursor = context.getContentResolver().query(CallLog.Calls.CONTENT_URI,
                null, CallLog.Calls.NUMBER + "=?", new String[]{phoneNumber},
                CallLog.Calls.DATE + " DESC");
        int phoneAccountID = cursor.getColumnIndex(CallLog.Calls.PHONE_ACCOUNT_ID);

        boolean result = false;
        while (cursor.moveToNext()) {
            String phoneAccID = cursor.getString(phoneAccountID);
            if (DEBUG) {
                log.debug("phone number: " + phoneNumber + " phoneAccountID: " + phoneAccID);
            }
            if (phoneAccID.equals(ConstantApp.CALL_IN_CS) ||
                    phoneAccID.equals(ConstantApp.CALL_OUT_CS)) {
                result = true;
                break;
            }
        }
        cursor.close();

        return result;
    }
}
