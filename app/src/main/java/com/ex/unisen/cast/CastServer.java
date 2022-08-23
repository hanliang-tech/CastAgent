package com.ex.unisen.cast;

import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;


import com.ex.unisen.AppContext;
import com.ex.unisen.activity.VideoPlayerActivity;
import com.ex.unisen.util.Utils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import static com.ex.unisen.cast.CommonUtil.getCastName;

public class CastServer extends Service {
    public static final String TAG = CastServer.class.getSimpleName();

    public static final String START_CAST_ENGINE = "com.unisen.ex.start.engine";
    public static final String RESTART_CAST_ENGINE = "com.unisen.ex.restart.engine";

    private Handler mHandler;
    private static final int START_ENGINE_MSG_ID = 0x0001;
    private static final int RESTART_ENGINE_MSG_ID = 0x0002;

    private static final int DELAY_TIME = 1000;

    public static int isFirstWifiConnect = 1;


    private CastThread castThread;

    private WifiManager.MulticastLock mMulticastLock;

//    private long mServerId = 0;
//    private long mBasePts = 0;
//    private Context mContext;
//    private long byteLen = 0;
//    private long frameCount = 0;
//    private long lastTime = 0;

    static {
        System.loadLibrary("native-lib");
    }

    public static native int startCastServer(String deviceName, String serialNumber, String UUID);

    public static native int stopCastServer();

    public static native int flushDlna();

    public static long getCurrentPosition() {
        return VideoPlayerActivity.getCurrentPosition();
    }

