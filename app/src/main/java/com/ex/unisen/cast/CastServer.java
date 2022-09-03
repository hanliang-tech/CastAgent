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
import com.ex.unisen.enu.App;
import com.ex.unisen.util.Utils;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import static com.ex.unisen.cast.CommonUtil.getCastName;

public class CastServer extends Service {
    public static final String TAG = CastServer.class.getSimpleName();

    public static final String START_CAST_ENGINE = "com.ex.start.engine";
    public static final String RESTART_CAST_ENGINE = "com.ex.restart.engine";

    private Handler mHandler;
    private static final int START_ENGINE_MSG_ID = 0x0001;
    private static final int RESTART_ENGINE_MSG_ID = 0x0002;

    private static final int DELAY_TIME = 1000;

    public static int isFirstWifiConnect = 1;


    private CastThread castThread;

    private WifiManager.MulticastLock mMulticastLock;

    private static App currentApp = null;

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


    private static void onNEvent(int type, String param1, String param2, String param3) {
        Log.d(TAG, "c to java ---type ::" + type + "---param1 ::" + param1 + "---param2 ::" + param2 + "---param3 ::" + param3);
        Event event = new Event();
        event.setOperate(type);
        switch (type) {
            case Event.CALLBACK_EVENT_ON_SET_AV_TRANSPORT_URI:
                if(currentApp != null && currentApp != App.UNKNOW) {
                    Utils.forceStopPackage(AppContext.getInstance(), currentApp.getPackageName());
                }
                currentApp = App.getInstanceWithAgent(param2);
                if(currentApp == App.UNKNOW){
                    Log.d("ruisen", "未知应用");
                    ComponentName componentName = new ComponentName(currentApp.getPackageName(), currentApp.getLaunchActivity());
                    Intent intent = new Intent();
                    intent.setComponent(componentName);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    AppContext.getInstance().startActivity(intent);
                    return;
                }
                if (Utils.isAppInstalled(AppContext.getInstance(), currentApp.getPackageName())) {
                    Log.d("ruisen", "播放");
                    ComponentName componentName = new ComponentName(currentApp.getPackageName(), currentApp.getLaunchActivity());
                    Intent intent = new Intent();
                    intent.setComponent(componentName);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    AppContext.getInstance().startActivity(intent);
                    //播放
//                    EventBus.getDefault().post(event);
                } else {
//                    Toast.makeText(AppContext.getInstance(), "未安装应用", Toast.LENGTH_LONG).show();
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
                Utils.forceStopPackage(AppContext.getInstance(), currentApp.getPackageName());
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
        String castname = AppContext.getInstance().getDeviceName();
        castThread.setParam(castname, "01234567890");
        if (castThread.isAlive()) {
            castThread.restartEngine();
        } else {
            castThread.start();
        }
        return true;
    }

    private void awakeWorkThread() {
        String castname = AppContext.getInstance().getDeviceName();
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
