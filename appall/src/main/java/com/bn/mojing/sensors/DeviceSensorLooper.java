/*
 * Decompiled with CFR 0_119.
 * 
 * Could not load the following classes:
 *  android.hardware.Sensor
 *  android.hardware.SensorEvent
 *  android.hardware.SensorEventListener
 *  android.hardware.SensorManager
 *  android.os.Build
 *  android.os.Handler
 *  android.os.HandlerThread
 *  android.os.Looper
 *  android.util.Log
 */
package com.bn.mojing.sensors;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;
import java.util.ArrayList;

public class DeviceSensorLooper
implements SensorEventProvider {
    private static final String LOG_TAG = DeviceSensorLooper.class.getSimpleName();
    private boolean isRunning;
    private SensorManager sensorManager;
    private Looper sensorLooper;
    private SensorEventListener sensorEventListener;
    private final ArrayList<SensorEventListener> registeredListeners = new ArrayList();

    public DeviceSensorLooper(SensorManager sensorManager) {
        this.sensorManager = sensorManager;
    }

    private Sensor getUncalibratedGyro() {
        if (Build.MANUFACTURER.equals("HTC")) {
            return null;
        }
        return this.sensorManager.getDefaultSensor(16);
    }

    @Override
    public void start() {
        if (this.isRunning) {
            return;
        }
        this.sensorEventListener = new SensorEventListener(){

            /*
             * WARNING - Removed try catching itself - possible behaviour change.
             */
            public void onSensorChanged(SensorEvent event) {
                ArrayList arrayList = DeviceSensorLooper.this.registeredListeners;
                synchronized (arrayList) {
                    for (SensorEventListener listener : DeviceSensorLooper.this.registeredListeners) {
                        listener.onSensorChanged(event);
                    }
                }
            }

            /*
             * WARNING - Removed try catching itself - possible behaviour change.
             */
            public void onAccuracyChanged(Sensor sensor, int accuracy) {
                ArrayList arrayList = DeviceSensorLooper.this.registeredListeners;
                synchronized (arrayList) {
                    for (SensorEventListener listener : DeviceSensorLooper.this.registeredListeners) {
                        listener.onAccuracyChanged(sensor, accuracy);
                    }
                }
            }
        };
        HandlerThread sensorThread = new HandlerThread("sensor"){

            protected void onLooperPrepared() {
                Handler handler = new Handler(Looper.myLooper());
                Sensor accelerometer = DeviceSensorLooper.this.sensorManager.getDefaultSensor(1);
                DeviceSensorLooper.this.sensorManager.registerListener(DeviceSensorLooper.this.sensorEventListener, accelerometer, 0, handler);
                Sensor gyroscope = DeviceSensorLooper.this.getUncalibratedGyro();
                if (gyroscope == null) {
                    Log.i((String)LOG_TAG, (String)"Uncalibrated gyroscope unavailable, default to regular gyroscope.");
                    gyroscope = DeviceSensorLooper.this.sensorManager.getDefaultSensor(4);
                }
                DeviceSensorLooper.this.sensorManager.registerListener(DeviceSensorLooper.this.sensorEventListener, gyroscope, 0, handler);
            }
        };
        sensorThread.start();
        this.sensorLooper = sensorThread.getLooper();
        this.isRunning = true;
    }

    @Override
    public void stop() {
        if (!this.isRunning) {
            return;
        }
        this.sensorManager.unregisterListener(this.sensorEventListener);
        this.sensorEventListener = null;
        this.sensorLooper.quit();
        this.sensorLooper = null;
        this.isRunning = false;
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    @Override
    public void registerListener(SensorEventListener listener) {
        ArrayList<SensorEventListener> arrayList = this.registeredListeners;
        synchronized (arrayList) {
            this.registeredListeners.add(listener);
        }
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    @Override
    public void unregisterListener(SensorEventListener listener) {
        ArrayList<SensorEventListener> arrayList = this.registeredListeners;
        synchronized (arrayList) {
            this.registeredListeners.remove((Object)listener);
        }
    }

}
