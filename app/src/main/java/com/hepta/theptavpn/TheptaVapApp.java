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

    static {
//        Log.e("TheptaVapApp",getCallStatck());
    }

    public static String getCallStatck() {
        Throwable ex = new Throwable();
        StackTraceElement[] stackElements = ex.getStackTrace();
        String result = "";
        for (int i = 4; i < stackElements.length; i++) {
            result = result +"\n" +stackElements[i].getClassName()+"."+stackElements[i].getFileName()+":"+stackElements[i].getMethodName()+"("+stackElements[i].getLineNumber()+")";
        }
        return result;
    }
}
