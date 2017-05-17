//
// Created by houguoli on 2017/5/17.
//

#include "OrientationEKF.h"

float OrientationEKF::NS2S = 1.0E-9f;
double OrientationEKF::MIN_ACCEL_NOISE_SIGMA = 0.75;
double OrientationEKF::MAX_ACCEL_NOISE_SIGMA = 7.0;