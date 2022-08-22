package com.ex.unisen.cast;

import android.content.Context;
import android.util.Log;


public class CastThread extends Thread {

    private static final int CHECK_INTERVAL = 30 * 1000;
    protected static final String TAG = CastThread.class.getSimpleName();
    private Context mContext = null;
    private boolean mStartSuccess = false;
    private boolean mExitFlag = false;

    private String mFriendName = "M3";
    private String mUUID = "12345678";

    public CastThread(Context context) {
        mContext = context;
    }

    public void setFlag(boolean flag) {
        mStartSuccess = flag;
    }

    public void setParam(String friendName, String uuid) {
        mFriendName = friendName;
        mUUID = uuid;
    }

    public void awakeThread() {
        synchronized (this) {
            notifyAll();
        }
    }

    public void exit() {
        mExitFlag = true;
        awakeThread();
    }

    @Override
    public void run() {


        while (true) {
            Log.i(TAG, "yzs  CastThread enter");
            if (mExitFlag) {
                Log.i(TAG, "yzs  CastThread enter1");
                stopEngine();
                break;
            }
            Log.i(TAG, "yzs  CastThread enter2");
            refreshNotify();
            Log.i(TAG, "yzs  CastThread enter3");
            synchronized (this) {
                try {
                    wait(CHECK_INTERVAL);
                    Log.i(TAG, "yzs  CastThread enter4");
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            Log.i(TAG, "yzs  CastThread enter5" + mExitFlag);
            if (mExitFlag) {
                Log.i(TAG, "yzs  CastThread enter6");
                stopEngine();
                Log.i(TAG, "yzs  CastThread enter7");
                break;
            }
        }

    }

    public void refreshNotify() {
        if (!CommonUtil.checkNetworkState(mContext)){
        	return ;
        }
        Log.i(TAG, "yzs  mStartSuccess" + mStartSuccess);
        if (!mStartSuccess) {
            stopEngine();
            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            Log.i(TAG, "yzs  ret");
            boolean ret = startEngine();
            Log.i(TAG, "yzs  ret" + ret);
            if (ret) {
                mStartSuccess = true;
                Log.i(TAG, "yzs  mStartSuccess" + mStartSuccess);
            }
        }

    }

    public boolean startEngine() {
        if (mFriendName.length() == 0) {
            return false;
        }
        Log.i(TAG, "yzs mFriendName" + " and "+CommonUtil.getSerialNumber(mContext) + " and " + CommonUtil.getDevUUID(mContext));
        int ret = CastServer.startCastServer(mFriendName, CommonUtil.getSerialNumber(mContext),CommonUtil.getDevUUID(mContext));
        boolean result = (ret == 0 ? true : false);
        Log.i(TAG,"yzs startEngine end");
        return result;
    }

    public boolean stopEngine() {
        Log.i(TAG, "yzs  stopEngine enter");
        CastServer.stopCastServer();
        Log.i(TAG, "yzs  stopEngine exit");
        return true;
    }

    public boolean restartEngine() {
        setFlag(false);
        Log.i(TAG, "yzs  restartEngine111 ");
        awakeThread();
        Log.i(TAG, "yzs  restartEngine222 ");
        return true;
    }

}
