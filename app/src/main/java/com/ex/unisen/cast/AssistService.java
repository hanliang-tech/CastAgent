package com.ex.unisen.cast;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.os.IBinder;

public class AssistService extends Service {
    public AssistService() {
    }

    public class LocalBinder extends Binder
    {
        public AssistService getService()
        {
            return AssistService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return new LocalBinder();
    }
}
