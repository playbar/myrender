//
// Created by houguoli on 2017/5/17.
//

#ifndef MYRENDER_ORIENTATIONEKF_H
#define MYRENDER_ORIENTATIONEKF_H


#include <assert.h>
#include "Matrix3x3d.h"
#include "So3Util.h"

class OrientationEKF {
public:
    static float NS2S;
    static double MIN_ACCEL_NOISE_SIGMA;
    static double MAX_ACCEL_NOISE_SIGMA;
    double rotationMatrix[16];
    Matrix3x3d so3SensorFromWorld;
    Matrix3x3d so3LastMotion;
    Matrix3x3d mP;
    Matrix3x3d mQ;
    Matrix3x3d mR;
    Matrix3x3d mRaccel;
    Matrix3x3d mS;
    Matrix3x3d mH;
    Matrix3x3d mK;
    Vector3dJ mNu;
    Vector3dJ mz;
    Vector3dJ mh;
    Vector3dJ mu;
    Vector3dJ mx;
    Vector3dJ down;
    Vector3dJ north;
    long sensorTimeStampGyro;
    Vector3dJ lastGyro;
    double previousAccelNorm = 0.0;
    double movingAverageAccelNormChange = 0.0;
    float filteredGyroTimestep;
    bool timestepFilterInit = false;
    int numGyroTimestepSamples;
    bool gyroFilterValid = true;
    Matrix3x3d getPredictedGLMatrixTempM1;
    Matrix3x3d getPredictedGLMatrixTempM2;
    Vector3dJ getPredictedGLMatrixTempV1;
    Matrix3x3d setHeadingDegreesTempM1;
    Matrix3x3d processGyroTempM1;
    Matrix3x3d processGyroTempM2;
    Matrix3x3d processAccTempM1;
    Matrix3x3d processAccTempM2;
    Matrix3x3d processAccTempM3;
    Matrix3x3d processAccTempM4;
    Matrix3x3d processAccTempM5;
    Vector3dJ processAccTempV1;
    Vector3dJ processAccTempV2;
    Vector3dJ processAccVDelta;
    Vector3dJ processMagTempV1;
    Vector3dJ processMagTempV2;
    Vector3dJ processMagTempV3;
    Vector3dJ processMagTempV4;
    Vector3dJ processMagTempV5;
    Matrix3x3d processMagTempM1;
    Matrix3x3d processMagTempM2;
    Matrix3x3d processMagTempM4;
    Matrix3x3d processMagTempM5;
    Matrix3x3d processMagTempM6;
    Matrix3x3d updateCovariancesAfterMotionTempM1;
    Matrix3x3d updateCovariancesAfterMotionTempM2;
    Matrix3x3d accObservationFunctionForNumericalJacobianTempM;
    Matrix3x3d magObservationFunctionForNumericalJacobianTempM;
    bool alignedToGravity;
    bool alignedToNorth;

    OrientationEKF()
    {
        reset();
    }

    //同步
    void reset()
    {
        sensorTimeStampGyro = 0;
        so3SensorFromWorld.setIdentity();
        so3LastMotion.setIdentity();
        double initialSigmaP = 5.0;
        mP.setZero();
        mP.setSameDiagonal(25.0);
        double initialSigmaQ = 1.0;
        mQ.setZero();
        mQ.setSameDiagonal(1.0);
        double initialSigmaR = 0.25;
        mR.setZero();
        mR.setSameDiagonal(0.0625);
        mRaccel.setZero();
        mRaccel.setSameDiagonal(0.5625);
        mS.setZero();
        mH.setZero();
        mK.setZero();
        mNu.setZero();
        mz.setZero();
        mh.setZero();
        mu.setZero();
        mx.setZero();
        down.set(0.0, 0.0, 9.81);
        north.set(0.0, 1.0, 0.0);
        alignedToGravity = false;
        alignedToNorth = false;
    }

    //同步
    bool isReady() {
        return alignedToGravity;
    }

