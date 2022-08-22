package com.ex.unisen.cast;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;


import static android.content.Context.WIFI_SERVICE;

/**
 * Created by yizhusong on 2019/9/28.
 */

public class NetWorkChangReceiver extends BroadcastReceiver {

	public static  String wifiname ="0";
    public NetWorkChangReceiver(Context context) {

    }
        private String getConnectionType(int type) {
            String connType = "";
            if (type == ConnectivityManager.TYPE_MOBILE) {
                connType = "3G��������";
            } else if (type == ConnectivityManager.TYPE_WIFI) {
                connType = "WIFI����";
            }
            return connType;
        }

    public String getWiFiName(Context context) {
        WifiManager wm = (WifiManager) context.getSystemService(WIFI_SERVICE);
        if (wm != null) {
            WifiInfo winfo = wm.getConnectionInfo();
            if (winfo != null) {
                String s = winfo.getSSID();
                if (s.length() > 2 && s.charAt(0) == '"' && s.charAt(s.length() - 1) == '"') {
                    return s.substring(1, s.length() - 1);
                }
            }
        }
        return "Wifi 0";
    }

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            System.out.println("yzs -----------networktest link action---------"+ action);
            if ( ConnectivityManager.CONNECTIVITY_ACTION.equals(action)
                    || "android.net.conn.CONNECTIVITY_CHANGE".equals(action)) {

                switch (CommonUtil.isNetworkAvailable(context)) {

                    case 1:
                        System.out.println("yzs -----------networktest link ---------wifi");
                        context.sendBroadcast(new Intent(CastChangeReceiver.START_AIRPLAY_DLNA_ACTION));
                        System.out.println("yzs -----------restart castserver success ---------");
                        break;
                    case 0:
                        context.sendBroadcast(new Intent(CastChangeReceiver.START_MIRACAST_ACTION));
                        System.out.println("yzs-----------networktest unlink---------wifi");
                        break;
                    default:
                        break;
                }
            }
        }
}
