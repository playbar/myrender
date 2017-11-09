package com.baofeng.mojing.unity;

import android.app.Activity;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.util.Log;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;

import com.baofeng.mojing.MojingSDK;
import com.baofeng.mojing.MojingSDKServiceManager;
import com.baofeng.mojing.MojingSDKReport;
import com.baofeng.mojing.MojingSurfaceView;
import com.baofeng.mojing.input.MojingInputManager;
import com.baofeng.mojing.input.base.MojingInputCallback;
import com.baofeng.mojing.input.base.MojingKeyCode;
import com.baofeng.mojing.sensor.MojingSDKSensorManager;
import com.baofeng.mojing.unity.inputinterface.InputListenerCallBack;
import com.unity3d.player.UnityPlayer;
//import com.zeemote.zc.ZeemoteActivityManager;

import java.io.File;

//import com.unity3d.player.UnityPlayerActivity;
//import com.unity3d.player.UnityPlayerNativeActivity;

public class MojingVrActivity extends Activity implements MojingInputCallback {
    private final static String TAG = MojingVrActivity.class.getSimpleName();
    private UnityPlayer mUnityPlayer = null;
//    private MojingInputManager joystick = MojingInputManager.getMojingInputManager();
    private static String inputMapfilepath = null;
    private static int _mojing_send_volume_key_event_status = MojingInputManager.MOJING_SEND_VOLUME_KEY_EVENT;
    private static boolean isUseCustomerInputMap = false;
    private Activity mActivity = null;
    private int lastWidth = 0;
    private int lastHeight = 0;
    private static InputListenerCallBack listenerCB = null;
    public static MojingVrActivity _this = null;

	protected UnityPlayer getUnityPlayer()
	{
		return mUnityPlayer;
	}
	
    public static void SetListenerCB(InputListenerCallBack cb)
    {
	     listenerCB = cb;
    }

    public void OnButtonDown( String keycode)
    {
         if (listenerCB != null)
        {
           listenerCB.OnButtonDown(keycode);
        }
     }

    public void OnButtonUp(String keycode)
    {
        if (listenerCB != null)
        {
            listenerCB.OnButtonUp(keycode);
        }
    }
    public void OnButtonLongPress(String keycode)
    {
        if (listenerCB != null)
        {
            listenerCB.OnButtonLongPress(keycode);
        }
    }
    public void onDeviceAttached(String deviceName)
    {
        if(listenerCB!=null) {
            listenerCB.onDeviceAttached(deviceName);
        }
    }
    public void onDeviceDetached(String deviceName)
    {
        if(listenerCB!=null) {
            listenerCB.onDeviceDetached(deviceName);
        }
    }

    public void onMove(String info)
    {
        if(listenerCB!=null) {
            listenerCB.onMove(info);
        }
    }
    public void OnTouchEvent(String event)
    {
        if (listenerCB != null)
        {
            listenerCB.OnTouchEvent(event);
        }
    }
    public void onBluetoothAdapterStateChanged(String state)
    {
        if (listenerCB != null)
        {
            listenerCB.onBluetoothAdapterStateChanged(state);
        }
    }


    public void onTouchPadPos(String info)
    {
        if (listenerCB != null)
        {
            listenerCB.onTouchPadPos(info);
        }
    }

    public void onTouchPadStatusChange(String info)
    {
        if (listenerCB != null)
        {
            listenerCB.onTouchPadStatusChange(info);
        }
    }

