/*
 * Decompiled with CFR 0_119.
 * 
 * Could not load the following classes:
 *  android.content.Context
 *  android.hardware.Sensor
 *  android.hardware.SensorEvent
 *  android.hardware.SensorEventListener
 *  android.hardware.SensorManager
 *  android.opengl.Matrix
 *  android.view.Display
 *  android.view.WindowManager
 */
package com.bn.mojing.sensors;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.Matrix;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import com.bn.mojing.sensors.internal.GyroscopeBiasEstimator;
import com.bn.mojing.sensors.internal.Matrix3x3d;
import com.bn.mojing.sensors.internal.OrientationEKF;
import com.bn.mojing.sensors.internal.Vector3d;

import java.util.concurrent.TimeUnit;

public class HeadTracker implements SensorEventListener {
    private static final float DEFAULT_NECK_HORIZONTAL_OFFSET = 0.08f;
    private static final float DEFAULT_NECK_VERTICAL_OFFSET = 0.075f;
    private static final float DEFAULT_NECK_MODEL_FACTOR = 1.0f;
    private static final float PREDICTION_TIME_IN_SECONDS = 0.058f;
    private final Display display;
    private final float[] ekfToHeadTracker = new float[16];
    private final float[] sensorToDisplay = new float[16];
    private float displayRotation = Float.NaN;
    private final float[] neckModelTranslation = new float[16];
    private final float[] tmpHeadView = new float[16];
    private final float[] tmpHeadView2 = new float[16];
    private float neckModelFactor = 1.0f;
    private final Object neckModelFactorMutex = new Object();
    private volatile boolean tracking;
    private final OrientationEKF tracker;
    private final Object gyroBiasEstimatorMutex = new Object();
    private GyroscopeBiasEstimator gyroBiasEstimator;
    private SensorEventProvider sensorEventProvider;
    private Clock clock;
    private long latestGyroEventClockTimeNs;
    private volatile boolean firstGyroValue = true;
    private float[] initialSystemGyroBias = new float[3];
    private final Vector3d gyroBias = new Vector3d();
    private final Vector3d latestGyro = new Vector3d();
    private final Vector3d latestAcc = new Vector3d();

    public static HeadTracker createFromContext(Context context) {
        SensorManager sensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
        Display display = ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        return new HeadTracker(new DeviceSensorLooper(sensorManager), new SystemClock(), display);
    }

