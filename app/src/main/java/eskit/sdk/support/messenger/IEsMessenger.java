package eskit.sdk.support.messenger;

import android.content.Context;

/**
 * Create by weipeng on 2022/04/27 17:29
 * Describe
 */
public interface IEsMessenger {

    public static final int CMD_PING = 0; // 检测服务存活
    public static final int CMD_SEARCH = 1; // 搜索设备
    public static final int CMD_EVENT = 2;  // 发送事件

    // 启动SDK UDP
    void start(Context context, MessengerCallback callback);

    // 关闭SDK UDP
    void stop(Context context);

    interface MessengerCallback {
        // 获取要显示的设备名称
        String getDeviceName();
        // 收到投屏需求
        void onReceiveStartEsEvent(String jsonData);
        // 启动成功
        void onStartSuccess();
        // 启动失败
        void onStartFailed(Exception e);
    }
}
