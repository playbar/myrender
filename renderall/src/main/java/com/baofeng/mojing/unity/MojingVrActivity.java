//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.baofeng.mojing.unity;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.SurfaceHolder.Callback2;
import android.view.View.OnGenericMotionListener;
import android.view.View.OnKeyListener;
import android.view.View.OnSystemUiVisibilityChangeListener;
import android.view.View.OnTouchListener;
import com.baofeng.mojing.MojingSDK;
import com.baofeng.mojing.MojingSDKReport;
import com.baofeng.mojing.MojingSDKServiceManager;
import com.baofeng.mojing.MojingSurfaceView;
import com.baofeng.mojing.MojingVrLib;
import com.baofeng.mojing.input.MojingInputManager;
import com.baofeng.mojing.input.base.MojingInputCallback;
import com.baofeng.mojing.sensor.MojingSDKSensorManager;
import com.baofeng.mojing.unity.inputinterface.InputListenerCallBack;
import com.unity3d.player.UnityPlayer;
import com.zeemote.zc.ZeemoteActivityManager;
import java.io.File;

public class MojingVrActivity extends Activity implements MojingInputCallback {
    private static final String TAG = MojingVrActivity.class.getSimpleName();
    private UnityPlayer mUnityPlayer = null;
    private MojingInputManager joystick = MojingInputManager.getMojingInputManager();
    private static String inputMapfilepath = null;
    private static int _mojing_send_volume_key_event_status = 1;
    private static boolean isUseCustomerInputMap = false;
    private Activity mActivity = null;
    private int lastWidth = 0;
    private int lastHeight = 0;
    private static InputListenerCallBack listenerCB = null;
    public static MojingVrActivity _this = null;

    public MojingVrActivity() {
    }

    protected UnityPlayer getUnityPlayer() {
        return this.mUnityPlayer;
    }

    public static void SetListenerCB(InputListenerCallBack cb) {
        listenerCB = cb;
    }

    public void OnButtonDown(String keycode) {
        if(listenerCB != null) {
            listenerCB.OnButtonDown(keycode);
        }

    }

    public void OnButtonUp(String keycode) {
        if(listenerCB != null) {
            listenerCB.OnButtonUp(keycode);
        }

    }

    public void OnButtonLongPress(String keycode) {
        if(listenerCB != null) {
            listenerCB.OnButtonLongPress(keycode);
        }

    }

    public void onDeviceAttached(String deviceName) {
        if(listenerCB != null) {
            listenerCB.onDeviceAttached(deviceName);
        }

    }

    public void onDeviceDetached(String deviceName) {
        if(listenerCB != null) {
            listenerCB.onDeviceDetached(deviceName);
        }

    }

    public void onMove(String info) {
        if(listenerCB != null) {
            listenerCB.onMove(info);
        }

    }

    public void OnTouchEvent(String event) {
        if(listenerCB != null) {
            listenerCB.OnTouchEvent(event);
        }

    }

    public void onBluetoothAdapterStateChanged(String state) {
        if(listenerCB != null) {
            listenerCB.onBluetoothAdapterStateChanged(state);
        }

    }

    public void onTouchPadPos(String info) {
        if(listenerCB != null) {
            listenerCB.onTouchPadPos(info);
        }

    }

    public void onTouchPadStatusChange(String info) {
        if(listenerCB != null) {
            listenerCB.onTouchPadStatusChange(info);
        }

    }

