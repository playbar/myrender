package com.baofeng.mojing;


import com.baofeng.mojing.sensor.MojingSDKSensorManager;
import com.baofeng.mojing.MojingSDKReport;
import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import java.lang.reflect.Method;

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
		MojingSDK.mContext = getApplicationContext();
		try
		{
			Class<?> c = Class.forName("com.baofeng.mojing.check.MojingCheckService");     			    
			Method method = c.getMethod("checkService", Activity.class);
			Log.d("MojingVrActivity", "MojingCheckService checkService");		
			method.invoke(c.newInstance(), this);
		}
   		catch(Exception e)
   		{
   			Log.d("MojingVrActivity", "MojingCheckService is not exist");
   			e.printStackTrace();
   		}  // end try 
		//MojingCheckService.checkService(this);
					    
		com.baofeng.mojing.MojingSDK.Init(this);

		//com.baofeng.mojing.MojingSDK.SetEngineVersion("Android");
    }

    @Override protected void onPause() {
	    super.onPause();

		MojingSDKSensorManager.UnRegisterSensor(this);
		if(!MojingSDK.IsUseUnityForSVR())
        {
			if(MojingSDK.IsInMachine()) {
				MojingSDK.StopTracker();
			}
			else 
			{
				if(MojingSDK.IsVrServiceDisabled())
				{
					MojingSDK.StopTracker();
				}
				else
				{
					MojingSDKServiceManager.onPause(this);
				}
			}
		}
		MojingSDKReport.onPause(this);
        mMojingSurfaceView.onPause();
        com.baofeng.mojing.MojingVrLib.stopVsync(this);
    }

    @Override protected void onResume() {
        super.onResume();

		MojingSDKSensorManager.RegisterSensor(this);
        if(!MojingSDK.IsUseUnityForSVR())
        {
            if(MojingSDK.IsInMachine())
            {
                MojingSDK.StartTracker(200);
            }
            else 
			{
				if(MojingSDK.IsVrServiceDisabled())
				{
					MojingSDK.StartTracker(100);
				}
				else
				{
					MojingSDKServiceManager.onResume(this);
				}
            }
      	}
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
