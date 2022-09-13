package com.ex.unisen.activity;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.media.AudioManager;
import android.media.Image;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.ex.unisen.R;
import com.ex.unisen.cast.CastServer;
import com.ex.unisen.cast.AnimationUtil;
import com.ex.unisen.cast.Event;
import com.ex.unisen.cast.MediaModel;
import com.ex.unisen.cast.MediaUtils;
import com.pili.pldroid.player.PLOnCompletionListener;
import com.pili.pldroid.player.PLOnInfoListener;
import com.pili.pldroid.player.PLOnPreparedListener;
import com.pili.pldroid.player.widget.PLVideoView;


import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.Timer;
import java.util.TimerTask;

public class VideoPlayerActivity extends Activity implements PLOnPreparedListener, PLOnInfoListener, PLOnCompletionListener {

    private static final String TAG = VideoPlayerActivity.class.getSimpleName();

    public static final String KEY_DLNA_DATA = "key_play_url";
    public static final String KEY_VIDEO_TITLE = "KEY_VIDEO_TITLE";
    static PLVideoView mVideoView;

    private ImageView ivBg, ivPlay;
    private SeekBar progressBar;
    private TextView tvCurrentTime, tvTime;

    private ImageView ivMediaPic;

    private AudioManager mAudioManager;

