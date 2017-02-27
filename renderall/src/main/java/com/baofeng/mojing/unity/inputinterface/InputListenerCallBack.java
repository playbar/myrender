//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.baofeng.mojing.unity.inputinterface;

public interface InputListenerCallBack {
    void OnButtonDown(String var1);

    void OnButtonUp(String var1);

    void OnButtonLongPress(String var1);

    void onDeviceAttached(String var1);

    void onDeviceDetached(String var1);

    void onMove(String var1);

    void OnTouchEvent(String var1);

    void onBluetoothAdapterStateChanged(String var1);

    void onTouchPadStatusChange(String var1);

    void onTouchPadPos(String var1);
}
