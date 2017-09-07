/*
 * Decompiled with CFR 0_119.
 */
package com.bn.mojing.sensors.internal;

public class GyroscopeBiasEstimator {
    private static final float ACCEL_LOWPASS_FREQ = 1.0f;
    private static final float GYRO_LOWPASS_FREQ = 10.0f;
    private static final float GYRO_BIAS_LOWPASS_FREQ = 0.15f;
    private static final int NUM_GYRO_BIAS_SAMPLES_THRESHOLD = 30;
    private static final int NUM_GYRO_BIAS_SAMPLES_INITIAL_SMOOTHING = 100;
    private LowPassFilter accelLowPass;
    private LowPassFilter gyroLowPass;
    private LowPassFilter gyroBiasLowPass;
    private static final float ACCEL_DIFF_STATIC_THRESHOLD = 0.5f;
    private static final float GYRO_DIFF_STATIC_THRESHOLD = 0.008f;
    private Vector3d smoothedGyroDiff;
    private Vector3d smoothedAccelDiff;
    private static final float GYRO_FOR_BIAS_THRESHOLD = 0.35f;
    private static final int IS_STATIC_NUM_FRAMES_THRESHOLD = 10;
    private IsStaticCounter isAccelStatic;
    private IsStaticCounter isGyroStatic;

    public GyroscopeBiasEstimator() {
        this.reset();
    }

    public void reset() {
        this.smoothedGyroDiff = new Vector3d();
        this.smoothedAccelDiff = new Vector3d();
        this.accelLowPass = new LowPassFilter(1.0);
        this.gyroLowPass = new LowPassFilter(10.0);
        this.gyroBiasLowPass = new LowPassFilter(0.15000000596046448);
        this.isAccelStatic = new IsStaticCounter(10);
        this.isGyroStatic = new IsStaticCounter(10);
    }

    public void processGyroscope(Vector3d gyro, long sensorTimestampNs) {
        this.gyroLowPass.addSample(gyro, sensorTimestampNs);
        Vector3d.sub(gyro, this.gyroLowPass.getFilteredData(), this.smoothedGyroDiff);
        this.isGyroStatic.appendFrame(this.smoothedGyroDiff.length() < 0.00800000037997961);
        if (this.isGyroStatic.isRecentlyStatic() && this.isAccelStatic.isRecentlyStatic()) {
            this.updateGyroBias(gyro, sensorTimestampNs);
        }
    }

    public void processAccelerometer(Vector3d accel, long sensorTimestampNs) {
        this.accelLowPass.addSample(accel, sensorTimestampNs);
        Vector3d.sub(accel, this.accelLowPass.getFilteredData(), this.smoothedAccelDiff);
        this.isAccelStatic.appendFrame(this.smoothedAccelDiff.length() < 0.5);
    }

    public void getGyroBias(Vector3d result) {
        if (this.gyroBiasLowPass.getNumSamples() < 30) {
            result.setZero();
        } else {
            result.set(this.gyroBiasLowPass.getFilteredData());
            double rampUpRatio = Math.min(1.0, (double)(this.gyroBiasLowPass.getNumSamples() - 30) / 100.0);
            result.scale(rampUpRatio);
        }
    }

    private void updateGyroBias(Vector3d gyro, long sensorTimestampNs) {
        if (gyro.length() >= 0.3499999940395355) {
            return;
        }
        double updateWeight = Math.max(0.0, 1.0 - gyro.length() / 0.3499999940395355);
        updateWeight *= updateWeight;
        this.gyroBiasLowPass.addWeightedSample(this.gyroLowPass.getFilteredData(), sensorTimestampNs, updateWeight);
    }

    private static class IsStaticCounter {
        private final int minStaticFrames;
        private int consecutiveIsStatic;

        IsStaticCounter(int minStaticFrames) {
            this.minStaticFrames = minStaticFrames;
        }

        void appendFrame(boolean isStatic) {
            this.consecutiveIsStatic = !isStatic ? 0 : ++this.consecutiveIsStatic;
        }

        boolean isRecentlyStatic() {
            return this.consecutiveIsStatic >= this.minStaticFrames;
        }
    }

}
