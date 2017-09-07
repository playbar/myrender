package com.bn.mojing;

import android.app.Application;
import android.content.Context;

import com.squareup.leakcanary.LeakCanary;
import com.squareup.leakcanary.RefWatcher;

/**
 * Created by houguoli on 2017/5/5.
 */

public class MJApplication extends Application {

    public static RefWatcher getRefWatcher(Context context) {
        MJApplication application = (MJApplication) context.getApplicationContext();
        return application.refWatcher;
    }

    private RefWatcher refWatcher;

    @Override
    public void onCreate() {
        super.onCreate();
        refWatcher = LeakCanary.install(this);
    }
}
