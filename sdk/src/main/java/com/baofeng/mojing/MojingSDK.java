package com.baofeng.mojing;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import android.R.bool;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Build;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.view.InputDevice;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;
import android.content.res.AssetManager;
import android.content.res.Resources;

import org.json.JSONException;
import org.json.JSONObject;
import com.baofeng.mojing.MojingSDKServiceManager;
import com.baofeng.mojing.sensor.MojingSDKSensorManager;

public class MojingSDK
{
    public static int 	SENSOR_ERROR_NOERROR		= 0;
	public static int 	SENSOR_ERROR_NoMag			=0x01;
	public static int 	SENSOR_ERROR_NoGryo			=0x04;
	public static int 	SENSOR_ERROR_GryoTooSlow	=0x08;
	public static int 	SENSOR_ERROR_NoAccel		=0x10;
	public static int 	SENSOR_ERROR_AccelTooSlow	=0x20;

	public static int	SENSOR_ORIGIN_EXTERNAL_SDK = 0;
	public static int	SENSOR_ORIGIN_DAYDREAM = 0;
	public static int	SENSOR_ORIGIN_LOCAL_NATIVE = 1;
	public static int	SENSOR_ORIGIN_LOCAL_JAVA = 2;
	public static int	SENSOR_ORIGIN_EXTERNAL_DEVICE = 3;

	public static int	EYE_TEXTURE_TYPE_LEFT =	1;
	public static int	EYE_TEXTURE_TYPE_RIGHT=	2;
	public static int	EYE_TEXTURE_TYPE_BOTH =	3;

	private static Timer g_DeviceTimer;
	
	static 
	{
		System.loadLibrary("curl");
		System.loadLibrary("sqlite3");
		System.loadLibrary("mojing");
    }

	public static native int GetSystemIntProperty(String property, int defaultValue);
	private static boolean m_bVrServiceDisabled = false;

	private static boolean m_inited = false;
	private static native boolean Init(String MerchantID, String AppID, String AppKey, String AppName, String packageName, String userID, String channelID, int nWidth, int nHeight, float xdpi, float ydpi, String ProfilePath);
	private  static native void CheckPackage(String jstrAppName, String jstrCaseCode);
	public  static native void SetEngineVersion(String jstrEngineVersion);
	public static boolean Init(Context context)
	{	
		if (!m_inited)
		{
			String path = exportFromAssetsFile(context);//context.getApplicationContext().getPackageResourcePath();
			String appName = getApplicationName(context);	
			String packageName = getAppPackageName(context);
			String userID = getUserID(context);
			String channelID = getCustomMetaData(context, "DEVELOPER_CHANNEL_ID");
			String appID = getCustomMetaData(context, "DEVELOPER_APP_ID");
			String appKey = getCustomMetaData(context, "DEVELOPER_APP_KEY");
			String merchantID = getCustomMetaData(context, "DEVELOPER_MERCHANT_ID");
			android.util.DisplayMetrics dm = context.getResources().getDisplayMetrics();	
			m_inited = true;
			Init(merchantID, appID, appKey, appName, packageName, userID, channelID, dm.widthPixels,dm.heightPixels,  dm.xdpi , dm.ydpi, path);
			String appNameMain = getAppMainName(context);
			String strCaseCode = getCustomMetaData(context, "DEVELOPER_CASE_CODE");
			CheckPackage(appNameMain, strCaseCode);
			//GetJoystickFileName();  //Done in Parameters::Init()
		}

		return true;
	}
   
	public static EyeTextureParameter GetEyeTextureParameter(int eyeTextureType)
	{ 
	   EyeTextureParameter Ret =  new EyeTextureParameter();
		int []Parameter = {0,0,0};
		Ret.m_EyeTexID = GetEyeTexture(eyeTextureType , Parameter);
		Ret.m_EyeTexType=eyeTextureType ;
		Ret.m_Width = Parameter[0];
		Ret.m_Height= Parameter[1];
		Ret.m_Format= Parameter[2];
		return Ret;
   }