    private boolean isDDAPP()
    {
        try {
            File f = new File(getApplicationContext().getApplicationInfo().nativeLibraryDir+"/libgvr.so");
            if (!f.exists()) {
                Log.d(TAG, "Check Daydream app return false.");
                return false;
            }
        }
        catch (Exception e) {
            Log.d(TAG, "Check Daydream app exception.");
            return false;
        }
        Log.d(TAG, "Check Daydream app return ok.");
        return true;
    }
    // Setup activity layout
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Code from UnityPlayerNativeActivity
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);
        _this = this;
        getWindow().takeSurface(null);
        setTheme(android.R.style.Theme_NoTitleBar_Fullscreen);
        getWindow().setFormat(PixelFormat.RGBX_8888); // <--- This makes xperia play happy
        mActivity = this;
        MojingSDK.mContext = getApplicationContext();
        if(isDDAPP())
        {
            MojingSDK.setsDaydreamPhoneOverrideForTesting();
            MojingSDK.setsFingerprint();
        }

        try {
            Class<?> c = Class.forName("com.unity3d.player.UnityPlayer");
            Log.d(TAG, "Succeed to find Class UnityPlayer.");
            Constructor theConstructor = null;
            try {
                theConstructor = c.getDeclaredConstructor(ContextWrapper.class); // 获取指定类的公有构造方法
            }
            catch(Exception e)
            {
                Log.d(TAG, "Failed to find Constructor of UnityPlayer(ContextWrapper).");
                e.printStackTrace();
            }
            if(theConstructor == null)
            {
                theConstructor = c.getDeclaredConstructor(Context.class); // 获取指定类的公有构造方法
            }
            mUnityPlayer = (UnityPlayer) theConstructor.newInstance(this);
            Log.d(TAG, "Succeed to find Constructor of UnityPlayer.");
            if (mUnityPlayer == null)
            {
                Log.d(TAG, "Faild to new UnityPlayer.");
                return;
            }
            Log.d(TAG, "Succeed to new UnityPlayer.");
        }
        catch(Exception e)
        {
            Log.d(TAG, "Failed to find Constructor of UnityPlayer.");
            e.printStackTrace();
        }
        //mUnityPlayer = new UnityPlayer(this);
        if(isDDAPP())
        {
            MojingSDK.hookUnityFun();
        }
        if (mUnityPlayer.getSettings().getBoolean("hide_status_bar", true)) {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }

        this.mUnityPlayer.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                return MojingVrActivity.this.onTouchEvent(event);
            }
        });

        this.mUnityPlayer.setOnGenericMotionListener(new View.OnGenericMotionListener() {
            public boolean onGenericMotion(View v, MotionEvent event) {
                return MojingVrActivity.this.onGenericMotionEvent(event);
            }
        });

        this.mUnityPlayer.setOnKeyListener(new View.OnKeyListener() {
            public boolean onKey(View v, int keyCode, KeyEvent event) {
                switch (event.getAction()) {
                    case 0:
                        return MojingVrActivity.this.onKeyDown(keyCode, event);
                    case 1:
                        return MojingVrActivity.this.onKeyUp(keyCode, event);
                }
                return MojingVrActivity.this.injectUnityEvent(event);
            }
        });

        setContentView(mUnityPlayer);
        mUnityPlayer.requestFocus();
        // End code from UnityPlayerNativeActivity

        try
        {
            Class<?> c = Class.forName("com.baofeng.mojing.check.MojingCheckService");
            Method method = c.getMethod("checkService", Activity.class);
            Log.d(TAG, "MojingCheckService checkService");
            method.invoke(c.newInstance(), this);
        }
        catch(Exception e)
        {
            Log.d(TAG, "MojingCheckService is not exist");
            e.printStackTrace();
        }  // end try

        // init mojing sdk
        MojingSDK.Init(this);
        // Add joystick support
