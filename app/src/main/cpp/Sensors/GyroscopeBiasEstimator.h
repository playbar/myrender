//
// Created by houguoli on 2017/5/17.
//

#ifndef MYRENDER_GYROSCOPEBIASESTIMATOR_H
#define MYRENDER_GYROSCOPEBIASESTIMATOR_H


#include "LowPassFilter.h"

class IsStaticCounter {
public:
    int minStaticFrames;
    int consecutiveIsStatic;

    IsStaticCounter(int minStaticFrames) {
        minStaticFrames = minStaticFrames;
    }

    void reset(int minStaticFrames){
        minStaticFrames = minStaticFrames;
    }
    void appendFrame(bool isStatic) {
        consecutiveIsStatic = !isStatic ? 0 : ++consecutiveIsStatic;
    }

    bool isRecentlyStatic() {
        return consecutiveIsStatic >= minStaticFrames;
    }
};

class GyroscopeBiasEstimator {
public:
    static float ACCEL_LOWPASS_FREQ;
    static float GYRO_LOWPASS_FREQ;
    static  float GYRO_BIAS_LOWPASS_FREQ;
    static  int NUM_GYRO_BIAS_SAMPLES_THRESHOLD;
    static  int NUM_GYRO_BIAS_SAMPLES_INITIAL_SMOOTHING;
    static  float ACCEL_DIFF_STATIC_THRESHOLD;
    static  float GYRO_DIFF_STATIC_THRESHOLD;
    static  float GYRO_FOR_BIAS_THRESHOLD;
    static  int IS_STATIC_NUM_FRAMES_THRESHOLD;

    LowPassFilter accelLowPass;
    LowPassFilter gyroLowPass;
    LowPassFilter gyroBiasLowPass;

    Vector3d smoothedGyroDiff;
    Vector3d smoothedAccelDiff;

    IsStaticCounter isAccelStatic;
    IsStaticCounter isGyroStatic;

    GyroscopeBiasEstimator() : accelLowPass(1.0),gyroLowPass(1.0),gyroBiasLowPass(1.0), isAccelStatic(10),isGyroStatic(10)
    {
        reset();
    }

    void reset() {
        smoothedGyroDiff.reset();
        smoothedAccelDiff.reset();
        accelLowPass.reset(1.0);
        gyroLowPass.reset(10.0);
        gyroBiasLowPass.reset(0.15000000596046448);
        isAccelStatic.reset(10);
        isGyroStatic.reset(10);
    }

    void processGyroscope(Vector3d &gyro, long sensorTimestampNs) {
        gyroLowPass.addSample(gyro, sensorTimestampNs);
        Vector3d::sub(gyro, gyroLowPass.getFilteredData(), smoothedGyroDiff);
        isGyroStatic.appendFrame(smoothedGyroDiff.length() < 0.00800000037997961);
        if (isGyroStatic.isRecentlyStatic() && isAccelStatic.isRecentlyStatic()) {
            updateGyroBias(gyro, sensorTimestampNs);
        }
    }

    void processAccelerometer(Vector3d accel, long sensorTimestampNs) {
        accelLowPass.addSample(accel, sensorTimestampNs);
        Vector3d::sub(accel, accelLowPass.getFilteredData(), smoothedAccelDiff);
        isAccelStatic.appendFrame(smoothedAccelDiff.length() < 0.5);
    }

    void getGyroBias(Vector3d result) {
        if (gyroBiasLowPass.getNumSamples() < 30) {
            result.setZero();
        } else {
            result.set(gyroBiasLowPass.getFilteredData());
            double rampUpRatio = std::min(1.0, (double)(gyroBiasLowPass.getNumSamples() - 30) / 100.0);
            result.scale(rampUpRatio);
        }
    }

    void updateGyroBias(Vector3d gyro, long sensorTimestampNs) {
        if (gyro.length() >= 0.3499999940395355) {
            return;
        }
        double updateWeight = std::max(0.0, 1.0 - gyro.length() / 0.3499999940395355);
        updateWeight *= updateWeight;
        gyroBiasLowPass.addWeightedSample(gyroLowPass.getFilteredData(), sensorTimestampNs, updateWeight);
    }

};


#endif //MYRENDER_GYROSCOPEBIASESTIMATOR_H
