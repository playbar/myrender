package com.bn.mojing;

import java.lang.ref.WeakReference;

import com.baofeng.mojing.MojingSDKServiceManager;
import com.baofeng.mojing.MojingSDKServiceManager.HMDJoystickListener;
import com.baofeng.mojing.MojingSDKServiceManager.HMDLowPowerListener;
import com.baofeng.mojing.MojingSDKServiceManager.HMDTrackerListener;
import com.baofeng.mojing.MojingSDKServiceManager.HMDUpgradeResultListener;
import com.baofeng.mojing.MojingSurfaceView;
import com.baofeng.mojing.MojingVrActivity;

import android.content.Intent;
import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.util.SparseArray;
import android.view.MotionEvent;

public class MojingActivity extends MojingVrActivity{
	static final int SEND_SMS_REQUEST = 0; 
	static final int CALL_REQUEST = 1; 
	
   static int StartCount = 0;
	SparseArray<String> stateMap = new SparseArray<String>();
	
    MojingSurfaceView mView;

    static String MJ2 = "F79F2H-Q8ZNXN-2HQN2F-2ZA9YV-QG4H9H-QGAYAE";
	static String MJ3 = "CY42HN-FKCRQ8-SGS34S-8K23ZR-SZSBQ8-QFXF95";
	static String MJ3_88pmma = "8349QZ-YBAQA8-QNWW86-W8ADQH-WVDFSG-FV8NHN";
	static String MJ3_96pmma = "SFCNDH-4WCYA4-4W8NX8-ZN9NF8-C386HT-A49ZWB";
	static String MJ4 = "2WF5F5-FPWGZZ-H7AE2C-H3F8SW-EE8KCF-YTHBCN";
	static String MJA1="WNE8QN-QVCCZN-XXF8W3-9Q2YDH-AQZRA2-XVDZ9Q";
	static String RK3288="WNE8QN-QVCCZN-XXF8W3-9Q2YDH-AQZRA2-XVDZ9Q";
	static String MJ_D="Z74QHR-AC9RYL-4FZGQ3-QGZZ2H-9T86Q5-XGFVW8";
	static String UC_VR = "WZ2Z8K-9VFT2Y-ZZQHZB-Z82N48-2CZL4B-CGAZDW";
	static String lefant = "Y7XVWU-4BDSFN-S99HA8-8598WR-YVH78U-Y7XNSW";
	static String MJ5 = "9GEDFU-954ESX-QV9B2Y-YVXNEG-FLDWAY-ZFS99Z";
	static String MJ_RIO = "SXF8DB-ZUSQS8-A2FMDC-SHHLHB-SZZNY3-DHC8Y7";
	static String MJ_VR43 = "QGQZCZ-2FQ383-C8QYS8-H5AHDG-DBSW98-EQEWQB";
	static String MJ_QB820 = "2Q2XWR-9YWQSQ-WUHFH7-WWE2WW-8RW84W-XT2FEX";

	static String[] MojingKey = {MJ2,MJ3,MJ3_96pmma,MJ3_88pmma,MJ4,MJ_RIO,MJ5};
	

	static String VrBox = "WYAQWH-CF95YZ-WWHL4Y-ZBE9YT-QZWGWT-9VDGWH";
	static String QIKU360="WUZFSH-XX8393-8RDY9B-XCS9WW-CBH8ZK-Q599XC";//奇酷360
	static String MOKE="X9DDQ5-XG9RDC-ADX3XF-DFHLHT-YTXWSQ-ZUHVWY";// 莫克
	static String QHMJ="9FWZDH-4SSXEH-8UZTFR-QYEH4Z-2XSFWQ-H8F59G";// 千幻魔镜
	
	static String Trd_3DXNTK = "XW9YQZ-HL2GSF-2HC9Z7-ZH498L-F7WVY5-WH4XXY";// 3D虚拟头盔
	static String Trd_HLVR = "Z88RAY-9Y42HH-9G4XQY-QTQQWQ-22XB2H-QFFNFM";// 幻侣VR眼镜

	//小宅z4	标准镜片	
	static String Trd_XZZ4 = "22F7DH-YB2W2C-YBDWQR-2X9ZQ7-Z78LZR-DCHG8T";
	//Pico1	COP	
	static String Trd_COP = "954YX8-DS2FA8-QRQYYG-Z5W828-FGAFHB-WNYBHH";
	//机饕	标准镜片	
	static String Trd_JT = "98EEY5-DZ99QB-FNHNS9-AYCGEW-EC4BDT-8BZVQG";
	//大朋看看	标准镜片	
	static String Trd_DPKK="Z7AAE9-EX48QZ-8NA2AF-YUC99N-F3HRS2-DZEQH5";
	//极睿DreamVR	标准镜片	
	static String Trd_JRDR = "Z3DWQ5-E2HF2B-93ADEC-WQ8PZ3-XNXGSQ-A2CHQ3";
	