//        joystick.AddProtocal(MojingInputManager.Protocol_Bluetooth);
        //joystick.AddProtocal(MojingInputManager.Protocol_Bluetooth_BLE);

        isUseCustomerInputMap = checkIsUseUserConfig();

        MojingSDKReport.openActivityDurationTrack(false);

        // Add system ui change listener
        getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
            @Override
            public void onSystemUiVisibilityChange(int visibility) {
                Log.i("adjustSDK","setOnSystemUiVisibilityChangeListener");
                int decorWidth = getWindow().getDecorView().getWidth();
                int decorHeight = getWindow().getDecorView().getHeight();
                Log.i("adjustSDK","decorWidth="+decorWidth+",decorHeight="+decorHeight);
                if (decorWidth != lastWidth || decorHeight != lastHeight) {
                    Log.i("adjustSDK","once");

                    MojingSurfaceView.OnSurfaceChanged(decorWidth, decorHeight);
                    UnityPlayer.UnitySendMessage("Mojing", "ResetTextureID", "textureReset");
                    lastWidth = decorWidth;
                    lastHeight = decorHeight;
                }
            }
        });
    }

    @Override
    protected void onNewIntent(Intent intent) {
        // TODO Auto-generated method stub
        super.onNewIntent(intent);
        UnityPlayer.UnitySendMessage("MojingInputManager", "OnButtonUp", "home");
    }

    boolean checkIsUseUserConfig() {
        try {
            File CacheDir = this.getCacheDir();
            File f = new File(CacheDir, "InputMap_mojing_user.json");
            MojingSDK.LogTrace(TAG + "inputMapfilepath:" + f.getAbsolutePath() + "exist:" + f.exists());
            inputMapfilepath = f.getAbsolutePath();
            return f.exists();
        } catch (Exception e) {
            return false;
        }
    }

    // Quit Unity
    @Override
    protected void onDestroy() {
        // Code from UnityPlayerNativeActivity
        mUnityPlayer.quit();
        super.onDestroy();
        // End code from UnityPlayerNativeActivity
    }

    // Pause Unity
    @Override
    protected void onPause() {
        Log.i(TAG, "onPause: IsUseUnityForSVR before");
        if(!MojingSDK.IsUseUnityForSVR())
        {
        	MojingSDKSensorManager.UnRegisterSensor(this);
        }
        Log.i(TAG, "onPause: IsUseUnityForSVR after");

        com.baofeng.mojing.MojingVrLib.stopVsync(this);

       // ZeemoteActivityManager.setActivity(null);
//        joystick.Disconnect();
        // Code from UnityPlayerNativeActivity
        Log.i(TAG, "onPause: super.onPause");
        super.onPause();
        Log.i(TAG, "onPause: mUnityPlayer.onPause");
        mUnityPlayer.pause();
        // End code from UnityPlayerNativeActivity

        Log.i(TAG, "onPause: MojingSDKReport.onPause");
        MojingSDKReport.onPause(this);

        Log.i(TAG, "onPause: IsUseUnityForSVR-2.before");
        if(!MojingSDK.IsUseUnityForSVR())
        {
            if(MojingSDK.IsInMachine()) {
                MojingSDK.StopTracker();
            }
            else {
                if(MojingSDK.IsVrServiceDisabled())
                {
                    MojingSDK.StopTracker();
                }
                else
                {
                    Log.i(TAG, "onPause: MojingSDKServiceManager.onPause before");
                    MojingSDKServiceManager.onPause(this);
                    Log.i(TAG, "onPause: MojingSDKServiceManager.onPause after");
                }
            }
      	}
        Log.i(TAG, "onPause: IsUseUnityForSVR-2.after");
        MojingSDK.LogTrace("StopTracker Done -- onPause");
    }

    // Resume Unity
    @Override
    protected void onResume() {
        Log.i(TAG, "onResume: IsUseUnityForSVR before");
        if(!MojingSDK.IsUseUnityForSVR())
        {
        	MojingSDKSensorManager.RegisterSensor(this);
        }
        Log.i(TAG, "onResume: IsUseUnityForSVR after");
        // Add joystick support
        //joystick.setMojingtouchpadDataSendingType(MojingInputManager.MOJING_TOUCHPAD_DATA_SENDING_TYPE_DPAD_TOUCHEPAD);
       // ZeemoteActivityManager.setActivity(this);

        Log.i(TAG, "onResume: joystick connect");
//        joystick.SetSendVolumeKeyEventStatus(_mojing_send_volume_key_event_status);
//        if (isUseCustomerInputMap && inputMapfilepath != null && (inputMapfilepath.isEmpty() == false)) {
//            joystick.Connect(this, inputMapfilepath);
//        } else {
//            joystick.Connect(this, null);
//        }

        // Code from UnityPlayerNativeActivity
        Log.i(TAG, "onResume: super.onResume");
        super.onResume();
        Log.i(TAG, "onResume: mUnityPlayer.onResume");
        mUnityPlayer.resume();
        // End code from UnityPlayerNativeActivity
        Log.i(TAG, "onResume: MojingSDKReport.onResume");
        MojingSDKReport.onResume(this);

        Log.i(TAG, "onResume: IsUseUnityForSVR-2 before");
        if(!MojingSDK.IsUseUnityForSVR())
        {
            if(MojingSDK.IsInMachine())
            {
                MojingSDK.StartTracker(200);
            }
            else {
                if (MojingSDK.IsVrServiceDisabled()) {
                    MojingSDK.StartTracker(100);
                } else {
                    MojingSDKServiceManager.onResume(this);
                }
            }
      	}
        com.baofeng.mojing.MojingVrLib.startVsync(this);
        Log.i(TAG, "onResume: IsUseUnityForSVR-2 after");
    }


    // Code from UnityPlayerNativeActivity
    // This ensures the layout will be correct.
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        mUnityPlayer.configurationChanged(newConfig);
        int decorWidth = getWindow().getDecorView().getWidth();
        int decorHeight = getWindow().getDecorView().getHeight();
        MojingSurfaceView.OnSurfaceChanged(decorWidth, decorHeight);
        UnityPlayer.UnitySendMessage("Mojing", "ResetTextureID", "textureReset");
    }

    // Notify Unity of the focus change.
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        mUnityPlayer.windowFocusChanged(hasFocus);
    }
