package com.ex.unisen.cast;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.core.app.NotificationCompat;

import com.ex.unisen.activity.MainActivity;

import org.greenrobot.eventbus.EventBus;

import java.util.HashMap;
import java.util.Timer;

public class StartService extends Service {
    protected static final String TAG = "xia";
    private NetWorkChangReceiver gChangReceiver = null;
    private final static int NOTIFICATION_ID = android.os.Process.myPid();
    private AssistServiceConnection mServiceConnection;

    public StartService() {
    }
    @Override
    public void onCreate() {
        // TODO Auto-generated method stub
        Log.i(TAG, "yzs oncreate");
        super.onCreate();

        IntentFilter filter = new IntentFilter();
        filter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
        gChangReceiver = new NetWorkChangReceiver(this);
        registerReceiver(gChangReceiver,filter);

    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "yzs onDestroy");
        super.onDestroy();
        stopForeground(true);
        if (gChangReceiver != null) {
            unregisterReceiver(gChangReceiver);
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        ///return super.onStartCommand(intent, flags, startId);
        setForeground();
        return Service.START_STICKY;

    }
    // 要注意的是android4.3之后Service.startForeground() 会强制弹出通知栏，解决办法是再
    // 启动一个service和推送共用一个通知栏，然后stop这个service使得通知栏消失。
    private void setForeground() {
        if (Build.VERSION.SDK_INT < 18)
        {
            startForeground(NOTIFICATION_ID, getNotification());
            return;
        }

        if (mServiceConnection == null)
        {
            mServiceConnection = new AssistServiceConnection();
        }
        // 绑定另外一条Service，目的是再启动一个通知，然后马上关闭。以达到通知栏没有相关通知
        // 的效果
        bindService(new Intent(this, AssistService.class), mServiceConnection,
                Service.BIND_AUTO_CREATE);
    }

    private class AssistServiceConnection implements ServiceConnection
    {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Service assistService = ((AssistService.LocalBinder)service)
                    .getService();
            StartService.this.startForeground(NOTIFICATION_ID, getNotification());
            assistService.startForeground(NOTIFICATION_ID, getNotification());
            assistService.stopForeground(true);

            StartService.this.unbindService(mServiceConnection);
            mServiceConnection = null;
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    }

    private Notification getNotification()
    {
        Intent intent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent,
                PendingIntent.FLAG_UPDATE_CURRENT);
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, "")
                .setContentTitle("服务运行于前台")
                .setContentText("service被设为前台进程")
                .setTicker("service正在后台运行...")
                .setPriority(NotificationCompat.PRIORITY_MAX)
                .setWhen(System.currentTimeMillis())
                .setDefaults(NotificationCompat.DEFAULT_ALL)
                .setContentIntent(pendingIntent);
        Notification notification = builder.build();
        notification.flags = Notification.FLAG_AUTO_CANCEL;
        return notification;
    }
}
