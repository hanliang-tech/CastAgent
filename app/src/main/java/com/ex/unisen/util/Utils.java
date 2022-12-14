package com.ex.unisen.util;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.util.Log;

import androidx.annotation.Nullable;


import com.ex.unisen.cast.CommonUtil;
import com.ex.unisen.enu.App;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;

import eu.chainfire.libsuperuser.Shell;

public final class Utils {

    //    爱奇艺、优酷、腾讯、哔哩哔哩、乐播

    public static final String CONFIG_SERVER_NAME = "Name";

    private static String currentPackage = "";

    public static <T> T castNonNull(@Nullable T value) {
        return value;
    }

    /**
     * 配置文件读取
     *
     * @param context
     * @param name
     * @return
     */
    public static String getConfigByName(Context context, String name) {
        try {
            String fileName = "app-config";
            InputStreamReader inputReader = new InputStreamReader(context.getResources().getAssets().open(fileName));
            BufferedReader bufReader = new BufferedReader(inputReader);
            String line = "";
            String Result = "";
            while ((line = bufReader.readLine()) != null) {
                Result += line;
                if (line.startsWith(name)) {
                    String[] object = line.split(":");
                    return object[1];
                } else {
                    continue;
                }
            }
            return Result;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }

    /**
     * 通过包名，判断是否安装客户端，如：
     *
     * @param context     上下文对象
     * @param packageName 包名
     * @return
     */
    public static boolean isAppInstalled(Context context, String packageName) {
        final PackageManager packageManager = context.getPackageManager();// 获取packagemanager
        List<PackageInfo> pinfo = packageManager.getInstalledPackages(0);// 获取所有已安装程序的包信息
        if (pinfo != null) {
            for (int i = 0; i < pinfo.size(); i++) {
                String pn = pinfo.get(i).packageName;
                if (pn.equals(packageName)) {
                    return true;
                }
            }
        }
        return false;
    }


    /**
     * 根据包名杀死某个程序
     *
     * @param context
     * @param packageName
     */
    public static void forceStopPackage(Context context, String packageName) {
        try {
            Shell.Threaded shell = Shell.Pool.SU.get();
            try {
                shell.run("am force-stop packageName");
            } finally {
                shell.close();
            }
        } catch (Shell.ShellDiedException e) {
            Log.i("xia", "杀死进程失败");
        }
    }

    public static void forceStopAllTvVideo(Context context) {
        try {
            Shell.Threaded shell = Shell.Pool.SU.get();
            try {
                for(App app:App.values()) {
                    Log.i("xia", "杀死进程" + app.getName());
                    shell.run("am force-stop " + app.getPackageName());
                }
            } finally {
                shell.close();
            }

        } catch (Shell.ShellDiedException e) {
            Log.i("xia", "杀死进程失败 e：" + e.getMessage());
        }
    }

    public static String creat12BitUUID(Context context) {
        String defaultUUID = "123456789abc";

        String mac = CommonUtil.getLocalMacAddress(context);

        mac = mac.replace(":", "");
        mac = mac.replace(".", "");

        if (mac.length() != 12) {
            mac = defaultUUID;
        }
        // 取4位随机数
        mac = mac.substring(mac.length() - 4, mac.length());
        return mac;
    }

}