// End code from UnityPlayerNativeActivity

    public boolean injectUnityEvent(InputEvent event) {
        return this.mUnityPlayer.injectEvent(event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        boolean isSend = true;
        if (_mojing_send_volume_key_event_status != MojingInputManager.MOJING_SEND_VOLUME_KEY_EVENT)
        {
            if (keyCode == MojingKeyCode.KEYCODE_VOLUME_UP || keyCode == MojingKeyCode.KEYCODE_VOLUME_DOWN)
            {
                isSend = false;
            }
        }
        if (isSend)
        {
            onMojingKeyDown("mobile", keyCode);
        }

        return injectUnityEvent(event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        boolean isSend = true;
        if (_mojing_send_volume_key_event_status != MojingInputManager.MOJING_SEND_VOLUME_KEY_EVENT)
        {
            if (keyCode == MojingKeyCode.KEYCODE_VOLUME_UP || keyCode == MojingKeyCode.KEYCODE_VOLUME_DOWN)
            {
                isSend = false;
            }
        }
        if (isSend)
        {
            onMojingKeyUp("mobile", keyCode);
        }

        return injectUnityEvent(event);
    }

    @Override
    public boolean onMojingKeyDown(String deviceName, int keyCode) {
        //MojingSDK.LogTrace("OnButtonDown" + deviceName + "/" + keyCode);
        //UnityPlayer.UnitySendMessage("MojingInputManager", "OnButtonDown", deviceName + "/" + keyCode);
        OnButtonDown(deviceName + "/" + keyCode);
        return true;
    }

    @Override
    public boolean onMojingKeyUp(String deviceName, int keyCode) {
        //MojingSDK.LogTrace("OnButtonUp" + deviceName + "/" + keyCode);
        //UnityPlayer.UnitySendMessage("MojingInputManager", "OnButtonUp", deviceName + "/" + keyCode);
        OnButtonUp(deviceName + "/" + keyCode);
        return true;
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // Code from UnityPlayerNativeActivity
        // For some reason the multiple keyevent type is not supported by the ndk.
        // Force event injection by overriding dispatchKeyEvent().
        if (event.getAction() == KeyEvent.ACTION_MULTIPLE)
            return mUnityPlayer.injectEvent(event);
        // End code from UnityPlayerNativeActivity

//        if (joystick.dispatchKeyEvent(event))
//            return true;
//        else
            return super.dispatchKeyEvent(event);
    }

    @Override
    public boolean dispatchGenericMotionEvent(MotionEvent event) {
//        if (joystick.dispatchGenericMotionEvent(event))
//            return true;
//        else
            return injectUnityEvent(event);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                //UnityPlayer.UnitySendMessage("MojingInputManager", "OnTouchEvent", "ACTION_DOWN");
                OnTouchEvent("ACTION_DOWN");
                break;
            case MotionEvent.ACTION_UP:
                //UnityPlayer.UnitySendMessage("MojingInputManager", "OnTouchEvent", "ACTION_UP");
                OnTouchEvent("ACTION_UP");
                break;
        }
        return injectUnityEvent(event);
    }

    @Override
    public void onBluetoothAdapterStateChanged(int newState) {
        Log.i("adjustSDK", "onBluetoothAdapterStateChanged");
        //UnityPlayer.UnitySendMessage("MojingInputManager", "onBluetoothAdapterStateChanged", String.format("%d", newState));
        onBluetoothAdapterStateChanged( String.format("%d", newState));
    }

    @Override
    public boolean onMojingKeyLongPress(String deviceName, int keyCode) {
        //UnityPlayer.UnitySendMessage("MojingInputManager", "onButtonLongPress", deviceName + "/" + keyCode);
        OnButtonLongPress(deviceName + "/" + keyCode);
        return true;
    }

    @Override
    public boolean onMojingMove(String deviceName, int axis, float x, float y, float z) {
        String info = String.format("%s/%d/%f,%f,%f", deviceName, axis, x, y, z);
        //UnityPlayer.UnitySendMessage("MojingInputManager", "onMove", info);
        onMove(info);
        return true;
    }

    @Override
    public boolean onMojingMove(String deviceName, int axis, float value) {
        String info = String.format("%s/%d/%f", deviceName, axis, value);
        //UnityPlayer.UnitySendMessage("MojingInputManager", "onMove", info);
        onMove(info);
        return true;
    }

    @Override
    public void onMojingDeviceAttached(String deviceName) {
        Log.i("adjustSDK", "onMojingDeviceAttached,deviceName=" + deviceName);
        UnityPlayer.UnitySendMessage("MojingInputManager", "onDeviceAttached", deviceName);
        //onDeviceAttached(deviceName);
    }

    @Override
    public void onMojingDeviceDetached(String deviceName) {
        Log.i("adjustSDK", "onMojingDeviceDetached,deviceName=" + deviceName);
        UnityPlayer.UnitySendMessage("MojingInputManager", "onDeviceDetached", deviceName);
        //onDeviceDetached(deviceName);
    }

    @Override
    public void onTouchPadStatusChange(String deviceName, boolean bisTouched) {
        String info = deviceName + "/" + bisTouched;
        Log.i("adjustSDK", "onTouchPadStatusChange,info=" + info);
        onTouchPadStatusChange(info);
    }

    @Override
    public void onTouchPadPos(String deviceName, float x, float y) {
        String info = String.format("%s/%f,%f", deviceName, x, y);
        onTouchPadPos(info);
    }

    public static boolean setCustomerInputmap(String customInputmap) {
        inputMapfilepath = customInputmap;
        isUseCustomerInputMap = true;
        return true;
    }

    public static boolean setSendVolumeKeyEventStatus(int istatus) {
        _mojing_send_volume_key_event_status = istatus;
        _this.setSendVolumeKeyEventStatus();
        return true;
    }
    private void setSendVolumeKeyEventStatus()
    {
//        if (joystick != null)
//        {
//            joystick.SetSendVolumeKeyEventStatus(_mojing_send_volume_key_event_status);
//        }
    }
}