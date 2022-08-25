package com.ex.unisen.activity;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pGroup;
import android.net.wifi.p2p.WifiP2pManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;

import com.ex.unisen.AppContext;
import com.ex.unisen.R;
import com.ex.unisen.cast.CastServer;
import com.ex.unisen.cast.CommonUtil;
import com.ex.unisen.cast.Event;
import com.ex.unisen.util.Utils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.UUID;

// WifiP2pManager.PersistentGroupInfoListener
public class MainActivity extends Activity implements WifiP2pManager.PeerListListener, WifiP2pManager.GroupInfoListener {

    public static final String TAG = "xia";
    private final IntentFilter mIntentFilter = new IntentFilter();
    private WifiP2pManager mWifiP2pManager;
    private WifiP2pManager.Channel mChannel;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.i("xia","BroadcastReceiver :: " + action);
            if(WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)){
                int wifiState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE,
                        WifiManager.WIFI_STATE_UNKNOWN);
                if(WifiManager.WIFI_STATE_ENABLED == wifiState){
//                    updateDevicePref();
                }
            }else if (WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION.equals(action)) {
//                mWifiP2pEnabled = intent.getIntExtra(WifiP2pManager.EXTRA_WIFI_STATE,
//                        WifiP2pManager.WIFI_P2P_STATE_DISABLED) == WifiP2pManager.WIFI_P2P_STATE_ENABLED;
                if(intent.getIntExtra(WifiP2pManager.EXTRA_WIFI_STATE,
                        WifiP2pManager.WIFI_P2P_STATE_DISABLED) == WifiP2pManager.WIFI_P2P_STATE_ENABLED){
                    mWifiP2pManager.requestPeers(mChannel, MainActivity.this);
                }
//                handleP2pStateChanged();
            } else if (WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION.equals(action)) {
                if (mWifiP2pManager != null) {
//                    mWifiP2pManager.requestPeers(mChannel, WifiDisplayActivity.this);
                }
            } else if (WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION.equals(action)) {
//                if (mWifiP2pManager == null) return;
//                NetworkInfo networkInfo = (NetworkInfo) intent.getParcelableExtra(
//                        WifiP2pManager.EXTRA_NETWORK_INFO);
//                if (mWifiP2pManager != null) {
//                    mWifiP2pManager.requestGroupInfo(mChannel, WifiDisplayActivity.this);
//                }
//                mMainHandler.removeCallbacks(mDelaySearchAction);
//                if (networkInfo.isConnected()) {
//                    if (DBG) Log.d(TAG, "Connected");
//                } else {
//                    //start a search when we are disconnected
//                    startSearch();
//                }
            } else if (WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION.equals(action)) {
//                mThisDevice = (WifiP2pDevice) intent.getParcelableExtra(
//                        WifiP2pManager.EXTRA_WIFI_P2P_DEVICE);
//                updateDevicePref();
            } else if (WifiP2pManager.WIFI_P2P_DISCOVERY_CHANGED_ACTION.equals(action)) {
//                int discoveryState = intent.getIntExtra(WifiP2pManager.EXTRA_DISCOVERY_STATE,
//                        WifiP2pManager.WIFI_P2P_DISCOVERY_STOPPED);
//                if (DBG) Log.d(TAG, "Discovery state changed: " + discoveryState);
//                if (discoveryState == WifiP2pManager.WIFI_P2P_DISCOVERY_STARTED) {
//                    updateSearchMenu(true, true);
//                } else {
//                    updateSearchMenu(false, true);
//                }
            }
//            else if (WifiP2pManager.WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION.equals(action)) {
//                if (mWifiP2pManager != null) {
//                    mWifiP2pManager.requestPersistentGroupInfo(mChannel, MainActivity.this);
//                }
//            }
        }
    };

    Button bt;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        mTextureView = findViewById(R.id.surface);
//        mTextureView.setSurfaceTextureListener(this);
//        mAVPlayer = new AVPlayer(this);
//        bt = (Button) findViewById(R.id.bt);
//        bt.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                startEngine();
//            }
//        });
//        startService(new Intent(this, WifiDisplayService.class));
        startEngine();
    }

    private void initContext() {
        mIntentFilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_DISCOVERY_CHANGED_ACTION);
