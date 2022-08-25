package com.ex.unisen;

import android.app.Application;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.view.KeyEvent;

import com.ex.unisen.cast.CastServer;
import com.ex.unisen.cast.CommonUtil;
import com.ex.unisen.cast.NetWorkChangReceiver;
import com.ex.unisen.util.Utils;

import java.util.UUID;

import eskit.sdk.support.messenger.EsMessenger;
import eskit.sdk.support.messenger.IEsMessenger;

public class AppContext extends Application {

    private NetWorkChangReceiver gChangReceiver = null;
    private static AppContext instance;
    private String deviceName;
    IEsMessenger esMessenger

    public static AppContext getInstance() {
        return instance;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
        esMessenger = EsMessenger.get();
        deviceName = Utils.getConfigByName(this, "Name") + Utils.creat12BitUUID(this);
        esMessenger.start(this, new IEsMessenger.MessengerCallback() {

            @Override
            public String getDeviceName() {
                return deviceName;
            }

            @Override
            public void onReceiveStartEsEvent(String jsonData) {
                Log.i("rui", "jsonData::" + jsonData);
            }

            @Override
            public void onStartSuccess() {

            }

            @Override
            public void onStartFailed(Exception e) {

            }
        });
//        Utils.forceStopAllTvVideo(this);

    }


    public String getDeviceName() {
        if (deviceName != null)
            return deviceName;
        else
            return Utils.getConfigByName(this,"Name") + Utils.creat12BitUUID(this);
    }

    public void onDestroy() {
        esMessenger.stop(this);
        /*
        if (gChangReceiver != null) {
            unregisterReceiver(gChangReceiver);
        }*/
    }

}
