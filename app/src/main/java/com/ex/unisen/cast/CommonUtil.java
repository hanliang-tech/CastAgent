package com.ex.unisen.cast;


import android.content.Context;
import android.media.AudioManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.TrafficStats;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.MulticastLock;
import android.os.Environment;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;
import android.widget.Toast;

import java.io.InputStream;
import java.lang.reflect.Method;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.UUID;

public class CommonUtil {

    protected static final String TAG = "xia";
    private static String uniqueId = null;
    private static String defaultSerialNumber = "0000000000000000000";

    public static boolean hasSDCard() {
        String status = Environment.getExternalStorageState();
        if (!status.equals(Environment.MEDIA_MOUNTED)) {
            return false;
        }
        return true;
    }

    public static String getRootFilePath() {
        if (hasSDCard()) {
            return Environment.getExternalStorageDirectory().getAbsolutePath() + "/";// filePath:/sdcard/
        } else {
            return Environment.getDataDirectory().getAbsolutePath() + "/data/"; // filePath: /data/data/
        }
    }

    public static boolean checkNetworkState(Context context) {
        boolean netstate = false;
        ConnectivityManager connectivity = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivity != null) {
            NetworkInfo[] info = connectivity.getAllNetworkInfo();
            if (info != null) {
                for (int i = 0; i < info.length; i++) {
                    if (info[i].getState() == State.CONNECTED) {
                        netstate = true;
                        break;
                    }
                }
            }
        }
        return netstate;
    }

    public static String getLocalMacAddress(Context mc) {
        String defmac = "00:00:00:00:00:00";
        InputStream input = null;
        String wifimac = getWifiMacAddress(mc);
        if (null != wifimac) {
            if (!wifimac.equals(defmac))
                return wifimac;
        }
        try {

            ProcessBuilder builder = new ProcessBuilder("busybox", "ifconfig");
            Process process = builder.start();
            input = process.getInputStream();


            byte[] b = new byte[1024];
            StringBuffer buffer = new StringBuffer();
            while (input.read(b) > 0) {
                buffer.append(new String(b));
            }
            String value = buffer.substring(0);
            String systemFlag = "HWaddr ";
            int index = value.indexOf(systemFlag);
            //List <String> address   = new ArrayList <String> ();
            if (0 < index) {
                value = buffer.substring(index + systemFlag.length());
                //address.add(value.substring(0,18));
                defmac = value.substring(0, 17);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return defmac;
    }

    public static String getWifiMacAddress(Context mc) {
        WifiManager wifi = (WifiManager) mc.getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = wifi.getConnectionInfo();
        return info.getMacAddress();
    }

    public static MulticastLock openWifiBrocast(Context context) {
        MulticastLock multicastLock = null;
        try {
            WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
            multicastLock = wifiManager.createMulticastLock("MediaRender");
            if (multicastLock != null) {
                multicastLock.acquire();
            }
        } catch (Exception e) {
        }
        return multicastLock;
    }


    public static void setCurrentVolume(int percent, Context mc) {
        AudioManager am = (AudioManager) mc.getSystemService(Context.AUDIO_SERVICE);
        int maxvolume = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        am.setStreamVolume(AudioManager.STREAM_MUSIC, (maxvolume * percent) / 100,
                AudioManager.FLAG_PLAY_SOUND | AudioManager.FLAG_SHOW_UI);
        am.setMode(AudioManager.MODE_INVALID);
    }

    public static void setVolumeMute(Context mc) {
        AudioManager am = (AudioManager) mc.getSystemService(Context.AUDIO_SERVICE);
        am.setStreamMute(AudioManager.STREAM_MUSIC, true);
    }

    public static void setVolumeUnmute(Context mc) {
        AudioManager am = (AudioManager) mc.getSystemService(Context.AUDIO_SERVICE);
        am.setStreamMute(AudioManager.STREAM_MUSIC, false);
    }

    public static void showToask(Context context, String tip) {
        Toast.makeText(context, tip, Toast.LENGTH_SHORT).show();
    }

    public static int getScreenWidth(Context context) {
        WindowManager manager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display display = manager.getDefaultDisplay();
        return display.getWidth();
    }

    public static int getScreenHeight(Context context) {
        WindowManager manager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display display = manager.getDefaultDisplay();
        return display.getHeight();
    }

    public static String md5(String string) {
        if (TextUtils.isEmpty(string)) {
            return "";
        }
        MessageDigest md5 = null;
        try {
            md5 = MessageDigest.getInstance("MD5");
            byte[] bytes = md5.digest(string.getBytes());
            String result = "";
            for (byte b : bytes) {
                String temp = Integer.toHexString(b & 0xff);
                if (temp.length() == 1) {
                    temp = "0" + temp;
                }
                result += temp;
            }
            return result;
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return "";
    }

//    public static ViewSize getFitSize(Context context, PLMediaPlayer mediaPlayer) {
//        int videoWidth = mediaPlayer.getVideoWidth();
//        int videoHeight = mediaPlayer.getVideoHeight();
//        double fit1 = videoWidth * 1.0 / videoHeight;
//
//        int width2 = getScreenWidth(context);
//        int height2 = getScreenHeight(context);
//        double fit2 = width2 * 1.0 / height2;
//
//        double fit = 1;
//        if (fit1 > fit2) {
//            fit = width2 * 1.0 / videoWidth;
//        } else {
//            fit = height2 * 1.0 / videoHeight;
//        }
//
//        ViewSize viewSize = new ViewSize();
//        viewSize.width = (int) (fit * videoWidth);
//        viewSize.height = (int) (fit * videoHeight);
//
//        return viewSize;
//    }

    public static class ViewSize {
        public int width = 0;
        public int height = 0;
    }

    public static boolean getWifiState(Context context) {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        State wifistate = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI).getState();
        if (wifistate != State.CONNECTED) {
            return false;
        }

        State mobileState = cm.getNetworkInfo(ConnectivityManager.TYPE_MOBILE).getState();
        boolean ret = State.CONNECTED != mobileState;
        return ret;
    }


    public static boolean getMobileState(Context context) {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        State wifistate = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI).getState();
        if (wifistate != State.CONNECTED) {
            return false;
        }

        State mobileState = cm.getNetworkInfo(ConnectivityManager.TYPE_MOBILE).getState();
        boolean ret = State.CONNECTED == mobileState;
        return ret;
    }


    private static long m_lSysNetworkSpeedLastTs = 0;
    private static long m_lSystNetworkLastBytes = 0;
    private static float m_fSysNetowrkLastSpeed = 0.0f;

    public static float getSysNetworkDownloadSpeed() {
        long nowMS = System.currentTimeMillis();
        long nowBytes = TrafficStats.getTotalRxBytes();

        long timeinterval = nowMS - m_lSysNetworkSpeedLastTs;
        long bytes = nowBytes - m_lSystNetworkLastBytes;

        if (timeinterval > 0) m_fSysNetowrkLastSpeed = (float) bytes * 1.0f / (float) timeinterval;

        m_lSysNetworkSpeedLastTs = nowMS;
        m_lSystNetworkLastBytes = nowBytes;

        return m_fSysNetowrkLastSpeed;
    }

    public static String getSerialNumber(Context context) {

        String str = getSystemProperties("persist.sys.boe.sn", defaultSerialNumber);
        return str;
    }

    public static String getSystemProperties(String paramString1, String paramString2) {
        try {
            Class clazz = Class.forName("android.os.SystemProperties");
            Method method = clazz.getDeclaredMethod("get", new Class[]{String.class, String.class});
            method.setAccessible(true);
            return (String) method.invoke(clazz, new Object[]{paramString1, paramString2});
        } catch (Exception e) {
            e.printStackTrace();
        }
        return paramString2;
    }

    private static final int SERIAL_NUMBER_LENGTH = "0330613700217370009675".length();

    private static boolean isValidSerialNumber(String paramString) {
        if (isStringInvalid(paramString))
            return false;
        paramString = paramString.trim();
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("\\d{");
        stringBuilder.append(SERIAL_NUMBER_LENGTH);
        stringBuilder.append("}");
        return paramString.matches(stringBuilder.toString());
    }

    public static boolean isStringInvalid(String paramString) {
        return (paramString == null || paramString.trim().length() <= 0);
    }

    public static String getCastName(Context context,String preName)
    {
         //M3-序列号
        String serialNumber;
        String partSerialNumber;

        StringBuilder pre = new StringBuilder(preName);
        serialNumber = getSerialNumber(context);
        partSerialNumber = serialNumber.substring(serialNumber.length() - 4);
        return pre.append(partSerialNumber).toString();

    }

    public static String getDevUUID(Context mContext) {
        synchronized (CommonUtil.class) {
            if (uniqueId == null) {
                final TelephonyManager tm = (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
                final String tmDevice, tmSerial, tmPhone, androidId;
                tmDevice = "" + getLocalMacAddress(mContext);
                tmSerial = "" + getSerialNumber(mContext);
                androidId = "" + android.provider.Settings.Secure.getString(mContext.getContentResolver(), android.provider.Settings.Secure.ANDROID_ID);
                UUID deviceUuid = new UUID(androidId.hashCode(), ((long) tmDevice.hashCode() << 32) | tmSerial.hashCode());
                uniqueId = deviceUuid.toString();
                Log.d(TAG, tmDevice + tmSerial + androidId);
                Log.d(TAG, "uuid = " + uniqueId);
            }
        }
        return uniqueId;
    }

    /**
     * 检测网络是否可用
     *
     * @return
     */
    public static int isNetworkAvailable(Context context) {
        ConnectivityManager connectMgr = (ConnectivityManager) context.getApplicationContext().getSystemService(Context.CONNECTIVITY_SERVICE);
        //NetworkInfo ethNetInfo = connectMgr.getNetworkInfo(ConnectivityManager.TYPE_ETHERNET);
        NetworkInfo wifiNetInfo = connectMgr.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        //NetworkInfo mobileNetInfo = connectMgr.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);

      if (wifiNetInfo != null && wifiNetInfo.isConnected()) {
            return 1;
        } else {
            return 0;
        }
    }

    public static boolean isPicture(String  pInput,
                                    String pImgeFlag) throws Exception{
        // 文件名称为空的场合
        if(TextUtils.isEmpty(pInput)){
            // 返回不和合法
            return false;
        }
        // 获得文件后缀名
        String tmpName = pInput.substring(pInput.lastIndexOf(".") + 1,
                pInput.length());
        // 声明图片后缀名数组
        String imgeArray [][] = {
                {"bmp", "0"}, {"dib", "1"}, {"gif", "2"},
                {"jfif", "3"}, {"jpe", "4"}, {"jpeg", "5"},
                {"jpg", "6"}, {"png", "7"} ,{"tif", "8"},
                {"tiff", "9"}, {"ico", "10"}
        };
        // 遍历名称数组
        for(int i = 0; i<imgeArray.length;i++){
            // 判断单个类型文件的场合
            if(!TextUtils.isEmpty(pImgeFlag)
                    && imgeArray [i][0].equals(tmpName.toLowerCase())
                    && imgeArray [i][1].equals(pImgeFlag)){
                return true;
            }
            // 判断符合全部类型的场合
            if(TextUtils.isEmpty(pImgeFlag)
                    && imgeArray [i][0].equals(tmpName.toLowerCase())){
                return true;
            }
        }
        return false;
    }

    /**
     * 获取ip地址
     *
     * @return ip或null
     */
    public static String getIpAddress(Context context) {
        try {
            WifiManager wifiManager = (WifiManager) context.getApplicationContext().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            assert wifiManager != null;
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();
            int ipAddress = wifiInfo.getIpAddress();
            return defaultIp((ipAddress & 0xFF) + "." +
                    ((ipAddress >> 8) & 0xFF) + "." +
                    ((ipAddress >> 16) & 0xFF) + "." +
                    (ipAddress >> 24 & 0xFF));
        } catch (Exception ignored) {
        }
        return defaultIp("");
    }

    private static String defaultIp(String ip) {
        if ("".equals(ip) || ip == null || "0.0.0.0".equals(ip)) {
            return null;
        }
        return ip;
    }
}


