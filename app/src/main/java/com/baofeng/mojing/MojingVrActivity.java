package com.baofeng.mojing;

import com.baofeng.mojing.sensor.MojingSDKSensorManager;
import com.baofeng.mojing.MojingSDKReport;
import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;

public class MojingVrActivity extends Activity {
    MojingSurfaceView mMojingSurfaceView;
    
    public MojingSurfaceView getSurfaceView()
    {
		return mMojingSurfaceView;
    }

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mMojingSurfaceView = new MojingSurfaceView(this);
		mMojingSurfaceView.requestFocus();
		mMojingSurfaceView.setFocusableInTouchMode(true);
        setContentView(mMojingSurfaceView);
                
		com.baofeng.mojing.MojingSDK.Init(this);

		com.baofeng.mojing.MojingSDK.SetEngineVersion("Android");
    }

    @Override protected void onPause() {
        mMojingSurfaceView.onPause();
        super.onPause();
		MojingSDKSensorManager.UnRegisterSensor(this);
        MojingSDKServiceManager.onPause(this);
        com.baofeng.mojing.MojingVrLib.stopVsync(this);
		MojingSDKReport.onPause(this);
    }

    @Override protected void onResume() {
        super.onResume();   

		MojingSDKSensorManager.RegisterSensor(this);
        MojingSDKServiceManager.onResume(this);
		com.baofeng.mojing.MojingVrLib.startVsync(this);
        mMojingSurfaceView.onResume();
		MojingSDKReport.onResume(this);
		//MojingSDKReport.onEvent("buy magic bean", "mojing shop", "money", 100, "magic bean", 1000);
    }
    
	@Override
	public void onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);
	}
}