    //public synchronized
    double getHeadingDegrees() {
        double y;
        double x = so3SensorFromWorld.get(2, 0);
        double mag = sqrt(x * x + (y = so3SensorFromWorld.get(2, 1)) * y);
        if (mag < 0.1) {
            return 0.0;
        }
        double heading = -90.0 - atan2(y, x) / 3.141592653589793 * 180.0;
        if (heading < 0.0) {
            heading += 360.0;
        }
        if (heading >= 360.0) {
            heading -= 360.0;
        }
        return heading;
    }

    //public synchronized
    void setHeadingDegrees(double heading) {
        double currentHeading = getHeadingDegrees();
        double deltaHeading = heading - currentHeading;
        double s = sin(deltaHeading / 180.0 * 3.141592653589793);
        double c = cos(deltaHeading / 180.0 * 3.141592653589793);
        double deltaHeadingRotationVals[3][3] = {{c, - s, 0.0}, {s, c, 0.0}, {0.0, 0.0, 1.0}};
        OrientationEKF::arrayAssign(deltaHeadingRotationVals, setHeadingDegreesTempM1);
        Matrix3x3d::mult(so3SensorFromWorld, setHeadingDegreesTempM1, so3SensorFromWorld);
    }

    //public synchronized
    double* getGLMatrix() {
        return glMatrixFromSo3(so3SensorFromWorld);
    }

//public synchronized
    double* getPredictedGLMatrix(double secondsAfterLastGyroEvent) {
        double dT = secondsAfterLastGyroEvent;
        Vector3dJ &pmu = getPredictedGLMatrixTempV1;
        pmu.set(lastGyro);
        pmu.scale(- dT);
        Matrix3x3d &so3PredictedMotion = getPredictedGLMatrixTempM1;
        So3Util::sO3FromMu(pmu, so3PredictedMotion);
        Matrix3x3d &so3PredictedState = getPredictedGLMatrixTempM2;
        Matrix3x3d::mult(so3PredictedMotion, so3SensorFromWorld, so3PredictedState);
        return glMatrixFromSo3(so3PredictedState);
    }

//public synchronized
    Matrix3x3d getRotationMatrix() {
        return so3SensorFromWorld;
    }

    static void arrayAssign(double data[3][3], Matrix3x3d &m) {
//        assert (3 == data.length);
//        assert (3 == data[0].length);
//        assert (3 == data[1].length);
//        assert (3 == data[2].length);
        m.set(data[0][0], data[0][1], data[0][2], data[1][0], data[1][1], data[1][2], data[2][0], data[2][1], data[2][2]);
    }

//public synchronized
    bool isAlignedToGravity() {
        return alignedToGravity;
    }

//public synchronized
    bool isAlignedToNorth() {
        return alignedToNorth;
    }

//public synchronized
    void processGyro(Vector3dJ &gyro, long sensorTimeStamp) {
        float kTimeThreshold = 0.04f;
        float kdTdefault = 0.01f;
        if (sensorTimeStampGyro != 0) {
            float dT = (float)(sensorTimeStamp - sensorTimeStampGyro) * 1.0E-9f;
            if (dT > 0.04f) {
                dT = gyroFilterValid ? filteredGyroTimestep : 0.01f;
            } else {
                filterGyroTimestep(dT);
            }
            mu.set(gyro);
            mu.scale(- dT);
            So3Util::sO3FromMu(mu, so3LastMotion);
            processGyroTempM1.set(so3SensorFromWorld);
            Matrix3x3d::mult(so3LastMotion, so3SensorFromWorld, processGyroTempM1);
            so3SensorFromWorld.set(processGyroTempM1);
            updateCovariancesAfterMotion();
            processGyroTempM2.set(mQ);
            processGyroTempM2.scale(dT * dT);
            mP.plusEquals(processGyroTempM2);
        }
        sensorTimeStampGyro = sensorTimeStamp;
        lastGyro.set(gyro);
    }

