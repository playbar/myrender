//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.baofeng.mojing;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.os.Build.VERSION;
import android.provider.Settings.Secure;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.view.InputDevice;
import com.baofeng.mojing.sensor.MojingSDKSensorManager;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

public class MojingSDK {
    public static int SENSOR_ERROR_NOERROR = 0;
    public static int SENSOR_ERROR_NoMag = 1;
    public static int SENSOR_ERROR_NoGryo = 4;
    public static int SENSOR_ERROR_GryoTooSlow = 8;
    public static int SENSOR_ERROR_NoAccel = 16;
    public static int SENSOR_ERROR_AccelTooSlow = 32;
    public static int SENSOR_ORIGIN_EXTERNAL_SDK = 0;
    public static int SENSOR_ORIGIN_DAYDREAM = 0;
    public static int SENSOR_ORIGIN_LOCAL_NATIVE = 1;
    public static int SENSOR_ORIGIN_LOCAL_JAVA = 2;
    public static int SENSOR_ORIGIN_EXTERNAL_DEVICE = 3;
    public static int EYE_TEXTURE_TYPE_LEFT = 1;
    public static int EYE_TEXTURE_TYPE_RIGHT = 2;
    public static int EYE_TEXTURE_TYPE_BOTH = 3;
    private static Timer g_DeviceTimer;
    private static boolean m_bVrServiceDisabled;
    private static boolean m_inited;
    private static Object g_DeviceTimerSync;
    public static Context mContext;

    public MojingSDK() {
    }

    public static native int GetSystemIntProperty(String var0, int var1);

    private static native boolean Init(String var0, String var1, String var2, String var3, String var4, String var5, String var6, int var7, int var8, float var9, float var10, String var11);

    private static native void CheckPackage(String var0, String var1);

    public static native void SetEngineVersion(String var0);

    public static boolean Init(Context context) {
        if(!m_inited) {
            String path = exportFromAssetsFile(context);
            String appName = getApplicationName(context);
            String packageName = getAppPackageName(context);
            String userID = getUserID(context);
            String channelID = getCustomMetaData(context, "DEVELOPER_CHANNEL_ID");
            String appID = getCustomMetaData(context, "DEVELOPER_APP_ID");
            String appKey = getCustomMetaData(context, "DEVELOPER_APP_KEY");
            String merchantID = getCustomMetaData(context, "DEVELOPER_MERCHANT_ID");
            DisplayMetrics dm = context.getResources().getDisplayMetrics();
            m_inited = true;
            Init(merchantID, appID, appKey, appName, packageName, userID, channelID, dm.widthPixels, dm.heightPixels, dm.xdpi, dm.ydpi, path);
            String appNameMain = getAppMainName(context);
            String strCaseCode = getCustomMetaData(context, "DEVELOPER_CASE_CODE");
            CheckPackage(appNameMain, strCaseCode);
        }

        return true;
    }

    public static EyeTextureParameter GetEyeTextureParameter(int eyeTextureType) {
        EyeTextureParameter Ret = new EyeTextureParameter();
        int[] Parameter = new int[]{0, 0, 0};
        Ret.m_EyeTexID = GetEyeTexture(eyeTextureType, Parameter);
        Ret.m_EyeTexType = eyeTextureType;
        Ret.m_Width = Parameter[0];
        Ret.m_Height = Parameter[1];
        Ret.m_Format = Parameter[2];
        return Ret;
    }

    private static String exportFromAssetsFile(Context context) {
        String result = null;
        File CacheDir = context.getCacheDir();
        String cachePath = CacheDir.getPath();
        File cachePathFile = new File(cachePath);
        if(cachePathFile != null && (!cachePathFile.exists() || !cachePathFile.isDirectory())) {
            cachePathFile.mkdir();
        }

        try {
            Resources r = context.getResources();
            String[] ProfileList = r.getAssets().list("MojingSDK");
            LogTrace("Find " + ProfileList.length + " file(s) in assets/MojingSDK");
            if(ProfileList.length > 0) {
                AssetManager assetManager = r.getAssets();
                String[] var8 = ProfileList;
                int var9 = ProfileList.length;

                for(int var10 = 0; var10 < var9; ++var10) {
                    String filename = var8[var10];
                    InputStream in = null;
                    FileOutputStream out = null;

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
                    } catch (IOException var15) {
                        LogError("Failed to copy asset file: " + filename + var15.toString());
                    }
                }
            }
        } catch (Exception var16) {
            var16.printStackTrace();
        }

