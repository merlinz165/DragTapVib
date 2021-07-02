package com.example.blenotification;

import android.app.Notification;
import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.PowerManager;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

public class NotificationListener extends NotificationListenerService {

    private static final class ApplicationPackageNames {
        public static final String FACEBOOK_PACK_NAME = "com.facebook.katana";
        public static final String FACEBOOK_MESSENGER_PACK_NAME = "com.facebook.orca";
        public static final String GOOGLECHAT_PACK_NAME = "com.google.android.apps.dynamite";
        public static final String OTHER_APP = "other";
    }

    private static final class InterceptedNotificationCode {
        public static final int FACEBOOK_CODE = 1;
        public static final int GOOGLECHAT_CODE = 2;
        public static final int OTHER_NOTIFICATIONS_CODE = 4;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return super.onBind(intent);
    }

    @Override
    public void onNotificationPosted(StatusBarNotification sbn) {
        int notificationCode = matchNotificationCode(sbn);

        if(notificationCode != InterceptedNotificationCode.OTHER_NOTIFICATIONS_CODE) {
            Intent intent = new Intent(String.valueOf(R.string.notification_intent_action));
            intent.putExtra("Notification Code", notificationCode);
            sendBroadcast(intent);
        }
    }

    private int matchNotificationCode(StatusBarNotification sbn) {
        String packageName = sbn.getPackageName();

        if (packageName.equals(ApplicationPackageNames.FACEBOOK_PACK_NAME) || packageName.equals(ApplicationPackageNames.FACEBOOK_MESSENGER_PACK_NAME)) {
            return(InterceptedNotificationCode.FACEBOOK_CODE);
        } else if (packageName.equals(ApplicationPackageNames.GOOGLECHAT_PACK_NAME)) {
            return (InterceptedNotificationCode.GOOGLECHAT_CODE);
        }
        else {
            return(InterceptedNotificationCode.OTHER_NOTIFICATIONS_CODE);
        }
    }
}