    void updateAccelCovariance(double currentAccelNorm) {
        double currentAccelNormChange = abs(currentAccelNorm - previousAccelNorm);
        previousAccelNorm = currentAccelNorm;
        double kSmoothingFactor = 0.5;
        movingAverageAccelNormChange = 0.5 * currentAccelNormChange + 0.5 * movingAverageAccelNormChange;
        double kMaxAccelNormChange = 0.15;
        double normChangeRatio = movingAverageAccelNormChange / 0.15;
        double accelNoiseSigma = std::min(7.0, 0.75 + normChangeRatio * 6.25);
        mRaccel.setSameDiagonal(accelNoiseSigma * accelNoiseSigma);
    }

//public synchronized
    void processAcc(Vector3dJ &acc, long sensorTimeStamp) {
        mz.set(acc);
        updateAccelCovariance(mz.length());
        if (alignedToGravity) {
            accObservationFunctionForNumericalJacobian(so3SensorFromWorld, mNu);
            double eps = 1.0E-7;
            for (int dof = 0; dof < 3; ++dof) {
                Vector3dJ &delta = processAccVDelta;
                delta.setZero();
                delta.setComponent(dof, eps);
                So3Util::sO3FromMu(delta, processAccTempM1);
                Matrix3x3d::mult(processAccTempM1, so3SensorFromWorld, processAccTempM2);
                accObservationFunctionForNumericalJacobian(processAccTempM2, processAccTempV1);
                Vector3dJ &withDelta = processAccTempV1;
                Vector3dJ::sub(mNu, withDelta, processAccTempV2);
                processAccTempV2.scale(1.0 / eps);
                mH.setColumn(dof, processAccTempV2);
            }
            mH.transpose(processAccTempM3);
            Matrix3x3d::mult(mP, processAccTempM3, processAccTempM4);
            Matrix3x3d::mult(mH, processAccTempM4, processAccTempM5);
            Matrix3x3d::add(processAccTempM5, mRaccel, mS);
            mS.invert(processAccTempM3);
            mH.transpose(processAccTempM4);
            Matrix3x3d::mult(processAccTempM4, processAccTempM3, processAccTempM5);
            Matrix3x3d::mult(mP, processAccTempM5, mK);
            Matrix3x3d::mult(mK, mNu, mx);
            Matrix3x3d::mult(mK, mH, processAccTempM3);
            processAccTempM4.setIdentity();
            processAccTempM4.minusEquals(processAccTempM3);
            Matrix3x3d::mult(processAccTempM4, mP, processAccTempM3);
            mP.set(processAccTempM3);
            So3Util::sO3FromMu(mx, so3LastMotion);
            Matrix3x3d::mult(so3LastMotion, so3SensorFromWorld, so3SensorFromWorld);
            updateCovariancesAfterMotion();
        } else {
            So3Util::sO3FromTwoVec(down, mz, so3SensorFromWorld);
            alignedToGravity = true;
        }
    }

//public synchronized
    void processMag(float *mag, long sensorTimeStamp) {
        if (!alignedToGravity) {
            return;
        }
        mz.set(mag[0], mag[1], mag[2]);
        mz.normalize();
        Vector3dJ downInSensorFrame;
        so3SensorFromWorld.getColumn(2, downInSensorFrame);
        Vector3dJ::cross(mz, downInSensorFrame, processMagTempV1);
        Vector3dJ &perpToDownAndMag = processMagTempV1;
        perpToDownAndMag.normalize();
        Vector3dJ::cross(downInSensorFrame, perpToDownAndMag, processMagTempV2);
        Vector3dJ &magHorizontal = processMagTempV2;
        magHorizontal.normalize();
        mz.set(magHorizontal);
        if (alignedToNorth) {
            magObservationFunctionForNumericalJacobian(so3SensorFromWorld, mNu);
            double eps = 1.0E-7;
            for (int dof = 0; dof < 3; ++dof) {
                Vector3dJ &delta = processMagTempV3;
                delta.setZero();
                delta.setComponent(dof, eps);
                So3Util::sO3FromMu(delta, processMagTempM1);
                Matrix3x3d::mult(processMagTempM1, so3SensorFromWorld, processMagTempM2);
                magObservationFunctionForNumericalJacobian(processMagTempM2, processMagTempV4);
                Vector3dJ &withDelta = processMagTempV4;
                Vector3dJ::sub(mNu, withDelta, processMagTempV5);
                processMagTempV5.scale(1.0 / eps);
                mH.setColumn(dof, processMagTempV5);
            }
            mH.transpose(processMagTempM4);
            Matrix3x3d::mult(mP, processMagTempM4, processMagTempM5);
            Matrix3x3d::mult(mH, processMagTempM5, processMagTempM6);
            Matrix3x3d::add(processMagTempM6, mR, mS);
            mS.invert(processMagTempM4);
            mH.transpose(processMagTempM5);
            Matrix3x3d::mult(processMagTempM5, processMagTempM4, processMagTempM6);
            Matrix3x3d::mult(mP, processMagTempM6, mK);
            Matrix3x3d::mult(mK, mNu, mx);
            Matrix3x3d::mult(mK, mH, processMagTempM4);
            processMagTempM5.setIdentity();
            processMagTempM5.minusEquals(processMagTempM4);
            Matrix3x3d::mult(processMagTempM5, mP, processMagTempM4);
            mP.set(processMagTempM4);
            So3Util::sO3FromMu(mx, so3LastMotion);
            Matrix3x3d::mult(so3LastMotion, so3SensorFromWorld, processMagTempM4);
            so3SensorFromWorld.set(processMagTempM4);
            updateCovariancesAfterMotion();
        } else {
            magObservationFunctionForNumericalJacobian(so3SensorFromWorld, mNu);
            So3Util::sO3FromMu(mNu, so3LastMotion);
            Matrix3x3d::mult(so3LastMotion, so3SensorFromWorld, processMagTempM4);
            so3SensorFromWorld.set(processMagTempM4);
            updateCovariancesAfterMotion();
            alignedToNorth = true;
        }
    }