        return cachePath;
    }

    private static void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];

        int read;
        while((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }

    }

    private static String getCPUSerial() {
        String str = "";
        String strCPU = "";
        String cpuAddress = "0000000000000000";

        try {
            Process pp = Runtime.getRuntime().exec("cat /proc/cpuinfo");
            InputStreamReader ir = new InputStreamReader(pp.getInputStream());
            LineNumberReader input = new LineNumberReader(ir);

            for(int i = 1; i < 100; ++i) {
                str = input.readLine();
                if(str == null) {
                    break;
                }

                if(str.indexOf("Serial") > -1) {
                    strCPU = str.substring(str.indexOf(":") + 1, str.length());
                    cpuAddress = strCPU.trim();
                    break;
                }
            }
        } catch (IOException var7) {
            var7.printStackTrace();
        }

        return cpuAddress;
    }

    public static String getSerialNumber() {
        String serial = "0000000000000000";

        try {
            Class c;
            Method getSerial;
            if(VERSION.SDK_INT >= 26) {
                c = Class.forName("android.os.Build");
                getSerial = c.getMethod("getSerial", new Class[0]);
                serial = (String)getSerial.invoke((Object)null, new Object[0]);
            } else {
                c = Class.forName("android.os.SystemProperties");
                getSerial = c.getMethod("get", new Class[]{String.class});
                serial = (String)getSerial.invoke(c, new Object[]{"ro.serialno"});
            }
        } catch (Exception var3) {
            var3.printStackTrace();
        }

        return serial;
    }

    public static String getUserID(Context context) {
        String uniqueId = "UNKNOWN";

        try {
            String tmDevice;
            try {
                TelephonyManager tm = (TelephonyManager)context.getSystemService("phone");
                tmDevice = "" + tm.getDeviceId();
            } catch (Exception var8) {
                tmDevice = "00000000";
                var8.printStackTrace();
            }

            LogTrace("tmDevice : " + tmDevice);

            String androidId;
            try {
                androidId = "" + Secure.getString(context.getContentResolver(), "android_id");
            } catch (Exception var7) {
                androidId = "UNKNOWN";
                var7.printStackTrace();
            }

            String serial = getSerialNumber();
            UUID deviceUuid = new UUID((long)androidId.hashCode(), (long)tmDevice.hashCode() << 32 | (long)serial.hashCode());
            uniqueId = deviceUuid.toString();
        } catch (Exception var9) {
            var9.printStackTrace();
        }

        return uniqueId;
    }

    public static String getCustomMetaData(Context context, String metadataKey) {
        String metadataValue = "UNKNOWN";

        try {
            PackageManager packageManager = context.getPackageManager();
            ApplicationInfo applicationInfo = packageManager.getApplicationInfo(context.getPackageName(), 128);
            Object obj = applicationInfo.metaData.get(metadataKey);
            if(obj != null) {
                metadataValue = obj.toString();
            }
        } catch (Exception var6) {
            var6.printStackTrace();
            metadataKey = "";
        }

        return metadataValue;
    }

    public static String getAppPackageName(Context context) {
        String packageName = "UNKNOWN";

        try {
            String pkName = context.getPackageName();
            packageName = pkName;
        } catch (Exception var3) {
            var3.printStackTrace();
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
            applicationName = (String)packageManager.getApplicationLabel(applicationInfo);
        } catch (NameNotFoundException var5) {
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
            applicationName = (String)packageManager.getApplicationLabel(applicationInfo);
            PackageInfo packInfo = packageManager.getPackageInfo(context.getPackageName(), 0);
            String version = packInfo.versionName;
            int versionCode = packInfo.versionCode;
            if(version != null) {
                applicationName = applicationName + " " + version;
            }

            String channel = getCustomMetaData(context, "APP_CHANNEL_ID");
            if(channel != "UNKNOWN") {
                applicationName = applicationName + "." + channel;
            }

            applicationName = applicationName + "(" + versionCode + ")";
        } catch (NameNotFoundException var8) {
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

    public static native boolean AppResume(String var0);

    public static native void AppPause();

    public static native void AppPageStart(String var0);

    public static native void AppPageEnd(String var0);

    public static native void AppSetEvent(String var0, String var1, String var2, float var3, String var4, float var5);

    public static native void AppReportLog(int var0, String var1, String var2);

    public static native void AppReportUserAction(String var0, String var1, String var2);

    public static native void AppSetContinueInterval(int var0);

    public static native void AppSetReportInterval(int var0);

    public static native void AppSetReportImmediate(boolean var0);

    public static native void OnKeyEvent(String var0, int var1, boolean var2);

    public static native void OnAxisEvent(String var0, int var1, float var2);

    public static native void OnTouchPadStatusChange(int var0, boolean var1);

    public static native void OnTouchPadPos(int var0, float var1, float var2);

    public static native boolean StartTracker(int var0);

    public static native boolean StartGlassTracker(String var0);

    public static native int CheckSensors();

    public static native void ResetSensorOrientation();

    public static native void ResetTracker();

    public static native float IsTrackerCalibrated();

    public static native boolean StartTrackerCalibration();

    public static native void getLastHeadView(float[] var0);

    public static native int getPredictionHeadView(float[] var0, double var1);

    public static native void getLastHeadEulerAngles(float[] var0);

    public static native void getLastHeadQuarternion(float[] var0);

    public static native void getDirectionalInScreen(float[] var0);

    public static native void SendSensorData(float[] var0, double var1);

    public static native void StopTracker();

    public static native void SendControllerData(byte[] var0, int var1);

    public static native void SendControllerDataV2(float[] var0, long var1, boolean var3);

    public static native boolean DrawTexture(int var0, int var1);

    public static native boolean DrawTextureWithOverlay(int var0, int var1, int var2, int var3);

    public static native void SetOverlayPosition(float var0, float var1, float var2, float var3);

    public static native void SetOverlayPosition3D(int var0, float var1, float var2, float var3);

    public static native void SetOverlayPosition3DV2(int var0, float var1, float var2, float var3, float var4, float var5);

    public static native float GetMojingWorldFOV();

    private static native int GetEyeTexture(int var0, int[] var1);

    public static native float GetGlassesSeparationInPix();

    public static native void SetImageYOffset(float var0);

    public static native void SetCenterLine(int var0, int var1, int var2, int var3, int var4);

    public static void SetCenterLine(int iWidth) {
        SetCenterLine(iWidth, 255, 255, 255, 255);
    }

    public static native String GetManufacturerList(String var0);

    public static native String GetProductList(String var0, String var1);

    public static native String GetGlassList(String var0, String var1);

    public static native String GetGlassInfo(String var0, String var1);

    public static native String GenerationGlassKey(String var0, String var1);

    public static native boolean SetDefaultMojingWorld(String var0);

    public static native String GetDefaultMojingWorld(String var0);

    public static native String GetLastMojingWorld(String var0);

    public static native String GetSDKVersion();

    public static native boolean GetInitSDK();

    public static native boolean GetStartTracker();

    public static native boolean GetInMojingWorld();

    public static native String GetGlasses();

    public static native float GetScreenPPI();

    public static native float GetDistortionR(String var0);

    public static native String GetUserSettings();

    public static native boolean SetUserSettings(String var0);

    public static native int GetSensorOriginStatus();

    public static native boolean SetSensorOriginStatus(int var0);

    public static native void SetEnableTimeWarp(boolean var0);

    public static native boolean IsLowPower();

    public static native void SetHDMWorking(boolean var0);

    public static native void SetGlassesSN(String var0);

    public static native int GetSocketPort();

    public static native int DeviceGetKeymask(int var0, int[] var1);

    public static native float DeviceGetInfo(int var0, float[] var1, float[] var2, float[] var3, float[] var4, int[] var5);

    public static native float DeviceGetFixInfo(int var0, float[] var1, float[] var2, float[] var3, float[] var4);

    public static native float DeviceGetControlFixCurrentInfo(int var0, float[] var1, float[] var2, float[] var3, float[] var4, int[] var5);

    public static native String GetEliminateBuiltin();

    public static native String GetJoystickFileName();

    public static native void NativeSetMojing2Number(int var0);

    public static native void NativeSetMojing3Number(int var0);

    public static native void NativeSetMojing4Number(int var0);

    private static native void NativeCleanDeviceMap();

    private static native void NativeBeginUpdateDeviceMap();

    private static native void NativeEndUpdateDeviceMap();

    private static native void NativeAddDeviceToMap(int var0, String var1);

    public static native boolean DirectionalRadiaInRect(float[] var0, float[] var1, float[] var2, float var3, float[] var4);

    public static native int SelectRectByDirectional(float[] var0, float[] var1, float[] var2);

    public static native boolean NativektxLoadTextureN(String var0, MojingSDK.KTXInfo var1);

    public static void onNativeActivePause() {
        if(g_DeviceTimer != null) {
            g_DeviceTimer.cancel();
        }

        g_DeviceTimer = null;
    }

    public static void onDisableVrService(boolean bDisabled) {
        m_bVrServiceDisabled = bDisabled;
    }

    public static boolean IsVrServiceDisabled() {
        return m_bVrServiceDisabled;
    }

    public static boolean IsSensorFromJava() {
        return MojingSDKSensorManager.useJavaSensor();
    }

    public static void onSensorOriginChanged(Context context) {
        if(MojingSDKServiceManager.isHMDWorking()) {
            LogTrace("Sensor origin isHMDWorking.");
        } else {
            int iSensorOrigin = GetSensorOriginStatus();
            SetSensorOriginStatus((iSensorOrigin + 1) % 3);
            if(iSensorOrigin != GetSensorOriginStatus()) {
                LogTrace("Sensor origin is set to " + GetSensorOriginStatus());
                if(MojingSDKServiceManager.isServiceTracker()) {
                    MojingSDKServiceManager.ClientReStart();
                } else {
                    StopTracker();
                    MojingSDKSensorManager.UnRegisterSensor(context);
                    MojingSDKSensorManager.RegisterSensor(context);
                    MojingSDKServiceManager.StartTracker();
                }
            } else {
                LogWarning("Sensor origin is already " + iSensorOrigin);
            }

        }
    }

    public static void onSensorOriginChanged(Context context, int iSensorOrigin) {
        if(MojingSDKServiceManager.isHMDWorking()) {
            LogTrace("Sensor origin isHMDWorking.");
            SetSensorOriginStatus(iSensorOrigin);
        } else if(iSensorOrigin == GetSensorOriginStatus()) {
            LogWarning("Sensor origin is already " + iSensorOrigin);
        } else {
            SetSensorOriginStatus(iSensorOrigin);
            if(iSensorOrigin == GetSensorOriginStatus()) {
                LogTrace("Sensor origin is set to " + iSensorOrigin);
                if(MojingSDKServiceManager.isServiceTracker()) {
                    MojingSDKServiceManager.ClientReStart();
                } else {
                    StopTracker();
                    MojingSDKSensorManager.UnRegisterSensor(context);
                    MojingSDKSensorManager.RegisterSensor(context);
                    MojingSDKServiceManager.StartTracker();
                }
            } else {
                LogWarning("Sensor origin is not set to " + iSensorOrigin);
            }

        }
    }

    public static void onNativeActiveResume() {
        if(g_DeviceTimer == null) {
            g_DeviceTimer = new Timer();
        }

        g_DeviceTimer.schedule(new TimerTask() {
            public void run() {
                synchronized(MojingSDK.g_DeviceTimerSync) {
                    if(MojingSDK.g_DeviceTimer != null) {
                        MojingSDK.NativeBeginUpdateDeviceMap();
                        int[] temp = InputDevice.getDeviceIds();

                        for(int i = 0; i < temp.length; ++i) {
                            InputDevice inputDevice = InputDevice.getDevice(temp[i]);
                            int deviceID = inputDevice.getId();
                            String strDeviceName = inputDevice.getName();
                            MojingSDK.NativeAddDeviceToMap(deviceID, strDeviceName);
                        }

                        MojingSDK.NativeEndUpdateDeviceMap();
                    }

                }
            }
        }, 0L, 5000L);
    }

    public static native void VrFuncInit(boolean var0);

    public static native void ReprojFuncInit();

    public static native void NativeUnityFuncInit();

    public static native void nativeEnableShowTip(boolean var0);

    private static native void Log(int var0, String var1, String var2, int var3);

    private static void Log(int logLevel, String sInfo) {
        StackTraceElement[] stacktrace = Thread.currentThread().getStackTrace();
        String tag = "[" + stacktrace[4].getMethodName() + "] " + sInfo;
        Log(logLevel, tag, stacktrace[4].getFileName(), stacktrace[4].getLineNumber());
    }

    public static void LogError(String sInfo) {
        Log('鱀', sInfo);
    }

    public static void LogWarning(String sInfo) {
        Log(30000, sInfo);
    }

    public static void LogDebug(String sInfo) {
        Log(10000, sInfo);
    }

    public static void LogTrace(String sInfo) {
        Log(0, sInfo);
    }

    public static boolean isDDPhone() {
        return mContext == null?false:(VERSION.SDK_INT < 24?false:mContext.getPackageManager().hasSystemFeature("android.hardware.vr.high_performance"));
    }

    public static int getSwapMode() {
        return isDDPhone()?1:0;
    }

    public static void hookFun() {
        VrFuncInit(isDDPhone());
        if(!isDDPhone()) {
            ReprojFuncInit();
        }
    }

    public static void enableShowTip(boolean bable)
    {
        nativeEnableShowTip(bable);
    }

    public static void hookUnityFun() {
        NativeUnityFuncInit();
    }

    public static void setsDaydreamPhoneOverrideForTesting() {
        if(VERSION.SDK_INT >= 21) {
            try {
                Class cls = Class.forName("com.google.vr.ndk.base.DaydreamUtils");
                Field f = cls.getDeclaredField("sDaydreamPhoneOverrideForTesting");
                f.setAccessible(true);
                f.set((Object)null, Boolean.valueOf(true));
            } catch (ClassNotFoundException var2) {
                var2.printStackTrace();
            } catch (NoSuchFieldException var3) {
                var3.printStackTrace();
            } catch (IllegalAccessException var4) {
                var4.printStackTrace();
            }
        }

    }

    public static void setsFingerprint() {
        if(VERSION.SDK_INT >= 21) {
            try {
                Class cls = Class.forName("android.os.Build");
                Field f = cls.getDeclaredField("FINGERPRINT");
                f.setAccessible(true);
                String fingerPrint = (String)f.get((Object)null);
                if(!TextUtils.isEmpty(fingerPrint) && !fingerPrint.endsWith("dev-keys")) {
                    String end = fingerPrint.substring(fingerPrint.length() - 8);
                    String newFingerPrint = fingerPrint.replace(end, "dev-keys");
                    f.set((Object)null, newFingerPrint);
                }
            } catch (ClassNotFoundException var5) {
                var5.printStackTrace();
            } catch (NoSuchFieldException var6) {
                var6.printStackTrace();
            } catch (IllegalAccessException var7) {
                var7.printStackTrace();
            }
        }

    }

    static {
        System.loadLibrary("curl");
        System.loadLibrary("sqlite3");
        System.loadLibrary("mojing");
        m_bVrServiceDisabled = false;
        m_inited = false;
        g_DeviceTimerSync = new Object();
        mContext = null;
    }

    public static class KTXInfo {
        public int iTextureID = 0;
        public int iTarget = 0;
        public int iHeight = 0;
        public int iWidth = 0;
        public int iDepth = 0;
        public boolean bIsMipmapped;
        public int iGLError = 0;
        public int iKTXError = 0;

        public KTXInfo() {
        }
    }
}