   private static String exportFromAssetsFile(Context context)
   {
   	String result=null;    	
	File CacheDir = context.getCacheDir();
   	String cachePath = CacheDir.getPath();
   	result = cachePath;
	File cachePathFile = new File(cachePath);
	if (cachePathFile == null)
	{
		//	LogTrace("Can not open cache floder at : " + cachePath);
	}
	else
	{
		if (!cachePathFile.exists() || !cachePathFile.isDirectory())
		{
			cachePathFile.mkdir();
		}
	}
		// LogTrace("Cache : " + cachePath + " , Exists = " + cachePathFile.exists() + " , Dir = " + cachePathFile.isDirectory());

   	try
   	{
   		Resources r = context.getResources();
   		String[] ProfileList =r.getAssets().list("MojingSDK"); 
   		LogTrace("Find " + ProfileList.length + " file(s) in assets/MojingSDK");
   		if (ProfileList.length > 0)
   		{
   			AssetManager assetManager = r.getAssets();
   			for(String filename : ProfileList) {
   				InputStream in = null;
   				OutputStream out = null;
   				try {
   					in = assetManager.open("MojingSDK/" + filename);
   					File outFile = new File(CacheDir, filename);
   					out = new FileOutputStream(outFile);
   					copyFile(in, out);
   					in.close();
   					in = null;
   					out.flush();
   					out.close();
   					out = null;
   					LogTrace("copy " + filename + " to CacheDir");
   				} catch(IOException e) {
   					LogError("Failed to copy asset file: " + filename + e.toString());
   				}// end catch       
   			}// end for
   		}//length>0
   		
   	}// end try
   	catch(Exception e)
   	{
   		e.printStackTrace();
   	}   
	return result;
   }
   
   private static void copyFile(InputStream in, OutputStream out) throws IOException 
   {
		byte[] buffer = new byte[1024];
		int read;
		while((read = in.read(buffer)) != -1)
		{
			out.write(buffer, 0, read);
		}
	}

	/**
	 * get CPU serial No.
	 * @return CPU serial No.(16 bytes)
	 *  if failed, return "0000000000000000"
	 */
	private static String getCPUSerial() 
	{
		String str = "", strCPU = "", cpuAddress = "0000000000000000";
		try {
			//read cpu info
			Process pp = Runtime.getRuntime().exec("cat /proc/cpuinfo");
			InputStreamReader ir = new InputStreamReader(pp.getInputStream());
			LineNumberReader input = new LineNumberReader(ir);

			for (int i = 1; i < 100; i++) {
				str = input.readLine();
				if (str != null) {
					// read "serial" field
					if (str.indexOf("Serial") > -1) {
						strCPU = str.substring(str.indexOf(":") + 1, str.length());
						cpuAddress = strCPU.trim();
						break;
					}
				} else {
					//eof 
					break;
				}
			}
		} catch (IOException ex) {
			ex.printStackTrace();
		}
		
		return cpuAddress;
	}

	public static String getSerialNumber(){ 

	    String serial = "0000000000000000"; 

	    try { 
			if(Build.VERSION.SDK_INT>=26)//android 8.0 api 26
			{
				Class<?> c =Class.forName("android.os.Build");
				Method getSerial =c.getMethod("getSerial");
				serial = (String)getSerial.invoke(null);
			}
			else
			{
				Class<?> c =Class.forName("android.os.SystemProperties"); 
				Method get =c.getMethod("get", String.class); 
				serial = (String)get.invoke(c, "ro.serialno"); 
				//serial= Build.SERIAL;
			}
	    } catch (Exception e) { 
	       e.printStackTrace(); 
	    } 

	    return serial; 
	} 