//        mIntentFilter.addAction(WifiP2pManager.WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION);

        mWifiP2pManager = (WifiP2pManager) getSystemService(Context.WIFI_P2P_SERVICE);
        if (mWifiP2pManager != null) {
            mChannel = mWifiP2pManager.initialize(this, getMainLooper(), null);
            if (mChannel == null) {
                //Failure to set up connection
                Log.e(TAG, "Failed to set up connection with wifi p2p service");
                mWifiP2pManager = null;
            }
            Log.e(TAG, "mWifiP2pManager init succees !");
        } else {
            Log.e(TAG, "mWifiP2pManager is null !");
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }


    public boolean startEngine() {
        Intent mIntent = new Intent();
        mIntent.setAction(CastServer.START_CAST_ENGINE);
        mIntent.setPackage("com.ex.unisen.cast");
        getApplication().startService(mIntent);
//        Intent serviceIntent = new Intent();
////        serviceIntent.setClass(this, WifiDisplayService.class);
////        getApplication().startService(serviceIntent);
////        initContext();
        String name = Utils.getConfigByName(this,"Name");
        Log.i("xia","config name ::" + name);
        CastServer.startCastServer(AppContext.getInstance().getDeviceName(),String.valueOf(9000), UUID.randomUUID().toString());
        return true;
    }

    public boolean stopEngine() {
//        getApplication().stopService(new Intent(getApplication(), CastServer.class));
        return true;
    }

    public boolean restartEngine() {
//        Intent mIntent = new Intent();
//        mIntent.setAction(CastServer.RESTART_CAST_ENGINE);
//        mIntent.setPackage("com.ex.unisen.cast");
//        getApplication().startService(mIntent);
        return true;
    }

//    @Override
//    protected void onResume() {
//        super.onResume();
////        mNetworkDetecting.detectWifiEnable();
//        registerReceiver(mReceiver, mIntentFilter);
//        //若直接设置为true，在弹出连接对话框时，会进行research
////        mMainHandler.postDelayed(mSetShowingUIAction, 1000);
//    }
//
//
//    @Override
//    protected void onPause() {
//        super.onPause();
////        mWifiP2pManager.stopPeerDiscovery(mChannel, null);
////        unregisterReceiver(mReceiver);
//    }

    @Override
    protected void onPause() {
        super.onPause();
//        if (EventBus.getDefault().isRegistered(this)) {
//            EventBus.getDefault().unregister(this);
//        }

    }

    @Override
    protected void onResume() {
        super.onResume();
//        registerReceiver(mReceiver, mIntentFilter);
//        if (!EventBus.getDefault().isRegistered(this)) {
//            EventBus.getDefault().register(this);
//        }
    }

    @Subscribe(threadMode = ThreadMode.MAIN, priority = 100, sticky = false)
    public void onEvent(Event event) {
        switch (event.getOperate()) {
            case Event.CALLBACK_EVENT_ON_SEEK:

                break;
            case Event.CALLBACK_EVENT_ON_STOP:
                //停止播放
                this.finish();
            case Event.CALLBACK_EVENT_ON_PAUSE:
                //暂停
            case Event.CALLBACK_EVENT_ON_PLAY:
                //继续播放
                break;
            case Event.CALLBACK_EVENT_ON_SET_VOLUME:
                //设置音量
                break;
            case Event.CALLBACK_EVENT_ON_SET_MUTE:
                //设置静音
                break;
            case Event.CALLBACK_EVENT_ON_SET_AV_TRANSPORT_URI:
                if(Utils.isAppInstalled(this,"com.gitvdemo.video")) {
                    ComponentName componentName = new ComponentName("com.gitvdemo.video", "com.gala.video.app.epg.HomeActivity");
                    Intent intent = new Intent();
                    intent.setComponent(componentName);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    startActivity(intent);
                }else{
                    Toast.makeText(this,"程序未安装",Toast.LENGTH_LONG).show();
                }
                break;
        }
    }

    @Override
    public void onGroupInfoAvailable(WifiP2pGroup wifiP2pGroup) {
        Log.i(TAG,"onGroupInfoAvailable");
    }

    @Override
    public void onPeersAvailable(WifiP2pDeviceList peers) {
        Log.i(TAG,"onPeersAvailable");
        int mConnectedDevices = 0;
        ArrayList<WifiP2pDevice> deviceList = new ArrayList<WifiP2pDevice>();
        for (WifiP2pDevice peer: peers.getDeviceList()) {
             Log.d(TAG, " peer " + peer);
//            if(WifiDisplayService.isWifiDisplaySource(peer)){
//                deviceList.add(peer);
//                if (peer.status == WifiP2pDevice.CONNECTED) mConnectedDevices++;
//            }
        }
        Log.d(TAG, " mConnectedDevices " + mConnectedDevices);
        startSearch(true);
//        updateDeviceStatus(false);
//        setDeviceAdapter(deviceList);
    }

    private void startSearch(boolean forceSearch) {
        if (mWifiP2pManager != null && (forceSearch)) {
            mWifiP2pManager.discoverPeers(mChannel, new WifiP2pManager.ActionListener() {
                public void onSuccess() {
                    Log.d(TAG, " discover succeed ");
                }
                public void onFailure(int reason) {
                    Log.d(TAG, " discover fail " + reason);
                }
            });
        }
    }


