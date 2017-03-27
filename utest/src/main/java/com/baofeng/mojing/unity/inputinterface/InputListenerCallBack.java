package com.baofeng.mojing.unity.inputinterface;

public abstract interface InputListenerCallBack
{
    public abstract void OnButtonDown(String paramString);

    public abstract void OnButtonUp(String paramString);

    public abstract void OnButtonLongPress(String paramString);

    public abstract void onDeviceAttached(String paramString);

    public abstract void onDeviceDetached(String paramString);

    public abstract void onMove(String paramString);

    public abstract void OnTouchEvent(String paramString);

    public abstract void onBluetoothAdapterStateChanged(String paramString);

    public abstract void onTouchPadStatusChange(String paramString);

    public abstract void onTouchPadPos(String paramString);
}