	public static String getUserID(Context context) {	
		String uniqueId = "UNKNOWN";
		String tmDevice, androidId, cpuSerial, serial;
		try {
			try {
				final TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
				tmDevice = "" + tm.getDeviceId();
			} catch (Exception e) {
				tmDevice = "00000000";
				e.printStackTrace();
			}
			LogTrace("tmDevice : " + tmDevice);
			try {
				androidId = "" + android.provider.Settings.Secure.getString(
							context.getContentResolver(),
							android.provider.Settings.Secure.ANDROID_ID);
			} catch (Exception e) {
				androidId = "UNKNOWN";
				e.printStackTrace();
			}
			//LogTrace("androidId : " + androidId);
			//cpuSerial = "" + getCPUSerial();
			//LogTrace("getCPUSerial : " + cpuSerial);
			serial = getSerialNumber();
			//LogTrace("getSerialNumber : " + serial);
			UUID deviceUuid = new UUID(androidId.hashCode(), ((long) tmDevice.hashCode() << 32) |serial.hashCode());
			uniqueId = deviceUuid.toString();
		} catch (Exception e) {
			e.printStackTrace();
		}

		return uniqueId;
	} 

	public static String getCustomMetaData(Context context, String metadataKey) {
		String metadataValue = "UNKNOWN";
		try {
	        PackageManager packageManager = context.getPackageManager();
	        ApplicationInfo applicationInfo = packageManager.getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA); 
			Object obj = (Object)applicationInfo.metaData.get(metadataKey);
			if(obj != null)
			{
				metadataValue = obj.toString();
			}
		} catch (Exception e) {
			e.printStackTrace();
			metadataKey = "";
		}

