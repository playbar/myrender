//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.os.Build;
import java.lang.Thread.UncaughtExceptionHandler;

final class Jcls implements UncaughtExceptionHandler {
    private volatile UncaughtExceptionHandler a;

    Jcls() {
    }

    final synchronized boolean a() {
        UncaughtExceptionHandler var1;
        if((var1 = Thread.getDefaultUncaughtExceptionHandler()) == this) {
            return false;
        } else {
            this.a = var1;
            Thread.setDefaultUncaughtExceptionHandler(this);
            return true;
        }
    }

    public final synchronized void uncaughtException(Thread var1, Throwable var2) {
        try {
            Error var3;
            (var3 = new Error(String.format("FATAL EXCEPTION [%s]\n", new Object[]{var1.getName()}) + String.format("Unity version     : %s\n", new Object[]{"5.6.0f3"}) + String.format("Device model      : %s %s\n", new Object[]{Build.MANUFACTURER, Build.MODEL}) + String.format("Device fingerprint: %s\n", new Object[]{Build.FINGERPRINT}))).setStackTrace(new StackTraceElement[0]);
            var3.initCause(var2);
            this.a.uncaughtException(var1, var3);
        } catch (Throwable var4) {
            this.a.uncaughtException(var1, var2);
        }
    }
}