    public HeadTracker(SensorEventProvider sensorEventProvider, Clock clock, Display display) {
        this.clock = clock;
        this.sensorEventProvider = sensorEventProvider;
        this.tracker = new OrientationEKF();
        this.display = display;
        this.gyroBiasEstimator = new GyroscopeBiasEstimator();
        Matrix.setIdentityM((float[]) this.neckModelTranslation, (int) 0);
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    public void onSensorChanged(SensorEvent event) {
        Thread t = Thread.currentThread();
        Log.e("headtracker", "onSensorChanged, id=" + t.getId());
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            this.latestAcc.set(event.values[0], event.values[1], event.values[2]);
            this.tracker.processAcc(this.latestAcc, event.timestamp);
            Object object = this.gyroBiasEstimatorMutex;
            synchronized (object) {
                if (this.gyroBiasEstimator != null) {
                    this.gyroBiasEstimator.processAccelerometer(this.latestAcc, event.timestamp);
                }
            }
        }
        if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE || event.sensor.getType() == Sensor.TYPE_GYROSCOPE_UNCALIBRATED) {
            this.latestGyroEventClockTimeNs = this.clock.nanoTime();
//            Log.e("timenano", "timenano: " + this.latestGyroEventClockTimeNs);
            if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE_UNCALIBRATED) {
                if (this.firstGyroValue && event.values.length == 6) {
                    this.initialSystemGyroBias[0] = event.values[3];
                    this.initialSystemGyroBias[1] = event.values[4];
                    this.initialSystemGyroBias[2] = event.values[5];
                }
                this.latestGyro.set(event.values[0] - this.initialSystemGyroBias[0], event.values[1] - this.initialSystemGyroBias[1], event.values[2] - this.initialSystemGyroBias[2]);
            } else {
                this.latestGyro.set(event.values[0], event.values[1], event.values[2]);
            }
            this.firstGyroValue = false;
//            Object object = this.gyroBiasEstimatorMutex;
//            synchronized (object) {
                if (this.gyroBiasEstimator != null) {
                    this.gyroBiasEstimator.processGyroscope(this.latestGyro, event.timestamp);
                    this.gyroBiasEstimator.getGyroBias(this.gyroBias);
                    Vector3d.sub(this.latestGyro, this.gyroBias, this.latestGyro);
                }
//            }
            this.tracker.processGyro(this.latestGyro, event.timestamp);
        }
    }

    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    public void startTracking() {
        if (this.tracking) {
            return;
        }
        this.tracker.reset();
        Object object = this.gyroBiasEstimatorMutex;
        synchronized (object) {
            if (this.gyroBiasEstimator != null) {
                this.gyroBiasEstimator.reset();
            }
        }
        this.firstGyroValue = true;
        this.sensorEventProvider.registerListener(this);
        this.sensorEventProvider.start();
        this.tracking = true;
    }

    public void resetTracker() {
        this.tracker.reset();
    }

    public void stopTracking() {
        if (!this.tracking) {
            return;
        }
        this.sensorEventProvider.unregisterListener(this);
        this.sensorEventProvider.stop();
        this.tracking = false;
    }

    public void setNeckModelEnabled(boolean enabled) {
        if (enabled) {
            this.setNeckModelFactor(1.0f);
        } else {
            this.setNeckModelFactor(0.0f);
        }
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    public float getNeckModelFactor() {
        Object object = this.neckModelFactorMutex;
        synchronized (object) {
            return this.neckModelFactor;
        }
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    public void setNeckModelFactor(float factor) {
        Object object = this.neckModelFactorMutex;
        synchronized (object) {
            if (factor < 0.0f || factor > 1.0f) {
                throw new IllegalArgumentException("factor should be within [0.0, 1.0]");
            }
            this.neckModelFactor = factor;
        }
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    public void getLastHeadView(float[] headView, int offset) {
        Thread t = Thread.currentThread();
        Log.e("headtracker", "getLastHeadView, id=" + t.getId());
        if (offset + 16 > headView.length) {
            throw new IllegalArgumentException("Not enough space to write the result");
        }
        float rotation = 0.0f;
        switch (this.display.getRotation()) {
            case 0: {
                rotation = 0.0f;
                break;
            }
            case 1: {
                rotation = 90.0f;
                break;
            }
            case 2: {
                rotation = 180.0f;
                break;
            }
            case 3: {
                rotation = 270.0f;
            }
        }
        if (rotation != this.displayRotation) {
            this.displayRotation = rotation;
            Matrix.setRotateEulerM((float[]) this.sensorToDisplay, (int) 0, (float) 0.0f, (float) 0.0f, (float) (-rotation));
            Matrix.setRotateEulerM((float[]) this.ekfToHeadTracker, (int) 0, (float) -90.0f, (float) 0.0f, (float) rotation);
        }
        OrientationEKF orientationEKF = this.tracker;
        synchronized (orientationEKF) {
            if (!this.tracker.isReady()) {
                return;
            }
            double secondsSinceLastGyroEvent = TimeUnit.NANOSECONDS.toSeconds(this.clock.nanoTime() - this.latestGyroEventClockTimeNs);
            double secondsToPredictForward = secondsSinceLastGyroEvent + 0.057999998331069946;
            double[] mat = this.tracker.getPredictedGLMatrix(secondsToPredictForward);
            for (int i = 0; i < headView.length; ++i) {
                this.tmpHeadView[i] = (float)mat[i];
            }
        }
        Matrix.multiplyMM((float[]) this.tmpHeadView2, (int) 0, (float[]) this.sensorToDisplay, (int) 0, (float[]) this.tmpHeadView, (int) 0);
        Matrix.multiplyMM((float[]) headView, (int) offset, (float[]) this.tmpHeadView2, (int) 0, (float[]) this.ekfToHeadTracker, (int) 0);
        Matrix.setIdentityM((float[]) this.neckModelTranslation, (int) 0);
        Matrix.translateM((float[]) this.neckModelTranslation, (int) 0, (float) 0.0f, (float) ((-this.neckModelFactor) * 0.075f), (float) (this.neckModelFactor * 0.08f));
        Matrix.multiplyMM((float[]) this.tmpHeadView, (int) 0, (float[]) this.neckModelTranslation, (int) 0, (float[]) headView, (int) offset);
        Matrix.translateM((float[]) headView, (int) offset, (float[]) this.tmpHeadView, (int) 0, (float) 0.0f, (float) (this.neckModelFactor * 0.075f), (float) 0.0f);
    }

    Matrix3x3d getCurrentPoseForTest() {
        return new Matrix3x3d(this.tracker.getRotationMatrix());
    }

    /*
     * WARNING - Removed try catching itself - possible behaviour change.
     */
    void setGyroBiasEstimator(GyroscopeBiasEstimator estimator) {
        Object object = this.gyroBiasEstimatorMutex;
        synchronized (object) {
            this.gyroBiasEstimator = estimator;
        }
    }
}