    public static long getDuration() {
        return VideoPlayerActivity.getDuration();
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

    private static void onNEvent(int type, String param1, String param2, String param3) {
        Log.d(TAG, "c to java ---type ::" + type + "---param1 ::" + param1 + "---param2 ::" + param2 + "---param3 ::" + param3);
        Event event = new Event();
        event.setOperate(type);
        switch (type) {
            case Event.CALLBACK_EVENT_ON_SET_AV_TRANSPORT_URI:
                String currentPackage = Utils.getCurrentAppPackage(param2);
                if (Utils.isAppInstalled(AppContext.getInstance(), currentPackage)) {
                    Log.d("xia", "播放");
                    ComponentName componentName = new ComponentName("com.gitvdemo.video", "com.gala.video.app.epg.HomeActivity");
                    Intent intent = new Intent();
                    intent.setComponent(componentName);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    AppContext.getInstance().startActivity(intent);
                    //播放
//                    EventBus.getDefault().post(event);
                } else {
                    Toast.makeText(AppContext.getInstance(), "未安装应用", Toast.LENGTH_LONG).show();
                }
                break;
            case Event.CALLBACK_EVENT_ON_SEEK:
                //拉进度条
                Log.d(TAG, "拉进度条");
                if (!TextUtils.isEmpty(param1)) {
                    int seek = Integer.parseInt(param1);
                    event.setData(seek);
                    EventBus.getDefault().post(event);
                }
                break;
            case Event.CALLBACK_EVENT_ON_SET_VOLUME:
                //设置音量
                Log.d(TAG, "设置音量");
                event.setData(param2);
                EventBus.getDefault().post(event);
                break;
            case Event.CALLBACK_EVENT_ON_PAUSE:
                //暂停
                Log.d(TAG, "暂停");
                EventBus.getDefault().post(event);
                break;
            case Event.CALLBACK_EVENT_ON_PLAY:
                //继续播放
                Log.d(TAG, "继续播放");
                EventBus.getDefault().post(event);
                break;
            case Event.CALLBACK_EVENT_ON_NEXT:
                //下一集
                Log.d(TAG, "下一集");
                EventBus.getDefault().post(event);
                break;
            case Event.CALLBACK_EVENT_ON_PREVIOUS:
                //上一集
                Log.d(TAG, "上一集");
                EventBus.getDefault().post(event);
                break;
            case Event.CALLBACK_EVENT_ON_STOP:
                Log.i("xia","暂停");
                String curPackage = Utils.getCurrentAppPackage("");
                Utils.forceStopPackage(AppContext.getInstance(), curPackage);
                EventBus.getDefault().post(event);
                break;
            case Event.CALLBACK_EVENT_ON_SET_PLAY_MODE:
                //设置播放模式
                Log.d(TAG, "设置播放模式");
                break;
            case Event.CALLBACK_EVENT_ON_SET_MUTE:
                //设置静音
                Log.d(TAG, "设置静音");
                break;
        }
    }


    @Subscribe(threadMode = ThreadMode.MAIN, priority = 100, sticky = false)
    public void onEvent(Event event) {
        Log.d(TAG, "start play");
        switch (event.getOperate()) {
            case Event.CALLBACK_EVENT_ON_SET_AV_TRANSPORT_URI:
                Intent intent = new Intent(this, VideoPlayerActivity.class);
                MediaModel model = (MediaModel) event.getData();
                intent.putExtra(VideoPlayerActivity.KEY_DLNA_DATA, model);
                intent.setFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(intent);
                break;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        initCastService();

    }

    @Override
    public void onDestroy() {
        unInitRenderService();
        super.onDestroy();

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent != null) {
            String actionString = intent.getAction();
            if (actionString != null) {
                if (actionString.equalsIgnoreCase(START_CAST_ENGINE)) {
                    Log.i(TAG, "yzs  onStartCommand start");
                    delayToSendStartMsg();
                } else if (actionString.equalsIgnoreCase(RESTART_CAST_ENGINE)) {
                    Log.i(TAG, "yzs  onStartCommand restart");
                    delayToSendRestartMsg();
                }
            }
        }
        Log.i(TAG, "yzs  onStartCommand");
        return super.onStartCommand(intent, flags, startId);

    }


    private void initCastService() {

        castThread = new CastThread(this);

        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case START_ENGINE_MSG_ID:
                        startEngine();
                        break;
                    case RESTART_ENGINE_MSG_ID:
                        restartEngine();
                        break;
                }
            }

        };

        mMulticastLock = CommonUtil.openWifiBrocast(this);
    }


    private void unInitRenderService() {
        stopEngine();
        removeStartMsg();
        removeRestartMsg();
        if (mMulticastLock != null) {
            mMulticastLock.release();
            mMulticastLock = null;
        }
    }

    private void delayToSendStartMsg() {
        removeStartMsg();
        mHandler.sendEmptyMessageDelayed(START_ENGINE_MSG_ID, DELAY_TIME);
    }

    private void delayToSendRestartMsg() {
        removeStartMsg();
        removeRestartMsg();
        mHandler.sendEmptyMessageDelayed(RESTART_ENGINE_MSG_ID, DELAY_TIME);
        Log.i(TAG, "yzs  delayToSendRestartMsg ");
    }

    private void removeStartMsg() {
        mHandler.removeMessages(START_ENGINE_MSG_ID);
    }

    private void removeRestartMsg() {
        mHandler.removeMessages(RESTART_ENGINE_MSG_ID);
    }


    public boolean startEngine() {
        awakeWorkThread();
        return true;
    }

    public boolean stopEngine() {
        castThread.setParam("", "");
        exitWorkThread();
        return true;
    }

    public boolean restartEngine() {
        String castname = getCastName(this, Utils.getConfigByName(this, "Name"));
        castThread.setParam(castname, "01234567890");
        if (castThread.isAlive()) {
            castThread.restartEngine();
        } else {
            castThread.start();
        }
        return true;
    }

    private void awakeWorkThread() {
        String castname = getCastName(this, Utils.getConfigByName(this, "Name"));
        castThread.setParam(castname, "01234567890");
        if (castThread.isAlive()) {
            castThread.awakeThread();
        } else {
            castThread.start();
        }
    }

    private void exitWorkThread() {
        if (castThread != null && castThread.isAlive()) {

            castThread.exit();
            long time1 = System.currentTimeMillis();
            while (castThread.isAlive()) {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            long time2 = System.currentTimeMillis();
            castThread = null;
        }
    }

}