    protected void onCreate(Bundle savedInstanceState) {
        this.requestWindowFeature(1);
        super.onCreate(savedInstanceState);
        _this = this;
        this.getWindow().takeSurface((Callback2)null);
        this.setTheme(16973831);
        this.getWindow().setFormat(2);
        this.mActivity = this;
        this.mUnityPlayer = new UnityPlayer(this);
        if(this.mUnityPlayer.getSettings().getBoolean("hide_status_bar", true)) {
            this.getWindow().setFlags(1024, 1024);
        }

        this.mUnityPlayer.setOnTouchListener(new OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                return MojingVrActivity.this.onTouchEvent(event);
            }
        });
        this.mUnityPlayer.setOnGenericMotionListener(new OnGenericMotionListener() {
            public boolean onGenericMotion(View v, MotionEvent event) {
                return MojingVrActivity.this.onGenericMotionEvent(event);
            }
        });
        this.mUnityPlayer.setOnKeyListener(new OnKeyListener() {
            public boolean onKey(View v, int keyCode, KeyEvent event) {
                switch(event.getAction()) {
                    case 0:
                        return MojingVrActivity.this.onKeyDown(keyCode, event);
                    case 1:
                        return MojingVrActivity.this.onKeyUp(keyCode, event);
                    default:
                        return MojingVrActivity.this.injectUnityEvent(event);
                }
            }
        });
        this.setContentView(this.mUnityPlayer);
        this.mUnityPlayer.requestFocus();
        MojingSDK.Init(this);
        this.joystick.AddProtocal("Protocol_Bluetooth");
        isUseCustomerInputMap = this.checkIsUseUserConfig();
        MojingSDKReport.openActivityDurationTrack(false);
        this.getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(new OnSystemUiVisibilityChangeListener() {
            public void onSystemUiVisibilityChange(int visibility) {
                Log.i("adjustSDK", "setOnSystemUiVisibilityChangeListener");
                int decorWidth = MojingVrActivity.this.getWindow().getDecorView().getWidth();
                int decorHeight = MojingVrActivity.this.getWindow().getDecorView().getHeight();
                Log.i("adjustSDK", "decorWidth=" + decorWidth + ",decorHeight=" + decorHeight);
                if(decorWidth != MojingVrActivity.this.lastWidth || decorHeight != MojingVrActivity.this.lastHeight) {
                    Log.i("adjustSDK", "once");
                    MojingSurfaceView.OnSurfaceChanged(decorWidth, decorHeight);
                    UnityPlayer.UnitySendMessage("Mojing", "ResetTextureID", "textureReset");
                    MojingVrActivity.this.lastWidth = decorWidth;
                    MojingVrActivity.this.lastHeight = decorHeight;
                }

            }
        });
    }

    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        UnityPlayer.UnitySendMessage("MojingInputManager", "OnButtonUp", "home");
    }

    boolean checkIsUseUserConfig() {
        try {
            File e = this.getCacheDir();
            File f = new File(e, "InputMap_mojing_user.json");
            MojingSDK.LogTrace(TAG + "inputMapfilepath:" + f.getAbsolutePath() + "exist:" + f.exists());
            inputMapfilepath = f.getAbsolutePath();
            return f.exists();
        } catch (Exception var3) {
            return false;
        }
    }

    protected void onDestroy() {
        this.mUnityPlayer.quit();
        super.onDestroy();
    }

    protected void onPause() {
        if(!MojingSDK.IsUseUnityForSVR()) {
            MojingSDKSensorManager.UnRegisterSensor(this);
        }

        MojingVrLib.stopVsync(this);
        ZeemoteActivityManager.setActivity((Activity)null);
        this.joystick.Disconnect();
        super.onPause();
        this.mUnityPlayer.pause();
        MojingSDKReport.onPause(this);
        if(!MojingSDK.IsUseUnityForSVR()) {
            MojingSDKServiceManager.onPause(this);
        }

        MojingSDK.LogTrace("StopTracker Done -- onPause");
    }

    protected void onResume() {
        if(!MojingSDK.IsUseUnityForSVR()) {
            MojingSDKSensorManager.RegisterSensor(this);
        }

        ZeemoteActivityManager.setActivity(this);
        this.joystick.SetSendVolumeKeyEventStatus(_mojing_send_volume_key_event_status);
        if(isUseCustomerInputMap && inputMapfilepath != null && !inputMapfilepath.isEmpty()) {
            this.joystick.Connect(this, inputMapfilepath);
        } else {
            this.joystick.Connect(this, (String)null);
        }

        super.onResume();
        this.mUnityPlayer.resume();
        MojingSDKReport.onResume(this);
        if(!MojingSDK.IsUseUnityForSVR()) {
            MojingSDKServiceManager.onResume(this);
        }

        MojingVrLib.startVsync(this);
    }

    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        this.mUnityPlayer.configurationChanged(newConfig);
        int decorWidth = this.getWindow().getDecorView().getWidth();
        int decorHeight = this.getWindow().getDecorView().getHeight();
        MojingSurfaceView.OnSurfaceChanged(decorWidth, decorHeight);
        UnityPlayer.UnitySendMessage("Mojing", "ResetTextureID", "textureReset");
    }

    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        this.mUnityPlayer.windowFocusChanged(hasFocus);
    }

    public boolean injectUnityEvent(InputEvent event) {
        return this.mUnityPlayer.injectEvent(event);
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        boolean isSend = true;
        if(_mojing_send_volume_key_event_status != 1 && (keyCode == 24 || keyCode == 25)) {
            isSend = false;
        }

        if(isSend) {
            this.onMojingKeyDown("mobile", keyCode);
        }

        return this.injectUnityEvent(event);
    }

    public boolean onKeyUp(int keyCode, KeyEvent event) {
        boolean isSend = true;
        if(_mojing_send_volume_key_event_status != 1 && (keyCode == 24 || keyCode == 25)) {
            isSend = false;
        }

        if(isSend) {
            this.onMojingKeyUp("mobile", keyCode);
        }

        return this.injectUnityEvent(event);
    }

    public boolean onMojingKeyDown(String deviceName, int keyCode) {
        MojingSDK.LogTrace("OnButtonDown" + deviceName + "/" + keyCode);
        this.OnButtonDown(deviceName + "/" + keyCode);
        return true;
    }

    public boolean onMojingKeyUp(String deviceName, int keyCode) {
        MojingSDK.LogTrace("OnButtonUp" + deviceName + "/" + keyCode);
        this.OnButtonUp(deviceName + "/" + keyCode);
        return true;
    }

    public boolean dispatchKeyEvent(KeyEvent event) {
        return event.getAction() == 2?this.mUnityPlayer.injectEvent(event):(this.joystick.dispatchKeyEvent(event)?true:super.dispatchKeyEvent(event));
    }

    public boolean dispatchGenericMotionEvent(MotionEvent event) {
        return this.joystick.dispatchGenericMotionEvent(event)?true:this.injectUnityEvent(event);
    }

    public boolean onTouchEvent(MotionEvent event) {
        switch(event.getAction()) {
            case 0:
                this.OnTouchEvent("ACTION_DOWN");
                break;
            case 1:
                this.OnTouchEvent("ACTION_UP");
        }

        return this.injectUnityEvent(event);
    }

    public void onBluetoothAdapterStateChanged(int newState) {
        Log.i("adjustSDK", "onBluetoothAdapterStateChanged");
        this.onBluetoothAdapterStateChanged(String.format("%d", new Object[]{Integer.valueOf(newState)}));
    }

    public boolean onMojingKeyLongPress(String deviceName, int keyCode) {
        this.OnButtonLongPress(deviceName + "/" + keyCode);
        return true;
    }

    public boolean onMojingMove(String deviceName, int axis, float x, float y, float z) {
        String info = String.format("%s/%d/%f,%f,%f", new Object[]{deviceName, Integer.valueOf(axis), Float.valueOf(x), Float.valueOf(y), Float.valueOf(z)});
        this.onMove(info);
        return true;
    }

    public boolean onMojingMove(String deviceName, int axis, float value) {
        String info = String.format("%s/%d/%f", new Object[]{deviceName, Integer.valueOf(axis), Float.valueOf(value)});
        this.onMove(info);
        return true;
    }

    public void onMojingDeviceAttached(String deviceName) {
        Log.i("adjustSDK", "onMojingDeviceAttached,deviceName=" + deviceName);
        this.onDeviceAttached(deviceName);
    }

    public void onMojingDeviceDetached(String deviceName) {
        Log.i("adjustSDK", "onMojingDeviceDetached,deviceName=" + deviceName);
        this.onDeviceDetached(deviceName);
    }

    public void onTouchPadStatusChange(String deviceName, boolean bisTouched) {
        String info = deviceName + "/" + bisTouched;
        Log.i("adjustSDK", "onTouchPadStatusChange,info=" + info);
        this.onTouchPadStatusChange(info);
    }

    public void onTouchPadPos(String deviceName, float x, float y) {
        String info = String.format("%s/%f,%f", new Object[]{deviceName, Float.valueOf(x), Float.valueOf(y)});
        this.onTouchPadPos(info);
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

    private void setSendVolumeKeyEventStatus() {
        if(this.joystick != null) {
            this.joystick.SetSendVolumeKeyEventStatus(_mojing_send_volume_key_event_status);
        }

    }
}
