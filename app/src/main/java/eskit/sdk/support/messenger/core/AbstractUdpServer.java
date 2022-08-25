package eskit.sdk.support.messenger.core;

import android.util.Log;

import org.json.JSONObject;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

/**
 * Create by weipeng on 2022/04/14 15:47
 */
public abstract class AbstractUdpServer implements Runnable {

    private DatagramSocket mSocket;
    private boolean mRunning;
    private byte[] mBuffer;
    private int mPortSearchStart;

    public void start() {
        mRunning = true;
        Executors.get().execute(this);
    }

    public void stop() {
        if (isRunning()) {
            if (mSocket != null) {
                try {
                    mSocket.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        mSocket = null;
    }

    public void send(DatagramPacket packet) throws IOException {
        if (mSocket == null) return;
        mSocket.send(packet);
    }

    public boolean isRunning() {
        return mRunning;
    }

    protected void onCreateUdpServer(UdpServerConfig config) {
        mBuffer = new byte[config.getBufferSize()];
        mPortSearchStart = config.getPortSearchStart();
    }

    protected abstract void onReceiveData(DatagramPacket packet) throws Exception;

    @Override
    public void run() {
        onCreateUdpServer(new UdpServerConfig());
        try {
            int port = AvailablePortFinder.getNextAvailable(mPortSearchStart);
            mSocket = new DatagramSocket(port);
            mSocket.setReuseAddress(true);
            Log.d(getClass().getSimpleName(), "start listen on port " + port);
            while (isRunning()) {
                DatagramPacket packet = new DatagramPacket(mBuffer, mBuffer.length);
                mSocket.receive(packet);
                if (packet.getLength() <= 0) continue;
                try {
                    onReceiveData(packet);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }finally {
            mRunning = false;
        }
    }

    protected String toString(DatagramPacket packet) {
        return new String(packet.getData(), 0, packet.getLength());
    }

    protected JSONObject toJson(String data) {
        try {
            return new JSONObject(data);
        } catch (Exception ignored) {
        }
        return null;
    }

}
