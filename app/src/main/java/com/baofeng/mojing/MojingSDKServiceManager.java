package com.baofeng.mojing;

import org.json.JSONException;
import org.json.JSONObject;

import com.baofeng.mojing.service.IServiceResponse;
import com.baofeng.mojing.service.MojingSDKAIDLService;
import com.baofeng.mojing.service.IServiceResponse.Stub;

import android.R.string;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class MojingSDKServiceManager {

	final static String TAG = "MojingSDKServiceManager";
	
	final static int SENSOR_VID = 11561;
	final static int SENSOR_PID = 4097;
	
	private static String packageName;
	private static String glassName = "UNKNOWN";
	private static boolean isGlassTracker = false;
	private static MojingSDKAIDLService mService = null;
	private static boolean noTrackerMode = false;
	
	private static ServiceConnection connection = new ServiceConnection() {
		
        @Override
        public void onServiceDisconnected(ComponentName name) {
        	mService = null;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mService = MojingSDKAIDLService.Stub.asInterface(service);
        	try {
        		JSONObject jsonObj = new JSONObject();
				jsonObj.put("PackageName", packageName);
				mService.clientResume(jsonObj.toString(), callback);
	        } catch (RemoteException e) {
	            e.printStackTrace();
	        } catch (JSONException e) {
	        	e.printStackTrace();
	        }
        }
    };
    
    private final static IServiceResponse.Stub callback = new IServiceResponse.Stub() {
    	@Override
        public void sensorSuccess(final String params) {
			try {
				JSONObject jsonObj = new JSONObject(params);
				glassName = jsonObj.getString("GlassName");
				if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StartGlassTracker(glassName);
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StartTracker(100);
			}
        }
        
        @Override
        public void sensorFailed(final String params) {
        	if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StartTracker(100);
        }

		@Override
		public void sensorStopped() throws RemoteException {
		}

		@Override
		public void sensorStarted(String params) throws RemoteException {
			if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StopTracker();
			
			try {
				JSONObject jsonObj = new JSONObject(params);
				glassName = jsonObj.getString("GlassName");
				if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StartGlassTracker(glassName);
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		@Override
		public void event(String data) throws RemoteException {
			try {
				JSONObject jsonObj = new JSONObject(data);
				String eventName = jsonObj.getString("Name");
				if (eventName.equals("HMDUpgrade")) {
					if (hmdUpgradeResultListener != null) hmdUpgradeResultListener.onHMDUpgradeResult(jsonObj.getBoolean("Result"));
				}
				else if (eventName.equals("HMDUpload")) {
					if (hmdUploadCompleteListener != null) hmdUploadCompleteListener.onHMDUploadComplete();
				}
				else if (eventName.equals("HMDTracker")) {
					isGlassTracker = jsonObj.getBoolean("State");
					if (hmdTrackerListener != null) hmdTrackerListener.onHMDTrackerStateChanged(jsonObj.getBoolean("State"));
				}
				else if (eventName.equals("HMDWorking")) {
					if (hmdWorkingListener != null) hmdWorkingListener.onHMDWorkingStateChanged(jsonObj.getBoolean("State"));
				}
				else if (eventName.equals("HMDLowPower")) {
					if (hmdLowPowerListener != null) hmdLowPowerListener.onHMDLowPowerStateChanged(jsonObj.getBoolean("State"));
				}
				else if (eventName.equals("HMDJoystick")) {
					if (hmdJoystickListener != null) hmdJoystickListener.onHMDJoystickStateChanged(jsonObj.getBoolean("State"));
				}
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}
    };
    
    public interface HMDLowPowerListener {
        void onHMDLowPowerStateChanged(boolean isLowPower);
    }
    
    private static HMDLowPowerListener hmdLowPowerListener = null;
    public static void setHMDLowPowerListener(HMDLowPowerListener listener) {
    	hmdLowPowerListener = listener;
	}

    public interface HMDTrackerListener {
        void onHMDTrackerStateChanged(boolean isHMDTracker);
    }
    
    private static HMDTrackerListener hmdTrackerListener = null;
    public static void setHMDTrackerListener(HMDTrackerListener listener) {
    	hmdTrackerListener = listener;
	}
    
    public interface HMDWorkingListener {
        void onHMDWorkingStateChanged(boolean isHMDWorking);
    }
    
    private static HMDWorkingListener hmdWorkingListener = null;
    public static void setHMDWorkingListener(HMDWorkingListener listener) {
    	hmdWorkingListener = listener;
	}
    
    public interface HMDJoystickListener {
        void onHMDJoystickStateChanged(boolean isConnected);
    }
    
    private static HMDJoystickListener hmdJoystickListener = null;
    public static void setHMDJoystickListener(HMDJoystickListener listener) {
    	hmdJoystickListener = listener;
	}
    
    public interface HMDUpgradeResultListener {
        void onHMDUpgradeResult(boolean isSuccess);
    }
    
    private static HMDUpgradeResultListener hmdUpgradeResultListener = null;
    public static void setHMDUpgradeResultListener(HMDUpgradeResultListener listener) {
    	hmdUpgradeResultListener = listener;
	}
    
    public interface HMDUploadCompleteListener {
        void onHMDUploadComplete();
    }
    
    private static HMDUploadCompleteListener hmdUploadCompleteListener = null;
    public static void setHMDUploadCompleteListener(HMDUploadCompleteListener listener) {
    	hmdUploadCompleteListener = listener;
	}
    
    
//	public static void init(Activity activity)
//	{
//		Intent intent = new Intent();
//		intent.setAction("com.baofeng.mojing.service.MojingSDKAIDLService");
//		intent.setPackage("com.baofeng.mj");
//		activity.startService(intent);
//	}
    
    public static void onResumeNoTrackerMode(Activity activity) {
    	noTrackerMode = true;
    	bindService(activity);
    }
    
	public static void onResume(Activity activity) {
		noTrackerMode = false;
		bindService(activity);
	}
	
	private static void bindService(Activity activity) {
		packageName = activity.getPackageName();
		
		Intent intent = new Intent();
		intent.setAction("com.baofeng.mojing.service.MojingSDKAIDLService");
		intent.setPackage("com.baofeng.mj");
		activity.startService(intent);
		if (!activity.bindService(intent, connection, Context.BIND_AUTO_CREATE)) 
		{
			Log.w(TAG, "ServiceApp 'com.baofeng.mj' not found!");
			if (!noTrackerMode)
			{
				com.baofeng.mojing.MojingSDK.StartTracker(100);
			}
		}
	}
	
	public static void onPause(Activity activity) {
		if (mService != null) {
			try {
				JSONObject jsonObj = new JSONObject();
				jsonObj.put("PackageName", activity.getPackageName());
				mService.clientPause(jsonObj.toString());
	        } catch (RemoteException e) {
	            e.printStackTrace();
	        } catch (JSONException e) {
	        	e.printStackTrace();
	        }
		}
		activity.unbindService(connection);
		
		if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StopTracker();
	}
	
	public static MojingSDKAIDLService getService() {
		return mService;
	}
	
	public static boolean isGlassTracker() {
		return isGlassTracker;
	}
	
	public static boolean StartTracker () {
		if(noTrackerMode) return false;

		if(isGlassTracker)
		{
			return com.baofeng.mojing.MojingSDK.StartGlassTracker(glassName);
		}
		else
		{
			return com.baofeng.mojing.MojingSDK.StartTracker(100);  	
		}
	}	
}
