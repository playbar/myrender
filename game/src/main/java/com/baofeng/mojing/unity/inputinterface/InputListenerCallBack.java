package com.baofeng.mojing.unity.inputinterface;

/**
 * Created by luomin on 2016/5/17.
 */
public interface InputListenerCallBack {
    void OnButtonDown(String keycode);
    void OnButtonUp(String keycode);
    void OnButtonLongPress(String keycode);
    void onDeviceAttached(String deviceName);
    void onDeviceDetached(String deviceName);
    void onMove(String info);
    void OnTouchEvent(String event);
    void onBluetoothAdapterStateChanged(String state);
    void onTouchPadStatusChange(String info);
    void onTouchPadPos(String info);
}