//    public void onRecvVideoData(byte[] nal, int nalType, long dts, long pts, int width, int height) {
//        Log.d(TAG, "onRecvVideoData pts = " + pts + ", nalType = " + nalType + ", width = " + width + ", height = " + height + ", nal length = " + nal.length);
//
//        if (lastTime == 0) {
//            lastTime = System.currentTimeMillis();
//        }
//        byteLen = byteLen + nal.length;
//        frameCount++;
//        long diffTime = System.currentTimeMillis() - lastTime;
//        if (diffTime > 1000) {
//            long fps = (long) (frameCount / ((float) diffTime / 1000));
//            long speed = (long) (byteLen / 1024 / ((float) diffTime / 1000));
//            Log.d("AVSPEED", "fps = " + fps + ", speed = " + speed);
//            lastTime = System.currentTimeMillis();
//            byteLen = 0;
//            frameCount = 0;
//        }
//
//        if (mBasePts == 0) {
//            mBasePts = pts;
//        }
//        NALPacket nalPacket = new NALPacket();
//        nalPacket.nalData = nal;
//        nalPacket.nalType = nalType;
//        nalPacket.pts = pts - mBasePts;
//        nalPacket.width = width;
//        nalPacket.height = height;
//        Log.d("AVSYNC", "recv video pts = " + nalPacket.pts);
//        mAVPlayer.addPacker(nalPacket);
//    }
//
//    public void onRecvAudioData(short[] pcm, long pts) {
//        Log.d(TAG, "onRecvAudioData pcm length = " + pcm.length + ", pts = " + pts);
//        PCMPacket pcmPacket = new PCMPacket();
//        pcmPacket.data = pcm;
//        pcmPacket.pts = pts - mBasePts;
//        Log.d("AVSYNC", "recv audio  pts = " + pcmPacket.pts);
//        mAVPlayer.addPacker(pcmPacket);
//    }
//
//    public void updateTextureView(int width, int height) {
//        Log.d(TAG, "updateTextureView width = " + width + ", height = " + height);
//        RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) mTextureView.getLayoutParams();
//        if (width < height) {
//            // 缩放比例
//            int scaleWidth = mScreenWidth;
//            float scale = (float) scaleWidth / width;
//            int scaleHeight = (int) (height * scale);
//            // 竖屏
//            params.width = scaleWidth;
//            params.height = scaleHeight;
//            if (scaleHeight < mScreenHeight) {
//                params.topMargin = mScreenHeight - scaleHeight;
//            }
//            mTextureView.setLayoutParams(params);
//        } else {
//            // 横屏
//            float rate = (float) width / height;
//            int scaleWidth = mScreenWidth;
//            int scaleHeight = (int) (scaleWidth / rate);
//            params.width = scaleWidth;
//            params.height = scaleHeight;
//            params.topMargin = (mScreenHeight - scaleHeight) / 2;
//            mTextureView.setLayoutParams(params);
//        }
//    }
//
//    @Override
//    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
//        Log.d(TAG, "onSurfaceTextureAvailable width = " + width + ", height = " + height);
//        mAVPlayer.setSurface(new Surface(surfaceTexture));
//    }
//
//    @Override
//    public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int width, int height) {
//        Log.d(TAG, "onSurfaceTextureSizeChanged width = " + width + ", height = " + height);
//    }
//
//    @Override
//    public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
//        Log.d(TAG, "onSurfaceTextureDestroyed");
//        return false;
//    }
//
//    @Override
//    public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {
//        //Log.d(TAG, "onSurfaceTextureUpdated");
//    }
}
