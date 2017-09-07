/*
 * Decompiled with CFR 0_119.
 * 
 * Could not load the following classes:
 *  android.hardware.SensorEventListener
 */
package com.bn.mojing.sensors;

import android.hardware.SensorEventListener;

public interface SensorEventProvider {
    public void start();

    public void stop();

    public void registerListener(SensorEventListener var1);

    public void unregisterListener(SensorEventListener var1);
}
