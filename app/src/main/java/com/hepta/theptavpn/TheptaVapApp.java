package com.hepta.theptavpn;

import android.app.Application;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.os.Build;
import android.util.Log;

import com.tencent.mmkv.MMKV;

public class TheptaVapApp extends Application {


    @Override
    public void onCreate() {
        super.onCreate();
//        Log.e("TheptaVapApp onCreate",getCallStatck());
        MMKV.initialize(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                    "channel_id",
                    "Channel name",
                    NotificationManager.IMPORTANCE_HIGH
            );
            channel.setDescription("Channel description");
            Log.e("Rzx","MyApp");
            NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
            notificationManager.createNotificationChannel(channel);
        }
    }


}