	static boolean   bTimeWarp = true;
	static boolean   bMultiThread = true;
	static String DefaultGlass = MJ4 ;// MJ2;//MJ_QB820;RK3288;
	private VrPhotoRender renderer;
	
	private Handler handler = new Handler( );

    private Runnable runnable = new Runnable( ) {
    	public void run ( ) {
//	    	Log.e("TEST", "IsLowPower:" + com.baofeng.mojing.MojingSDK.IsLowPower());
    		Log.e("TEST", "isGlassTracker:" + MojingSDKServiceManager.isServiceTracker());
		    handler.postDelayed(this,1000);
	    }
    };
	
	GLMsgHandler mGLMsgHandler;
	private static class GLMsgHandler extends Handler {
		private final WeakReference<MojingActivity> mActivity;

		public GLMsgHandler(MojingActivity activity) {
			mActivity = new WeakReference<MojingActivity>(activity);
		}

		@Override
		public void handleMessage(Message msg) {
			MojingActivity activity = mActivity.get();
			if (activity != null) {
				if (msg.what == MojingSurfaceView.ON_CHANGE_MOJING_WORLD) 
				{
					activity.onChangeMojingWorld();
				}
			}
		}
	}
	
	public void onChangeMojingWorld() 
	{
		float fov = com.baofeng.mojing.MojingSDK.GetMojingWorldFOV() / 2.f;            
        float ratio = (float)Math.tan(Math.toRadians(fov));
        MatrixState.setProjectFrustum(-ratio, ratio, -ratio, ratio, 1.f, 800);
	}
	
    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = super.getSurfaceView();
        
        mGLMsgHandler = new GLMsgHandler(this);
        mView.setMessageHandler(mGLMsgHandler);
        
        mView.setTimeWarp(bTimeWarp);
        mView.setMultiThread(bMultiThread);
        mView.setGlassesKey(DefaultGlass);

		renderer=new VrPhotoRender(this);
		mView.setRenderer(renderer);
		mView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        
		String  strManufacturerList = 
			com.baofeng.mojing.MojingSDK.GetManufacturerList("ZH");
		String  strProductList = 
			com.baofeng.mojing.MojingSDK.GetProductList("E92YDY-AHDQWV-ACF39Y-QT4EDC-CZ4CFU-4E8B85", "ZH");
		
		String  strGlassList = 
			com.baofeng.mojing.MojingSDK.GetGlassList("SG2W2B-HG95SX-29CR8M-EFZ8DQ-9HEZW3-2BYZZU", "ZH");
		
		String  strGlassInfo = 
			com.baofeng.mojing.MojingSDK.GetGlassInfo("SFCNDH-4WCYA4-4W8NX8-ZN9NF8-C386HT-A49ZWB", "ZH");
		
		String  strGenerationGlassKey = 
			com.baofeng.mojing.MojingSDK.GenerationGlassKey("E92YDY-AHDQWV-ACF39Y-QT4EDC-CZ4CFU-4E8B85",  "CY42HN-FKCRQ8-SGS34S-8K23ZR-SZSBQ8-QFXF95");
		
		com.baofeng.mojing.MojingSDK.LogTrace("strManufacturerList >>>>" + strManufacturerList);
		com.baofeng.mojing.MojingSDK.LogTrace("strProductList >>>>" + strProductList);
		com.baofeng.mojing.MojingSDK.LogTrace("strGlassList >>>>" + strGlassList);
		com.baofeng.mojing.MojingSDK.LogTrace("strGlassInfo >>>>" + strGlassInfo);

		strManufacturerList = 
			com.baofeng.mojing.MojingSDK.GetManufacturerList("ZH");
		strProductList = 
			com.baofeng.mojing.MojingSDK.GetProductList("E92YDY-AHDQWV-ACF39Y-QT4EDC-CZ4CFU-4E8B85", "ZH");
		
		strGlassList = 
			com.baofeng.mojing.MojingSDK.GetGlassList("SG2W2B-HG95SX-29CR8M-EFZ8DQ-9HEZW3-2BYZZU", "ZH");
		
		strGlassInfo = 
			com.baofeng.mojing.MojingSDK.GetGlassInfo("SFCNDH-4WCYA4-4W8NX8-ZN9NF8-C386HT-A49ZWB", "ZH");
		