		return metadataValue;
	}
   
	public static String getAppPackageName(Context context) {  
		String packageName = "UNKNOWN";

        //PackageInfo packageInfo = null;  
        try {  
			String pkName = context.getPackageName();
 			//packageInfo = context.getPackageManager().getPackageInfo(pkName, 0);
			packageName = pkName; // + packageInfo.versionName + packageInfo.versionCode; 
        } catch (Exception e) {  
        	e.printStackTrace(); 
        }  

        return packageName;  
    }

	private static String getAppMainName(Context context) {  
		String applicationName = "UNKNOWN";
        PackageManager packageManager = null;  
        ApplicationInfo applicationInfo = null;  
        try {  
            packageManager = context.getPackageManager();
            applicationInfo = packageManager.getApplicationInfo(context.getApplicationInfo().packageName, 0);  
			applicationName = (String) packageManager.getApplicationLabel(applicationInfo); 
        } catch (PackageManager.NameNotFoundException e) {  
            applicationInfo = null;  
        }  
        return applicationName;   
    } 

	public static String getApplicationName(Context context) {  
		String applicationName = "UNKNOWN";
        PackageManager packageManager = null;  
        ApplicationInfo applicationInfo = null;  
        try {  
            packageManager = context.getPackageManager();
            applicationInfo = packageManager.getApplicationInfo(context.getApplicationInfo().packageName, 0);  
			applicationName = (String) packageManager.getApplicationLabel(applicationInfo); 
			
			PackageInfo packInfo = packageManager.getPackageInfo(context.getPackageName(),0);  
			String version = packInfo.versionName;  
			int versionCode = packInfo.versionCode;  
			if (version != null)
			{
				applicationName += " " + version;
			}
			String channel = getCustomMetaData(context, "APP_CHANNEL_ID");
			if(channel != "UNKNOWN")
			{	
				applicationName += "." + channel;
			}
			applicationName += "(" + versionCode + ")";
        } catch (PackageManager.NameNotFoundException e) {  
            applicationInfo = null;  
        }  
        return applicationName;   
    }  

	public static native boolean IsInMachine();
	public static native boolean IsUseUnityForSVR();
	public static native boolean IsUseForDayDream();
	public static native boolean ReportReInit();
	public static native void AppExit();
	public static native String AppGetRunID();
	public static native boolean AppResume(String uniqueID);
	public static native void AppPause();
	public static native void AppPageStart(String pageName);
	public static native void AppPageEnd(String pageName);
	public static native void AppSetEvent(String eventName, String eChannelID, String eInName, float eInData, String eOutName, float eOutData);
	public static native void AppReportLog(int iLogType, String typeName, String logContent);
	public static native void AppReportUserAction(String strActionType, String strItemID, String strJsonValue);
	public static native void AppSetContinueInterval(int interval);
	public static native void AppSetReportInterval(int interval);
	public static native void AppSetReportImmediate(boolean bImmediate);

	////////////////////////////////////////////////////////////////
	public static native void OnKeyEvent(String deviceName, int buttonId, boolean buttonDown);
	public static native void OnAxisEvent(String deviceName, int AxisID, float value);
	public static native void OnTouchPadStatusChange(int jiDeviceID, boolean bisTouched);
	public static native void OnTouchPadPos(int jiDeviceID, float x, float y);
	// API FOR SENSORS
	public static native boolean StartTracker(int nSampleFrequence);
	public static native boolean StartGlassTracker(String glassName);
	public static native int CheckSensors();
	public static native void ResetSensorOrientation();
	public static native void ResetTracker();
	public static native float IsTrackerCalibrated();
	public static native boolean StartTrackerCalibration();
	public static native void getLastHeadView(float[] headView);
	public static native int  getPredictionHeadView(float[] headView, double time);
	
	public static native void getLastHeadEulerAngles(float[] headEulerAngles);
	public static native void getLastHeadQuarternion(float[] jUnitQuarternion);
	public static native void getDirectionalInScreen(float[] array);

	public static native void SendSensorData(float[] sensorArr , double second);
	public static native void StopTracker();
	public static native void SendControllerData(byte[] data, int len);
	public static native void SendControllerDataV2(float[] data, long timestamp, boolean isRecenter);
	// API FOR DISPLAY/DISTORTION
	public static native boolean DrawTexture(int LeftTexID, int RightTexID);	
	// public static native boolean DrawTextureWithSameOverlay(int LeftTexID, int RightTexID, int OverlayID);
	public static native boolean DrawTextureWithOverlay(int LeftTexID, int RightTexID, int LeftOverlayID, int RightOverlayID);

	public static native void SetOverlayPosition(float fLeft, float fTop, float fWidth, float fHeight);
	public static native void SetOverlayPosition3D(int eyeTextureType, float fWidth, float fHeight, float fDistanceInMetre);
	public static native void SetOverlayPosition3DV2(int eyeTextureType, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre);
	public static native float GetMojingWorldFOV();	
	private static native int GetEyeTexture(int eyeTextureType, int[] outParams);
	public static native float GetGlassesSeparationInPix();
	public static native void SetImageYOffset(float fYOffset);
	public static native void SetCenterLine(int iWidth , int colR, int colG, int colB, int colA);
	public static void SetCenterLine(int iWidth )
	{
		SetCenterLine(iWidth , 255 , 255 ,255 , 255);
	}
	// API FOR GLASSES MANAGER
	public static native String GetManufacturerList(String strLanguageCodeByISO639);
	public static native String GetProductList(String strManufacturerKey, String strLanguageCodeByISO639);
	public static native String GetGlassList(String  strProductKey, String  strLanguageCodeByISO639);
	public static native String GetGlassInfo(String  strGlassKey, String  strLanguageCodeByISO639);
	public static native String GenerationGlassKey(String  strProductQRCode, String  strGlassQRCode);
	public static native boolean SetDefaultMojingWorld(String strGlassesKey);
	public static native String GetDefaultMojingWorld(String strLanguageCodeByISO639);
	public static native String GetLastMojingWorld(String strLanguageCodeByISO639);

	public static native String GetSDKVersion();
	public static native boolean GetInitSDK();
	public static native boolean GetStartTracker();
	public static native boolean GetInMojingWorld();
	public static native String GetGlasses();
	public static native float GetScreenPPI();
	public static native float GetDistortionR(String strGlassesKey);
	public static native String GetUserSettings();
	public static native boolean SetUserSettings(String strUserSettings);
	public static native int GetSensorOriginStatus();
	public static native boolean SetSensorOriginStatus(int iSensorOrigin);
	public static native void SetEnableTimeWarp(boolean bEnable);

	// Mojing5 API
	public static native boolean IsLowPower();
	public static native void SetHDMWorking(boolean bHDMWorking);
	public static native void SetGlassesSN(String strGlassesSN);
	
	public static native int GetSocketPort();

	//Motion Input Device
	public static native int  DeviceGetKeymask(int iID, int[] KeyMask);
	public static native float DeviceGetInfo(int iID, float[] QuartArray, float[] AngularAccelArray, float[] LinearAccelArray, float[] PositionArray, int[] KeystatusArray);
	public static native float DeviceGetFixInfo(int iID, float[] QuartArray, float[] AngularAccelArray, float[] LinearAccelArray, float[] PositionArray);
	public static native float DeviceGetControlFixCurrentInfo(int iID, float[] QuartArray, float[] AngularAccelArray, float[] LinearAccelArray, float[] PositionArray, int[] KeystatusArray);
	
	// Joystatic ?? 
	public static native String GetEliminateBuiltin();
	public static native String GetJoystickFileName();
	public static native void NativeSetMojing2Number(int iNumber);
	public static native void NativeSetMojing3Number(int iNumber);
	public static native void NativeSetMojing4Number(int iNumber);
	private static native void NativeCleanDeviceMap();
	private static native void NativeBeginUpdateDeviceMap();
	private static native void NativeEndUpdateDeviceMap();
	private static native void NativeAddDeviceToMap(int jiDeviceID, String jsDeviceName);
	

	public static native boolean DirectionalRadiaInRect(float[] jDirection, float[] v2TopLeft, float[] v2BottomRight, float fZ, float[] v2OutPoint);
	public static native int SelectRectByDirectional(float[]  jm4Direction, float[]  jv3TopLeft, float[]  jv3BottomRight);
	public static class KTXInfo
	{
		public int iTextureID = 0;
		public int iTarget = 0;
		public int iHeight = 0;
		public int iWidth = 0;
		public int iDepth = 0;
		public boolean bIsMipmapped;
		public int iGLError = 0;
		public int iKTXError = 0;
		public KTXInfo()
		{
			
		}
	}
	
	public static native boolean NativektxLoadTextureN(String jsPath, KTXInfo info);
	// SDK Interface For Unreal
	public static void onNativeActivePause()
	{
		if (g_DeviceTimer != null)
			g_DeviceTimer.cancel();		

		g_DeviceTimer = null;
	}

	public static void onDisableVrService(boolean bDisabled)
	{
		m_bVrServiceDisabled = bDisabled;
	}

	public static boolean IsVrServiceDisabled()
	{
		return m_bVrServiceDisabled;
	}

	public static boolean IsSensorFromJava()
	{
		return MojingSDKSensorManager.useJavaSensor();
	}

	/*
	public static int GetSensorOriginStatus()
	{
		if(MojingSDKServiceManager.isHMDWorking())
		//if(MojingSDKServiceManager.isServiceTracker())
		{
			return SENSOR_ORIGIN_EXTERNAL_DEVICE;
		}
		else
		{ 
			try {
				String json = GetUserSettings();
				JSONObject jsonObject = new JSONObject(json);
				if (jsonObject.has("SensorDataFromMJSDK")) 
				{
					if(!jsonObject.getBoolean("SensorDataFromMJSDK"))
					{
						return SENSOR_ORIGIN_EXTERNAL_SDK;	
					}
				}
				if (jsonObject.has("SensorDataFromJava")) 
				{
					return jsonObject.getBoolean("SensorDataFromJava") ? SENSOR_ORIGIN_LOCAL_JAVA : SENSOR_ORIGIN_LOCAL_NATIVE;
				}
			} catch (JSONException e) {
				e.printStackTrace();
			}
			return SENSOR_ORIGIN_LOCAL_NATIVE;		
		}	
	}

	public static void onSensorOriginChanged(Context context)
	{
		try
		{
			boolean bChanged = false;
			if(!MojingSDKServiceManager.isServiceTracker())
			{
				String json = GetUserSettings();
				JSONObject jsonObject = new JSONObject(json);
				if (jsonObject.has("SensorDataFromJava")) 
				{
					int bFromJava = jsonObject.getInt("SensorDataFromJava");
					jsonObject.put("SensorDataFromJava", bFromJava==1 ? 0 : 1);
    				SetUserSettings(jsonObject.toString());
					json = GetUserSettings();
					jsonObject = new JSONObject(json);
					if (jsonObject.has("SensorDataFromJava")) 
					{
						if(jsonObject.getInt("SensorDataFromJava") != bFromJava)
						{
							bChanged = true;
						}
					}
				}
			}
			if(bChanged)
			{
				StopTracker();
				MojingSDKSensorManager.UnRegisterSensor(context);
				MojingSDKSensorManager.RegisterSensor(context);
				MojingSDKServiceManager.StartTracker();
			}
		} 
		catch (JSONException e) {
			e.printStackTrace();
		}
	}
	*/


    public static void onSensorOriginChanged(Context context)
	{
		if(MojingSDKServiceManager.isHMDWorking())
		{
			LogTrace("Sensor origin isHMDWorking.");
			return;
		}

		/*
		boolean bChanged = false;
		try
		{
			String json = GetUserSettings();
			JSONObject jsonObject = new JSONObject(json);
			if (jsonObject.has("SensorDataFromJava")) 
			{
				int bFromJava = jsonObject.getInt("SensorDataFromJava");
				jsonObject.put("SensorDataFromJava", bFromJava==1 ? 0 : 1);
    			SetUserSettings(jsonObject.toString());
				json = GetUserSettings();
				jsonObject = new JSONObject(json);
				if (jsonObject.has("SensorDataFromJava")) 
				{
					if(jsonObject.getInt("SensorDataFromJava") != bFromJava)
					{
						bChanged = true;
					}
				}
			}
		} 
		catch (JSONException e) {
			e.printStackTrace();
		}
		*/
		int iSensorOrigin = GetSensorOriginStatus();
		SetSensorOriginStatus((iSensorOrigin+1)%3);
		
		//if(bChanged)
		if(iSensorOrigin != GetSensorOriginStatus())
		{
			LogTrace("Sensor origin is set to " + GetSensorOriginStatus());
			if(MojingSDKServiceManager.isServiceTracker())
			{
				MojingSDKServiceManager.ClientReStart();
			}
			else
			{
				StopTracker();
				MojingSDKSensorManager.UnRegisterSensor(context);
				MojingSDKSensorManager.RegisterSensor(context);
				MojingSDKServiceManager.StartTracker();
			}
		}
		else
		{
			LogWarning("Sensor origin is already " + iSensorOrigin);	
		}
	}

	public static void onSensorOriginChanged(Context context, int iSensorOrigin)
	{
		if(MojingSDKServiceManager.isHMDWorking())
		{
			LogTrace("Sensor origin isHMDWorking.");
			SetSensorOriginStatus(iSensorOrigin);
			return;
		 }

		if(iSensorOrigin == GetSensorOriginStatus()) 
		{
			LogWarning("Sensor origin is already " + iSensorOrigin);
			return;
		}

		SetSensorOriginStatus(iSensorOrigin);
		
		if(iSensorOrigin == GetSensorOriginStatus())
		{
			LogTrace("Sensor origin is set to " + iSensorOrigin);
			if(MojingSDKServiceManager.isServiceTracker())
			{
				MojingSDKServiceManager.ClientReStart();
			}
			else
			{
				StopTracker();
				MojingSDKSensorManager.UnRegisterSensor(context);
				MojingSDKSensorManager.RegisterSensor(context);
				MojingSDKServiceManager.StartTracker();
			}
		}
		else
		{
			LogWarning("Sensor origin is not set to " + iSensorOrigin);
		}
	}


	private static Object g_DeviceTimerSync = new Object();
	public static void onNativeActiveResume()
	{
		if (g_DeviceTimer == null)
			g_DeviceTimer = new Timer();
		g_DeviceTimer.schedule(new TimerTask()
		{
			@Override
			public void run()
			{
				synchronized (g_DeviceTimerSync)
				{
					// Test if Disconnect is called!
					if (g_DeviceTimer != null)
					{
						// Find within all input devices
						NativeBeginUpdateDeviceMap();
						int[] temp = InputDevice.getDeviceIds();
						for(int i = 0; i < temp.length; i++) 
						{
							InputDevice inputDevice = InputDevice.getDevice(temp[i]);
							////////////////////////////////////////////////////////
							int deviceID = inputDevice.getId();
							String strDeviceName = inputDevice.getName();
							////////////////////////////////////////////////////////
							NativeAddDeviceToMap(deviceID , strDeviceName);
						}
						NativeEndUpdateDeviceMap();
					}// end of if
				}// end of synchronized (g_DeviceTimerSync)
			}// end of run
		}, 0, 5000);
	}

	public static native void VrFuncInit(boolean isDD);
	public static native void ReprojFuncInit();
	public static native void NativeUnityFuncInit();
	public static native void NativeEnableShowTip(boolean var0);

	//////////////////////////////////////////////////////////////////////////////
	private static native void Log(int logLevel, String sInfo, String sFileName, int line);

	private static void Log(int logLevel, String sInfo)
	{
		StackTraceElement[] stacktrace = Thread.currentThread().getStackTrace();
		// 0 is dalvik.system.VMStack.getThreadStackTrace
		// 1 is java.lang.Thread.getStackTrace
		// 2 is this function: Log
		// 3 is the caller
		String tag = "[" + stacktrace[4].getMethodName() + "] " + sInfo;
		Log((int)logLevel, tag, stacktrace[4].getFileName(), stacktrace[4].getLineNumber());
      }

	  public static void LogError(String sInfo)
	  {
		Log(40000, sInfo);
	  }

	  public static void LogWarning(String sInfo)
	  {
		Log(30000, sInfo);
	  }

	  public static void LogDebug(String sInfo)
	  {
		Log(10000, sInfo);
	  }

  	  public static void LogTrace(String sInfo)
	  {
		Log(0, sInfo);
	  }

	  public static boolean isDDPhone()
	  {
		  if(mContext == null)
		  	return false;
		  return Build.VERSION.SDK_INT < 24 ? false : mContext.getPackageManager().hasSystemFeature("android.hardware.vr.high_performance");
	  }

	public static Context mContext = null;
	public static int getSwapMode() {
		 if(isDDPhone()){
			return 1;
		}else {
			 return 0;
		 }
	}

	public static void hookFun()
	{
		VrFuncInit(isDDPhone());
		if( ! isDDPhone()){
			ReprojFuncInit();
		}
	}

	public static void enableShowTip(boolean bable)
	{
		NativeEnableShowTip(bable);
	}

	public static void hookUnityFun()
	{
		NativeUnityFuncInit();
	}

	public static void setsDaydreamPhoneOverrideForTesting() {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
			try {
				Class cls = Class.forName("com.google.vr.ndk.base.DaydreamUtils");
				Field f = cls.getDeclaredField("sDaydreamPhoneOverrideForTesting");
				f.setAccessible(true);
				f.set(null, true);
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			} catch (NoSuchFieldException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			}
		}
	}


	public static void setsFingerprint() {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
			try {
				Class cls = Class.forName("android.os.Build");
				Field f = cls.getDeclaredField("FINGERPRINT");
				f.setAccessible(true);
				String fingerPrint = (String) f.get(null);
				if (!TextUtils.isEmpty(fingerPrint)) {
					if (!fingerPrint.endsWith("dev-keys")) {
						String end = fingerPrint.substring(fingerPrint.length() - 8);
						String newFingerPrint = fingerPrint.replace(end, "dev-keys");
						f.set(null, newFingerPrint);
					}
				}
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			} catch (NoSuchFieldException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			}
		}
	}

}
