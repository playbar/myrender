package com.baofeng.mojing;

import java.util.Timer;
import java.util.TimerTask;

import org.json.JSONException;
import org.json.JSONObject;

import com.baofeng.mojing.service.IServiceResponse;
import com.baofeng.mojing.service.MojingSDKAIDLService;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.InputDevice;

public class MojingSDKServiceManager {

	final static String TAG = "MojingSDKServiceManager";
	
	final static int SENSOR_VID = 11561;
	final static int SENSOR_PID = 4097;
	
	private static String packageName;
	private static String glassName = "UNKNOWN";
	private static boolean isServiceTracker = false;
	private static MojingSDKAIDLService mService = null;
	private static boolean noTrackerMode = false;
	private static String sn = "";
	private static boolean isHMDWorking;
	private static Timer g_SocketHBTimer;
	private static Object g_SocketHBTimerSync = new Object();
	
	private static ServiceConnection connection = new ServiceConnection() {
		
        @Override
        public void onServiceDisconnected(ComponentName name) {
        	Log.d(TAG, "Service '" + name + "' Disconnected");
        	mService = null;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
        	Log.d(TAG, "Service '" + name + "' Connected");
            mService = MojingSDKAIDLService.Stub.asInterface(service);
        	try {
        		JSONObject jsonObj = new JSONObject();
				jsonObj.put("PackageName", packageName);
				jsonObj.put("EnableSensorFusion", true);
				jsonObj.put("SensorDataFromJava", com.baofeng.mojing.MojingSDK.IsSensorFromJava());
				//jsonObj.put("SensorDataFromJava", false);
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
			isServiceTracker = true;
			try {
				JSONObject jsonObj = new JSONObject(params);
				glassName = jsonObj.getString("GlassName");
				if (!noTrackerMode) 
				{
					MojingSDKAIDLService service = getService();
					if (service != null) {
						service.addSocketTarget(com.baofeng.mojing.MojingSDK.GetSocketPort());
						
						// start socket heartbeat
						if (g_SocketHBTimer == null)
							g_SocketHBTimer = new Timer();
						g_SocketHBTimer.schedule(new TimerTask()
						{
							@Override
							public void run()
							{
								synchronized (g_SocketHBTimerSync)
								{
									Log.w(TAG, "socketHeartbeat");
									if (g_SocketHBTimer != null)
									{
										try {
											mService.socketHeartbeat(com.baofeng.mojing.MojingSDK.GetSocketPort());
											Log.w(TAG, "socketHeartbeat ok");
										} catch (RemoteException e) {
											e.printStackTrace();
										}
									}// end of if
								}// end of synchronized (g_SocketHBTimerSync)
							}// end of run
						}, 0, 30000);
					}
					
					com.baofeng.mojing.MojingSDK.StartGlassTracker(glassName);
				}
			} catch (Exception e) {
				e.printStackTrace();
				if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StartTracker(100);
			}
        }
        
        @Override
        public void sensorFailed(final String params) {
			isServiceTracker = false;
        	if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StartTracker(100);
        }

		@Override
		public void sensorStopped() throws RemoteException {
			isServiceTracker = false;
		}

		@Override
		public void sensorStarted(String params) throws RemoteException {
			isServiceTracker = true;
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
					if (!jsonObj.getBoolean("State")) {
						sn = "";
						isHMDWorking = false;
						com.baofeng.mojing.MojingSDK.SetHDMWorking(isHMDWorking);
					}
					if (hmdTrackerListener != null) hmdTrackerListener.onHMDTrackerStateChanged(jsonObj.getBoolean("State"));
				}
				else if (eventName.equals("HMDWorking")) {
					isHMDWorking = jsonObj.getBoolean("State"); 
					com.baofeng.mojing.MojingSDK.SetHDMWorking(isHMDWorking);
					sn = mService.getSN();
					if (sn == null) {
						sn = "";
					}
					else
					{
						//com.baofeng.mojing.MojingSDK.LogTrace("Get glasses sn: " + sn);
						com.baofeng.mojing.MojingSDK.SetGlassesSN(sn);
					}
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

		@Override
		public void onControllerData(int id, byte[] data) throws RemoteException {
			if (controllerListener != null) controllerListener.onData(id, data);
		}

		@Override
		public void onControllerConnect(int id) throws RemoteException {
			if (controllerListener != null) controllerListener.onDeviceConnect(id);
		}

		@Override
		public void onControllerDisconnect(int id) throws RemoteException {
			if (controllerListener != null) controllerListener.onDeviceDisconnect(id);
		}
    };
    
    public interface ControllerListener {
    	void onData(int id, byte[] data);
    	void onDeviceConnect(int id);
    	void onDeviceDisconnect(int id);
    }
    
    private static ControllerListener controllerListener = null;
    public static void setControllerListener(ControllerListener listener) {
    	controllerListener = listener;
	}
    
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
				// stop socket heartbeat
				if (g_SocketHBTimer != null)
				{
					g_SocketHBTimer.cancel();		
					g_SocketHBTimer = null;
				}
	
				JSONObject jsonObj = new JSONObject();
				jsonObj.put("PackageName", activity.getPackageName());
				jsonObj.put("EnableSensorFusion", true);
				mService.clientPause(jsonObj.toString());
							
				mService.removeSocketTarget(com.baofeng.mojing.MojingSDK.GetSocketPort());

				isHMDWorking = false;
				com.baofeng.mojing.MojingSDK.SetHDMWorking(isHMDWorking);
				sn = "";
	        } catch (RemoteException e) {
	            e.printStackTrace();
	        } catch (JSONException e) {
	        	e.printStackTrace();
	        }
		}

		try {
			activity.unbindService(connection);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
			
		if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StopTracker();
	}
	
	public static MojingSDKAIDLService getService() {
		return mService;
	}
	
	public static boolean isServiceTracker() {
		return isServiceTracker;
	}
	
	public static String getSN() {
		return sn;
	}
	
	public static boolean isHMDWorking() {
		return isHMDWorking;
	}
	
	public static boolean StartTracker() {
		if(noTrackerMode) return false;

		if(isServiceTracker)
		{
			return com.baofeng.mojing.MojingSDK.StartGlassTracker(glassName);
		}
		else
		{
			return com.baofeng.mojing.MojingSDK.StartTracker(100);  	
		}
	}

	public static void ClientReStart() {
		if(mService == null || isHMDWorking) return;
		if (g_SocketHBTimer != null)
		{
			g_SocketHBTimer.cancel();		
			g_SocketHBTimer = null;
		}	

        try {
			JSONObject jsonObj = new JSONObject();	
			jsonObj.put("PackageName", packageName);
			jsonObj.put("EnableSensorFusion", true);
			jsonObj.put("SensorDataFromJava", com.baofeng.mojing.MojingSDK.IsSensorFromJava());
		
			mService.clientPause(jsonObj.toString());
			mService.removeSocketTarget(com.baofeng.mojing.MojingSDK.GetSocketPort());
			if (!noTrackerMode) com.baofeng.mojing.MojingSDK.StopTracker();

			mService.clientResume(jsonObj.toString(), callback);
	   } catch (RemoteException e) {
	        e.printStackTrace();
	   } catch (JSONException e) {
			e.printStackTrace();
	   }
	}
	/*
	public static String getSkinList()
	{
		if(mService != null)
		{
			try
			{
				return mService.getSkinList();
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return "";
	}
		
	public static int loadOneSkin(String skinName)
	{
		if(mService != null)
		{
			try
			{
				return mService.loadOneSkin(skinName);
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return 0;
	}
	*/
}