		strGenerationGlassKey = 
			com.baofeng.mojing.MojingSDK.GenerationGlassKey("E92YDY-AHDQWV-ACF39Y-QT4EDC-CZ4CFU-4E8B85",  "CY42HN-FKCRQ8-SGS34S-8K23ZR-SZSBQ8-QFXF95");
		
		com.baofeng.mojing.MojingSDK.LogTrace("strManufacturerList 2 >>>>" + strManufacturerList);
		com.baofeng.mojing.MojingSDK.LogTrace("strProductList 2 >>>>" + strProductList);
		com.baofeng.mojing.MojingSDK.LogTrace("strGlassList 2 >>>>" + strGlassList);
		com.baofeng.mojing.MojingSDK.LogTrace("strGlassInfo 2 >>>>" + strGlassInfo);

		MojingSDKServiceManager.setHMDTrackerListener(new HMDTrackerListener() {
			
			@Override
			public void onHMDTrackerStateChanged(boolean isHMDTracker) {
				Log.e("TEST", "onHMDTrackerStateChanged:" + isHMDTracker);
			}
		});
		
		MojingSDKServiceManager.setHMDLowPowerListener(new HMDLowPowerListener() {
			
			@Override
			public void onHMDLowPowerStateChanged(boolean isLowPower) {
				Log.e("TEST", "onHMDLowPowerStateChanged:" + isLowPower);
			}
		});
		
		MojingSDKServiceManager.setHMDJoystickListener(new HMDJoystickListener() {
			
			@Override
			public void onHMDJoystickStateChanged(boolean isConnected) {
				Log.e("TEST", "onHMDJoystickStateChanged:" + isConnected);
			}
		});
		
		MojingSDKServiceManager.setHMDUpgradeResultListener(new HMDUpgradeResultListener() {
			
			@Override
			public void onHMDUpgradeResult(boolean isSuccess) {
				Log.e("TEST", "onHMDUpgradeResult:" + isSuccess);
			}
		});
    }

    
	    
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		
		if (event.getAction() == MotionEvent.ACTION_UP)
		{
			//jump2AdjustActivity();
			//com.baofeng.mojing.MojingSDK.onSensorOriginChanged(this);
			String MojingType = MojingKey[StartCount];
			mView.setGlassesKey(MojingType);
			//com.baofeng.mojing.MojingSDK.ChangeMojingWorld(MojingType);
			StartCount  = StartCount + 1;
			StartCount %=  MojingKey.length;
		}	
		
		return super.onTouchEvent(event);
	}
    
    
	@Override protected void onDestroy() {	
		
		super.onDestroy();
		com.baofeng.mojing.MojingSDKPrivate.DistortionSaveParamet();
		com.baofeng.mojing.MojingSDK.LogTrace("MojingActivity onDestroy");	
	}

    @Override protected void onPause() {
        //com.baofeng.mojing.MojingVrLib.stopVsync(this);
        super.onPause();
        //mView.onPause();
		//com.baofeng.mojing.MojingSDK.StopTracker();
		
		//MojingSDKReport.onPause(this);
        
//        handler.removeCallbacks(runnable);
    }

    @Override protected void onResume() {
        super.onResume();   
       // mView.onResume();

        //ZGameBTUtil.setMzKeyCallback(this);
        
		//int ii = com.baofeng.mojing.MojingSDK.CheckSensors();
		//com.baofeng.mojing.MojingSDK.StartTracker(100);
		//ii = com.baofeng.mojing.MojingSDK.CheckSensors();
		//com.baofeng.mojing.MojingVrLib.startVsync(this);
		//MojingSDKReport.onResume(this);
		//MojingSDKReport.onEvent("buy magic bean", "mojing shop", "money", 100, "magic bean", 1000);
        
//        handler.postDelayed(runnable, 1000);
    }
    
    private void jump2AdjustActivity() {
		// mView.SetEndFream();
		// Intent intent = new Intent(this, AdjustActivity.class);
		
		// List parameter = new ArrayList();
		// for(int i = 0; i < param.length; i++)
		// {
		// 	parameter.add(param[i]);
		// }
		// Bundle bundle = new Bundle();
		// bundle.putSerializable("parameter", (Serializable) parameter);		
		// intent.putExtras(bundle);
		
		//startActivityForResult(intent, SEND_SMS_REQUEST);
		// startActivity(intent);	
		finish();
    }
    
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    	switch (resultCode) 
    	{
    		case RESULT_OK:
    	    break;
    	default:
    	    break;
    	    }
    	}
    
  

	@Override
	public void onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);
	}

}
