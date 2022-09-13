package com.ex.unisen.enu;

import com.ex.unisen.AppContext;
import com.ex.unisen.cast.CommonUtil;
import com.ex.unisen.util.Utils;

/**
 * @Author: exdog
 * @Date: 9/3/22 12:40 PM
 * @Email: love_mobile@163.com
 * @Description:
 *
 */
public enum App
{
    QIYIGUO("奇异果",
            "UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0",
            "com.gitvdemo.video",
            "com.gala.video.app.epg.HomeActivity"),
    BILIBILI("bilibili","Linux/3.0.0 UPnP/1.0 Platinum/1.0.5.13","com.xiaodianshi.tv.yst","com.xiaodianshi.tv.yst.ui.main.MainActivity"),
    YOUKU("优酷","Linux/4.14.186+, UPnP/1.0, Portable SDK for UPnP devices/1.6.20","com.cibn.tv","com.youku.tv.home.activity.HomeActivity"),
    UNKNOW("未知","","com.ex.unisen",".activity.VideoPlayerActivity");
    private String agent;
    private String name;
    private String packageName;
    private String launchActivity;


    public static App getInstanceWithAgent(String agent){

        if(agent.contains("IQIYIDLNA") && Utils.getConfigByName(AppContext.getInstance(),"WhiteList").contains(QIYIGUO.packageName))
            return QIYIGUO;
        if(agent.equals("Linux/3.0.0 UPnP/1.0 Platinum/1.0.5.13") && Utils.getConfigByName(AppContext.getInstance(),"WhiteList").contains(BILIBILI.packageName))
            return BILIBILI;
        if(agent.equals("Linux/4.14.186+, UPnP/1.0, Portable SDK for UPnP devices/1.6.20") && Utils.getConfigByName(AppContext.getInstance(),"WhiteList").contains(YOUKU.packageName))
            return YOUKU;
        return UNKNOW;
    }

    App(String name,String agent,String packageName,String launchActivity) {
        this.name = name;
        this.agent = agent;
        this.packageName = packageName;
        this.launchActivity = launchActivity;
    }

    public String getName() {
        return name;
    }

    public String getAgent() {
        return agent;
    }

    public void setAgent(String agent) {
        this.agent = agent;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getPackageName() {
        return packageName;
    }

    public void setPackageName(String packageName) {
        this.packageName = packageName;
    }

    public String getLaunchActivity() {
        return launchActivity;
    }

    public void setLaunchActivity(String launchActivity) {
        this.launchActivity = launchActivity;
    }
}
