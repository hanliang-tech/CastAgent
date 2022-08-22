package com.ex.unisen.cast;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.util.Log;

import com.ex.unisen.activity.VideoPlayerActivity;

public class CastChangeReceiver extends BroadcastReceiver {

    private static final String TAG = CastChangeReceiver.class.getSimpleName();
    public static final String START_MIRACAST_ACTION = "android.intent.action.START_MIRACAST";
    public static final String START_AIRPLAY_DLNA_ACTION = "android.intent.action.START_AIRPLAY_DLNA";
    public static final String STOP_MIRACAST_ACTION = "android.intent.action.STOP_MIRACAST";
    public static final String STOP_AIRPLAY_DLNA_ACTION = "android.intent.action.STOP_AIRPLAY_DLNA";

    private Context context;
    private WifiManager wifi;
    private DisconnectWifi discon;
    public static boolean isMiracast = false;
    public static boolean isAirplaydlna = false;
    @Override
    public void onReceive(Context context, Intent intent) {
        wifi = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        this.context = context;
        String action = intent.getAction();
        Log.d(TAG, "action:::" + action);
        if (START_MIRACAST_ACTION.equals(action)) {
            startMiracast();
        } else if (START_AIRPLAY_DLNA_ACTION.equals(action)) {
            startAirplayDlna();
        } else if (STOP_MIRACAST_ACTION.equals(action)) {
            stopMiracast();
        } else if (STOP_AIRPLAY_DLNA_ACTION.equals(action)) {
            stopAirplayDlan();
        }
    }

    public class DisconnectWifi extends BroadcastReceiver {
        @Override
        public void onReceive(Context c, Intent intent) {
            if (!intent.getParcelableExtra(WifiManager.EXTRA_NEW_STATE).toString().equals(SupplicantState.SCANNING.toString())) {
                try {
                    if (isMiracast) {
                        Log.d(TAG, "onReceive: 1");
                        Thread.sleep(500);
                        wifi.disconnect();
                    }
                } catch (Exception e) {
                }
            }
        }
    }

    private void startMiracast() {
        Log.d(TAG, "startMiracast isMiracast= "+ isMiracast);
        try {
            if(isMiracast==false)
                isMiracast = true;
            else if(isMiracast==true)
                return;
            stopAirplayDlan();
            //discon = new DisconnectWifi();
            //IntentFilter filter = new IntentFilter(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
            //context.getApplicationContext().registerReceiver(discon, filter);
           // SystemProperties.set("persist.sys.boe.wifidisconnect","1");
            if (wifi != null) {
                wifi.disconnect();
            }
            String propvalue = SystemProperties.get("persist.sys.boe.wifidisconnect","0");
            if ("0".equals(propvalue))
            {
                Log.d(TAG, "yzs do not open miracast");
                isMiracast = false;
                return ;
            }
//            context.startServiceAsUser(new Intent(context, WifiDisplayService.class), UserHandle.CURRENT);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void stopMiracast() {
        Log.d(TAG, "stopMiracast isMiracast= "+isMiracast);
        if(isMiracast==true)
            isMiracast = false;
        else if(isMiracast==false)
            return;
        /*
        if (discon != null) {
            context.getApplicationContext().unregisterReceiver(discon);
            discon = null;
        }
        */
        //SystemProperties.set("persist.sys.boe.wifidisconnect","0");
        if (wifi != null) {
            wifi.reconnect();
        }
        Intent intent = new Intent(WifiDisplayService.STOP_ACTION);
        intent.putExtra(WifiDisplayService.CMDFROM, WifiDisplayService.CMDFROM_WFD);
//        context.sendBroadcastAsUser(intent, UserHandle.ALL);
//        context.stopServiceAsUser(new Intent(context, WifiDisplayService.class), UserHandle.CURRENT);
    }

    private void startAirplayDlna() {
        Log.d(TAG, "startAirplayDlna isAirplaydlna= "+isAirplaydlna);
        try {
            if(isAirplaydlna==false)
                isAirplaydlna = true;
            else if(isAirplaydlna==true)
                return;
            stopMiracast();
            int count = 30;// 阻塞30秒检测ip
            String ip = null;
            while (count-- > 0 && ip == null) {
                ip = CommonUtil.getIpAddress(context);
                if (wifi != null) {
                    wifi.reconnect();
                }
                Thread.sleep(1000);
                Log.d(TAG, "startAirplayDlna1: " + count + "   ip:" + ip);
            }
            if(ip == null)
            {
                Log.d(TAG, "startAirplayDlna no ip ");
                isAirplaydlna = false;
                return;
            }
            Log.d(TAG, "startAirplayDlna2: " + count + "   ip:" + ip);
            Intent intent = new Intent();
            intent.setAction(CastServer.RESTART_CAST_ENGINE);
            intent.setPackage("com.ex.unisen.cast");
//            context.startServiceAsUser(intent, UserHandle.CURRENT);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void stopAirplayDlan() {
        Log.d(TAG, "stopAirplayDlan isAirplaydlna = "+isAirplaydlna);
        if(isAirplaydlna==true)
            isAirplaydlna = false;
        else if(isAirplaydlna==false)
            return;
        if (VideoPlayerActivity.playActivity != null)
        {
            VideoPlayerActivity.playActivity.finish();
        }
//        context.stopServiceAsUser(new Intent(context, CastServer.class), UserHandle.CURRENT);
    }
}