package com.ex.unisen.cast;


import com.ex.unisen.cast.CastServer;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

public class StartupReceiver extends BroadcastReceiver {
	protected static final String TAG = StartupReceiver.class.getSimpleName();
	
	public StartupReceiver() {
		
		Log.e(TAG,"StartupReceiver construct");
	}

	@Override
	public void onReceive(Context context, Intent intent) {
	       Log.e(TAG,"StartupReceiver  onReceive");

	        if(intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED))
	        {
				Intent mIntent = new Intent();
				mIntent.setAction(CastServer.START_CAST_ENGINE);
				mIntent.setPackage("com.ex.unisen.cast");
				context.startService(mIntent);
				Log.i(TAG, "yzs  get start broadcast & StartService");

	        }
	}
	
	

	
	
}