    double* glMatrixFromSo3(Matrix3x3d &so3) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                rotationMatrix[4 * c + r] = so3.get(r, c);
            }
        }
        rotationMatrix[11] = 0.0;
        rotationMatrix[7] = 0.0;
        rotationMatrix[3] = 0.0;
        rotationMatrix[14] = 0.0;
        rotationMatrix[13] = 0.0;
        rotationMatrix[12] = 0.0;
        rotationMatrix[15] = 1.0;
        return rotationMatrix;
    }

    void filterGyroTimestep(float timeStep) {
        float kFilterCoeff = 0.95f;
        float kMinSamples = 10.0f;
        if (!timestepFilterInit) {
            filteredGyroTimestep = timeStep;
            numGyroTimestepSamples = 1;
            timestepFilterInit = true;
        } else {
            filteredGyroTimestep = 0.95f * filteredGyroTimestep + 0.050000012f * timeStep;
            if ((float)(++numGyroTimestepSamples) > 10.0f) {
                gyroFilterValid = true;
            }
        }
    }

    void updateCovariancesAfterMotion() {
        so3LastMotion.transpose(updateCovariancesAfterMotionTempM1);
        Matrix3x3d::mult(mP, updateCovariancesAfterMotionTempM1, updateCovariancesAfterMotionTempM2);
        Matrix3x3d::mult(so3LastMotion, updateCovariancesAfterMotionTempM2, mP);
        so3LastMotion.setIdentity();
    }

    void accObservationFunctionForNumericalJacobian(Matrix3x3d &so3SensorFromWorldPred, Vector3dJ &result) {
        Matrix3x3d::mult(so3SensorFromWorldPred, down, mh);
        So3Util::sO3FromTwoVec(mh, mz, accObservationFunctionForNumericalJacobianTempM);
        So3Util::muFromSO3(accObservationFunctionForNumericalJacobianTempM, result);
    }

    void magObservationFunctionForNumericalJacobian(Matrix3x3d &so3SensorFromWorldPred, Vector3dJ &result) {
        Matrix3x3d::mult(so3SensorFromWorldPred, north, mh);
        So3Util::sO3FromTwoVec(mh, mz, magObservationFunctionForNumericalJacobianTempM);
        So3Util::muFromSO3(magObservationFunctionForNumericalJacobianTempM, result);
    }
};


#endif //MYRENDER_ORIENTATIONEKF_H
