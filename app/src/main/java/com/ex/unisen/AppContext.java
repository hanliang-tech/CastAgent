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

public class AppContext extends Application {

    private NetWorkChangReceiver gChangReceiver = null;
    private static AppContext instance;

    public static AppContext getInstance() {
        return instance;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
//        Utils.forceStopAllTvVideo(this);
        Intent mIntent = new Intent();
        mIntent.setAction(CastServer.START_CAST_ENGINE);
        mIntent.setPackage("com.ex.unisen.cast");
        startService(mIntent);
        String name = Utils.getConfigByName(this,"Name");
        Log.i("xia","config name ::" + name);
        CastServer.startCastServer(CommonUtil.getCastName(this, Utils.getConfigByName(this,"Name")),String.valueOf(9000), UUID.randomUUID().toString());
    }


    public void onDestroy() {
        Log.i("xia", "yzs onDestroy");
        /*
        if (gChangReceiver != null) {
            unregisterReceiver(gChangReceiver);
        }*/
    }

}
