package com.giovannizilli.pebblestrava.app;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import java.util.UUID;

public class DataReceiver extends com.getpebble.android.kit.PebbleKit.PebbleDataReceiver {
    private static final int STRAVA_TOGGLE = 5;
    private static final int STRAVA_START = 1;
    private static final int STRAVA_STOP = 2;

    private static final String TAG = "PB-PebbleDataReceiver";
    private static final UUID MYSTRAVA_PEBBLE_APP_UUID = UUID.fromString("45bd95bf-0b73-4e94-b03e-dca552f24d25");

    public DataReceiver() {
        super(MYSTRAVA_PEBBLE_APP_UUID);
    }

    @Override
    public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
        final int cmd = data.getUnsignedIntegerAsLong(1).intValue();

        PebbleKit.sendAckToPebble(context, transactionId);

        Intent intent = new Intent(Intent.ACTION_RUN);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra("show_activity",false);
        switch (cmd) {
            case STRAVA_START_RUN:
                intent.setData(Uri.parse("http://strava.com/nfc/record"));
                intent.putExtra("rideType","Run");
                break;
            case STRAVA_START_RIDE:
                intent.setData(Uri.parse("http://strava.com/nfc/record"));
                intent.putExtra("rideType","Ride");
                break;
            case STRAVA_STOP:
                intent.setData(Uri.parse("http://strava.com/nfc/record/stop"));
                break;
        }
        context.startActivity(intent);
    }

}
