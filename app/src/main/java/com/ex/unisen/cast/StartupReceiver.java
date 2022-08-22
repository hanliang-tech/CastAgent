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
		// TODO: This method is called when the BroadcastReceiver is receiving
		// an Intent broadcast.
		//throw new UnsupportedOperationException("Not yet implemented");
	       Log.e(TAG,"StartupReceiver  onReceive");

	        if(intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED))
	        {
				Intent mIntent = new Intent();
				mIntent.setAction("com.unisen.ex.start.wifidetect");
				mIntent.setPackage("com.ex.unisen.cast");
				context.startService(mIntent);
				Log.i(TAG, "yzs  get start broadcast & StartService");

	        }
	}
	
	

	
	
}
