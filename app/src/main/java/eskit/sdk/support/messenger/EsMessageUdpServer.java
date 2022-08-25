package eskit.sdk.support.messenger;

import org.json.JSONObject;

import java.net.DatagramPacket;
import java.net.InetAddress;

import eskit.sdk.support.messenger.core.AbstractUdpServer;
import eskit.sdk.support.messenger.core.UdpServerConfig;

/**
 * Create by weipeng on 2022/04/15 17:00
 */
public class EsMessageUdpServer extends AbstractUdpServer {

    private static final String TAG = "[-EsMessageUdpServer-]";

    private IUdpMessageCallback mUdpMessageCallback;

    private InetAddress mCurrentAddress;
    private int mCurrentPort;

    public EsMessageUdpServer(IUdpMessageCallback callback) {
        this.mUdpMessageCallback = callback;
    }

    @Override
    protected void onCreateUdpServer(UdpServerConfig config) {
        config.setPortSearchStart(5000);
        super.onCreateUdpServer(config);
    }

    @Override
    protected void onReceiveData(DatagramPacket packet) throws Exception {
        if(this.mUdpMessageCallback == null) return;
        String data = toString(packet);
        JSONObject jo;
        if ((jo = toJson(data)) == null) return;
        this.mUdpMessageCallback.onReceiveUdpMessage(packet, jo);
    }

//    public void sendEvent(String event, String data) {
//        if (mCurrentAddress != null && mCurrentPort > 0) {
//            Executors.get().execute(() -> {
//                try {
//                    JSONObject result = new JSONObject();
//                    result.put("type", CMD_EVENT);
//                    JSONObject jo = new JSONObject();
//                    jo.put("action", event);
//                    jo.put("args", data);
//                    result.put("data", jo);
//                    byte[] bytes = result.toString().getBytes("UTF-8");
//                    send(new DatagramPacket(bytes, bytes.length, mCurrentAddress, mCurrentPort));
//                } catch (Exception e) {
//                    e.printStackTrace();
//                }
//            });
//        }
//
//    }

    public interface IUdpMessageCallback {
        void onReceiveUdpMessage(DatagramPacket packet, JSONObject jo) throws Exception;
    }

}
