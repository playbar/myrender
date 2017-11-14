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
import com.unity3d.player.UnityPlayerActivity;
//import com.zeemote.zc.ZeemoteActivityManager;

import java.io.File;

//import com.unity3d.player.UnityPlayerActivity;
//import com.unity3d.player.UnityPlayerNativeActivity;

public class MojingUnityActivity extends UnityPlayerActivity {
    private final static String TAG = MojingVrActivity.class.getSimpleName();
    //    private MojingInputManager joystick = MojingInputManager.getMojingInputManager();
    private static String inputMapfilepath = null;
    private static int _mojing_send_volume_key_event_status = MojingInputManager.MOJING_SEND_VOLUME_KEY_EVENT;
    private static boolean isUseCustomerInputMap = false;
    private int lastWidth = 0;
    private int lastHeight = 0;

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
        MojingSDK.Init(this);
        getWindow().takeSurface(null);
        setTheme(android.R.style.Theme_NoTitleBar_Fullscreen);
        getWindow().setFormat(PixelFormat.RGBX_8888); // <--- This makes xperia play happy
        MojingSDK.mContext = getApplicationContext();
        if(isDDAPP())
        {
            MojingSDK.setsDaydreamPhoneOverrideForTesting();
            MojingSDK.setsFingerprint();
        }

        if(isDDAPP())
        {
            MojingSDK.hookUnityFun();
        }

        // End code from UnityPlayerNativeActivity

        // init mojing sdk

        // Add joystick support
//        joystick.AddProtocal(MojingInputManager.Protocol_Bluetooth);
        //joystick.AddProtocal(MojingInputManager.Protocol_Bluetooth_BLE);

        isUseCustomerInputMap = checkIsUseUserConfig();

        MojingSDKReport.openActivityDurationTrack(false);

//        // Add system ui change listener
//        getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
//            @Override
//            public void onSystemUiVisibilityChange(int visibility) {
//                Log.i("adjustSDK","setOnSystemUiVisibilityChangeListener");
//                int decorWidth = getWindow().getDecorView().getWidth();
//                int decorHeight = getWindow().getDecorView().getHeight();
//                Log.i("adjustSDK","decorWidth="+decorWidth+",decorHeight="+decorHeight);
//                if (decorWidth != lastWidth || decorHeight != lastHeight) {
//                    Log.i("adjustSDK","once");
//
//                    MojingSurfaceView.OnSurfaceChanged(decorWidth, decorHeight);
//                    UnityPlayer.UnitySendMessage("Mojing", "ResetTextureID", "textureReset");
//                    lastWidth = decorWidth;
//                    lastHeight = decorHeight;
//                }
//            }
//        });
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
        super.onDestroy();
        // End code from UnityPlayerNativeActivity
    }

    // Pause Unity
    @Override
    protected void onPause() {
        super.onPause();
    }

    // Resume Unity
    @Override
    protected void onResume() {
        super.onResume();
    }


    // Code from UnityPlayerNativeActivity
    // This ensures the layout will be correct.
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        int decorWidth = getWindow().getDecorView().getWidth();
        int decorHeight = getWindow().getDecorView().getHeight();
        MojingSurfaceView.OnSurfaceChanged(decorWidth, decorHeight);
        UnityPlayer.UnitySendMessage("Mojing", "ResetTextureID", "textureReset");
    }

    // Notify Unity of the focus change.
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
    }
// End code from UnityPlayerNativeActivity

    public boolean injectUnityEvent(InputEvent event) {
        return true;
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

        return injectUnityEvent(event);
    }



    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // Code from UnityPlayerNativeActivity
        // For some reason the multiple keyevent type is not supported by the ndk.
        // Force event injection by overriding dispatchKeyEvent().
//        if (event.getAction() == KeyEvent.ACTION_MULTIPLE)
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
                break;
            case MotionEvent.ACTION_UP:
                //UnityPlayer.UnitySendMessage("MojingInputManager", "OnTouchEvent", "ACTION_UP");
                break;
        }
        return injectUnityEvent(event);
    }

    public static boolean setCustomerInputmap(String customInputmap) {
        inputMapfilepath = customInputmap;
        isUseCustomerInputMap = true;
        return true;
    }

    public static boolean setSendVolumeKeyEventStatus(int istatus) {
        _mojing_send_volume_key_event_status = istatus;
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