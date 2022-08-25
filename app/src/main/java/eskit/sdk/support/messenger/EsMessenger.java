package eskit.sdk.support.messenger;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.text.TextUtils;
import android.util.Log;

import org.json.JSONObject;

import java.net.DatagramPacket;

import eskit.sdk.support.messenger.util.EsUtils;

/**
 * Create by weipeng on 2022/04/27 17:35
 * Describe
 */
public class EsMessenger implements IEsMessenger, EsMessageUdpServer.IUdpMessageCallback {

    private static final String TAG = "[-EsMessenger-]";

    private Context mContext;
    private MessengerCallback mCallback;
    private EsMessageUdpServer mUdpServer;

    @Override
    public void start(Context context, MessengerCallback callback) {
        Log.d(TAG, "start");
        this.mContext = context.getApplicationContext();
        this.mCallback = callback;

        if (mUdpServer == null || !mUdpServer.isRunning()) {
            Log.d(TAG, "create udp");
            mUdpServer = new EsMessageUdpServer(this);
            mUdpServer.start();
        }
    }

    @Override
    public void onReceiveUdpMessage(DatagramPacket packet, JSONObject jo) throws Exception {
        int type = jo.getInt("type");
        switch (type) {
            case CMD_PING: {
                Log.d(TAG, "ping");
                JSONObject data = new JSONObject();
                data.put("type", 0);
                data.put("pkg", mContext.getPackageName());
                byte[] bytes = data.toString().getBytes("UTF-8");
                mUdpServer.send(new DatagramPacket(bytes, bytes.length, packet.getAddress(), packet.getPort()));
                break;
            }
            case CMD_SEARCH:
                Log.d(TAG, "search");
                if (mCallback != null && mUdpServer != null) {
                    try {
                        JSONObject result = new JSONObject();
                        result.put("type", CMD_SEARCH);
                        JSONObject data = new JSONObject();
                        data.put("name", mCallback.getDeviceName());
                        data.put("pkg", mContext.getPackageName());
                        result.put("data", data);
                        byte[] bytes = result.toString().getBytes("UTF-8");
                        mUdpServer.send(new DatagramPacket(bytes, bytes.length, packet.getAddress(), packet.getPort()));
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                break;
            case CMD_EVENT:
                String data = jo.getString("data");
                Log.d(TAG, "start: " + data);

                callbackJsonData(data);

                try {
                    // 在前台运行
                    if (EsUtils.isEsRunning(mContext)) {
                        Log.d(TAG, "es runtime running");
                        EsUtils.insertData(mContext, packet.getAddress().getHostAddress(), packet.getPort(), data);
                    }
                    // 未启动
                    else {
                        Log.d(TAG, "start activity");
                        Intent intent = new Intent();
                        intent.setComponent(new ComponentName("com.extscreen.runtime", "com.extscreen.runtime.activity.RuntimeProxyActivity"));
                        intent.putExtra("data", data);
                        intent.putExtra("ip", packet.getAddress().getHostAddress());
                        intent.putExtra("port", packet.getPort());
                        intent.putExtra("from", mContext.getPackageName());
                        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                        mContext.startActivity(intent);
                    }
                    if (mCallback != null) mCallback.onStartSuccess();
                } catch (Exception e) {
                    e.printStackTrace();
                    if (mCallback != null) mCallback.onStartFailed(e);
                }
                break;
        }
    }

    private void callbackJsonData(String data) {
        if (mCallback == null) return;
        if (TextUtils.isEmpty(data)) return;
        try {
            JSONObject jo = new JSONObject(data);
            String action = jo.optString("action");
            if ("start_es".contains(action)) {
                mCallback.onReceiveStartEsEvent(data);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stop(Context context) {
        Log.d(TAG, "stop");
        if (mUdpServer != null) {
            mUdpServer.stop();
        }
        mUdpServer = null;
        mCallback = null;
        mContext = null;
    }

    //region 单例

    private static final class EsMessengerHolder {
        private static final EsMessenger INSTANCE = new EsMessenger();
    }

    public static IEsMessenger get() {
        return EsMessengerHolder.INSTANCE;
    }

    private EsMessenger() {
    }

    //endregion

}