    private  MediaModel dlnaMediaModel;
    private View LvVideo;
    public static VideoPlayerActivity playActivity = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED, WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED);

        dlnaMediaModel = getIntent().getParcelableExtra(KEY_DLNA_DATA);
        LvVideo = findViewById(R.id.cl_player);
        mVideoView = (PLVideoView)findViewById(R.id.PLVideoView);
        mVideoView.setDisplayAspectRatio(PLVideoView.ASPECT_RATIO_FIT_PARENT);//ASPECT_RATIO_PAVED_PARENT
        mVideoView.setBackgroundColor(Color.BLACK);
        mVideoView.setOnPreparedListener(this);
        mVideoView.setOnInfoListener(this);
        mVideoView.setOnCompletionListener(this);
        //加载转圈
        View loadingView = findViewById(R.id.loading);
        mVideoView.setBufferingIndicator(loadingView);

        //初始化组件
        ivBg = (ImageView)findViewById(R.id.iv_bg);
        ivPlay = (ImageView) findViewById(R.id.iv_play);
        progressBar = (SeekBar)findViewById(R.id.progressBar);
        tvCurrentTime = (TextView)findViewById(R.id.tv_current_time);
        ivMediaPic = (ImageView)findViewById(R.id.iv_picture);
        tvTime = (TextView)findViewById(R.id.tv_time);
        mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        progressBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                if(seekBar.getProgress() < mVideoView.getDuration()){
                    mVideoView.seekTo(seekBar.getProgress());
                }
            }
        });
        Log.e("url","onCreate url ="+dlnaMediaModel.getUrl());
        if(MediaUtils.isVideoItem(dlnaMediaModel)){
            ivMediaPic.setVisibility(View.GONE);
            LvVideo.setVisibility(View.VISIBLE);
            mVideoView.setVideoURI(Uri.parse(dlnaMediaModel.getUrl()));
        }else if(MediaUtils.isImageItem(dlnaMediaModel)){
            ivMediaPic.setVisibility(View.VISIBLE);
            LvVideo.setVisibility(View.GONE);
//            dlnaMediaModel.setUrl("https://imgsa.baidu.com/forum/w%3D580/sign=173d2ed0f6dcd100cd9cf829428947be/35b0d8dda3cc7cd9a1eab0043701213fba0e91cc.jpg");
            Glide.with(this).load(dlnaMediaModel.getUrl())
                    .fitCenter()
                    .into(ivMediaPic);
        }

        playActivity = this;
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
        dlnaMediaModel = getIntent().getParcelableExtra(KEY_DLNA_DATA);
        Log.e("url","onNewIntent url ="+dlnaMediaModel.getUrl());
        if(dlnaMediaModel.getUrl().indexOf("ff=265ts") != -1)
        {
            Toast.makeText(VideoPlayerActivity.this, "不支持此视频源格式" , Toast.LENGTH_SHORT).show();
            CastServer.flushDlna();
        }
        else {
            if (MediaUtils.isVideoItem(dlnaMediaModel)) {
                ivMediaPic.setVisibility(View.GONE);
                LvVideo.setVisibility(View.VISIBLE);
                mVideoView.setVideoURI(Uri.parse(dlnaMediaModel.getUrl()));
            } else if (MediaUtils.isImageItem(dlnaMediaModel)) {
                ivMediaPic.setVisibility(View.VISIBLE);
                LvVideo.setVisibility(View.GONE);
                ivMediaPic.setImageURI(Uri.parse(dlnaMediaModel.getUrl()));
            }
        }
    }

    @Subscribe(threadMode = ThreadMode.MAIN, priority = 100, sticky = false)
    public void onEvent(Event event) {
        switch (event.getOperate()) {
            case Event.CALLBACK_EVENT_ON_SEEK:
                //拉进度条
                Integer seek = (Integer) event.getData();
                if(seek < mVideoView.getDuration()) {
                    mVideoView.seekTo(seek);
                    String currentTime = MediaUtils.formatTimeFromMSInt(seek);
                    tvCurrentTime.setText(currentTime);
                    showMediaController();
                }else {
//                    new Handler().postDelayed(new Runnable() {
//                        @Override
//                        public void run() {
//                            mVideoView.seekTo(seek);
//                            String currentTime = MediaUtils.formatTimeFromMSInt(seek);
//                            tvCurrentTime.setText(currentTime);
//                            showMediaController();
//                        }
//                    }, 3000);
                }
                break;
            case Event.CALLBACK_EVENT_ON_STOP:
                //停止播放
                mVideoView.stopPlayback();
                this.finish();
            case Event.CALLBACK_EVENT_ON_PAUSE:
                //暂停
                mVideoView.pause();
                ivPlay.setImageResource(R.drawable.ic_pause);
                setMediaControllerVisibility(true);
                break;
            case Event.CALLBACK_EVENT_ON_PLAY:
                //继续播放
                mVideoView.start();
                ivPlay.setImageResource(R.drawable.ic_play);
                setMediaControllerVisibility(false);
                break;
            case Event.CALLBACK_EVENT_ON_SET_VOLUME:
                //设置音量
                String data = String.valueOf(event.getData());
                int volume = Integer.valueOf(data);//0-100
                int v = volume * 15 / 100;
                mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, v, AudioManager.FLAG_SHOW_UI);
                //盒子0-15,0-100:android10格,ios20格
                break;
            case Event.CALLBACK_EVENT_ON_SET_MUTE:
                //设置静音

                break;
        }
    }

    public static long getCurrentPosition(){
        if(mVideoView == null)
            return 0;
        Log.e("xia","mVideoView.getCurrentPosition()::" + mVideoView.getCurrentPosition());
        return mVideoView.getCurrentPosition();
    }

    public static long getDuration(){
        if(mVideoView == null)
            return 0;
        Log.e("xia","mVideoView.getDuration()::" + mVideoView.getDuration());
        return mVideoView.getDuration();
    }
    @Override
    protected void onPause() {
        super.onPause();
        if (EventBus.getDefault().isRegistered(this)) {
            EventBus.getDefault().unregister(this);
        }
        if (MediaUtils.isVideoItem(dlnaMediaModel) && mVideoView != null) {
            mVideoView.pause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!EventBus.getDefault().isRegistered(this)) {
            EventBus.getDefault().register(this);
        }
        if (MediaUtils.isVideoItem(dlnaMediaModel) && mVideoView != null) {
            mVideoView.start();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (MediaUtils.isVideoItem(dlnaMediaModel) && mVideoView != null) {
            mVideoView.stopPlayback();
            CastServer.flushDlna();
        }
    }

    @Override
    public void onPrepared(int i) {
        showMediaController();
        int durationTime = (int) mVideoView.getDuration();
        if (durationTime > 0) {
            tvTime.setText(MediaUtils.formatTimeFromMSInt(durationTime));
            progressBar.setMax(durationTime);
        }
    }

    @Override
    public void onInfo(int i, int i1) {
        int currentTime = (int) mVideoView.getCurrentPosition();
        if (currentTime > 0) {
            tvCurrentTime.setText(MediaUtils.formatTimeFromMSInt(currentTime));
            progressBar.setProgress(currentTime);
        }
    }

    @Override
    public void onCompletion() {
        Log.e(TAG, "播放完成");
        this.finish();
    }

    private void showMediaController() {
        if(ivBg.getVisibility() != View.VISIBLE)
            setMediaControllerVisibility(true);
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        setMediaControllerVisibility(false);
                    }
                });
            }
        }, 5000);
    }

    private void setMediaControllerVisibility(boolean visit) {
        if (visit) {
            ivBg.setVisibility(View.VISIBLE);
            ivBg.setAnimation(AnimationUtil.moveToViewLocation());
            ivPlay.setVisibility(View.VISIBLE);
            ivPlay.setAnimation(AnimationUtil.moveToViewLocation());
            tvCurrentTime.setVisibility(View.VISIBLE);
            tvCurrentTime.setAnimation(AnimationUtil.moveToViewLocation());
            progressBar.setVisibility(View.VISIBLE);
            progressBar.setAnimation(AnimationUtil.moveToViewLocation());
            tvTime.setVisibility(View.VISIBLE);
            tvTime.setAnimation(AnimationUtil.moveToViewLocation());
        } else {
            ivBg.setVisibility(View.GONE);
            ivBg.setAnimation(AnimationUtil.moveToViewBottom());
            ivPlay.setVisibility(View.GONE);
            ivPlay.setAnimation(AnimationUtil.moveToViewBottom());
            tvCurrentTime.setVisibility(View.GONE);
            tvCurrentTime.setAnimation(AnimationUtil.moveToViewBottom());
            progressBar.setVisibility(View.GONE);
            progressBar.setAnimation(AnimationUtil.moveToViewBottom());
            tvTime.setVisibility(View.GONE);
            tvTime.setAnimation(AnimationUtil.moveToViewBottom());
        }
    }


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        showMediaController();
        return super.onTouchEvent(event);
    }
}