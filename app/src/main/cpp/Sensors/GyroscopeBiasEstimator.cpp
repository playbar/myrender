//
// Created by houguoli on 2017/5/17.
//

#include "GyroscopeBiasEstimator.h"

float GyroscopeBiasEstimator::ACCEL_LOWPASS_FREQ = 1.0f;
float GyroscopeBiasEstimator::GYRO_LOWPASS_FREQ = 10.0f;
 float GyroscopeBiasEstimator::GYRO_BIAS_LOWPASS_FREQ = 0.15f;
 int GyroscopeBiasEstimator::NUM_GYRO_BIAS_SAMPLES_THRESHOLD = 30;
 int GyroscopeBiasEstimator::NUM_GYRO_BIAS_SAMPLES_INITIAL_SMOOTHING = 100;
 float GyroscopeBiasEstimator::ACCEL_DIFF_STATIC_THRESHOLD = 0.5f;
 float GyroscopeBiasEstimator::GYRO_DIFF_STATIC_THRESHOLD = 0.008f;
 float GyroscopeBiasEstimator::GYRO_FOR_BIAS_THRESHOLD = 0.35f;
 int GyroscopeBiasEstimator::IS_STATIC_NUM_FRAMES_THRESHOLD = 10;
