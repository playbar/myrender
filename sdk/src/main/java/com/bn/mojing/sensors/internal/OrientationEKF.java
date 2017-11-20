/*
 * Decompiled with CFR 0_119.
 */
package com.bn.mojing.sensors.internal;

public class OrientationEKF {
    private static final float NS2S = 1.0E-9f;
    private static final double MIN_ACCEL_NOISE_SIGMA = 0.75;
    private static final double MAX_ACCEL_NOISE_SIGMA = 7.0;
    private double[] rotationMatrix = new double[16];
    private Matrix3x3d so3SensorFromWorld = new Matrix3x3d();
    private Matrix3x3d so3LastMotion = new Matrix3x3d();
    private Matrix3x3d mP = new Matrix3x3d();
    private Matrix3x3d mQ = new Matrix3x3d();
    private Matrix3x3d mR = new Matrix3x3d();
    private Matrix3x3d mRaccel = new Matrix3x3d();
    private Matrix3x3d mS = new Matrix3x3d();
    private Matrix3x3d mH = new Matrix3x3d();
    private Matrix3x3d mK = new Matrix3x3d();
    private Vector3d mNu = new Vector3d();
    private Vector3d mz = new Vector3d();
    private Vector3d mh = new Vector3d();
    private Vector3d mu = new Vector3d();
    private Vector3d mx = new Vector3d();
    private Vector3d down = new Vector3d();
    private Vector3d north = new Vector3d();
    private long sensorTimeStampGyro;
    private final Vector3d lastGyro = new Vector3d();
    private double previousAccelNorm = 0.0;
    private double movingAverageAccelNormChange = 0.0;
    private float filteredGyroTimestep;
    private boolean timestepFilterInit = false;
    private int numGyroTimestepSamples;
    private boolean gyroFilterValid = true;
    private Matrix3x3d getPredictedGLMatrixTempM1 = new Matrix3x3d();
    private Matrix3x3d getPredictedGLMatrixTempM2 = new Matrix3x3d();
    private Vector3d getPredictedGLMatrixTempV1 = new Vector3d();
    private Matrix3x3d setHeadingDegreesTempM1 = new Matrix3x3d();
    private Matrix3x3d processGyroTempM1 = new Matrix3x3d();
    private Matrix3x3d processGyroTempM2 = new Matrix3x3d();
    private Matrix3x3d processAccTempM1 = new Matrix3x3d();
    private Matrix3x3d processAccTempM2 = new Matrix3x3d();
    private Matrix3x3d processAccTempM3 = new Matrix3x3d();
    private Matrix3x3d processAccTempM4 = new Matrix3x3d();
    private Matrix3x3d processAccTempM5 = new Matrix3x3d();
    private Vector3d processAccTempV1 = new Vector3d();
    private Vector3d processAccTempV2 = new Vector3d();
    private Vector3d processAccVDelta = new Vector3d();
    private Vector3d processMagTempV1 = new Vector3d();
    private Vector3d processMagTempV2 = new Vector3d();
    private Vector3d processMagTempV3 = new Vector3d();
    private Vector3d processMagTempV4 = new Vector3d();
    private Vector3d processMagTempV5 = new Vector3d();
    private Matrix3x3d processMagTempM1 = new Matrix3x3d();
    private Matrix3x3d processMagTempM2 = new Matrix3x3d();
    private Matrix3x3d processMagTempM4 = new Matrix3x3d();
    private Matrix3x3d processMagTempM5 = new Matrix3x3d();
    private Matrix3x3d processMagTempM6 = new Matrix3x3d();
    private Matrix3x3d updateCovariancesAfterMotionTempM1 = new Matrix3x3d();
    private Matrix3x3d updateCovariancesAfterMotionTempM2 = new Matrix3x3d();
    private Matrix3x3d accObservationFunctionForNumericalJacobianTempM = new Matrix3x3d();
    private Matrix3x3d magObservationFunctionForNumericalJacobianTempM = new Matrix3x3d();
    private boolean alignedToGravity;
    private boolean alignedToNorth;

    public OrientationEKF() {
        this.reset();
    }

    public synchronized void reset() {
        this.sensorTimeStampGyro = 0;
        this.so3SensorFromWorld.setIdentity();
        this.so3LastMotion.setIdentity();
        double initialSigmaP = 5.0;
        this.mP.setZero();
        this.mP.setSameDiagonal(25.0);
        double initialSigmaQ = 1.0;
        this.mQ.setZero();
        this.mQ.setSameDiagonal(1.0);
        double initialSigmaR = 0.25;
        this.mR.setZero();
        this.mR.setSameDiagonal(0.0625);
        this.mRaccel.setZero();
        this.mRaccel.setSameDiagonal(0.5625);
        this.mS.setZero();
        this.mH.setZero();
        this.mK.setZero();
        this.mNu.setZero();
        this.mz.setZero();
        this.mh.setZero();
        this.mu.setZero();
        this.mx.setZero();
        this.down.set(0.0, 0.0, 9.81);
        this.north.set(0.0, 1.0, 0.0);
        this.alignedToGravity = false;
        this.alignedToNorth = false;
    }

    public synchronized boolean isReady() {
        return this.alignedToGravity;
    }

    public synchronized double getHeadingDegrees() {
        double y;
        double x = this.so3SensorFromWorld.get(2, 0);
        double mag = Math.sqrt(x * x + (y = this.so3SensorFromWorld.get(2, 1)) * y);
        if (mag < 0.1) {
            return 0.0;
        }
        double heading = -90.0 - Math.atan2(y, x) / 3.141592653589793 * 180.0;
        if (heading < 0.0) {
            heading += 360.0;
        }
        if (heading >= 360.0) {
            heading -= 360.0;
        }
        return heading;
    }

    public synchronized void setHeadingDegrees(double heading) {
        double currentHeading = this.getHeadingDegrees();
        double deltaHeading = heading - currentHeading;
        double s = Math.sin(deltaHeading / 180.0 * 3.141592653589793);
        double c = Math.cos(deltaHeading / 180.0 * 3.141592653589793);
        double[][] deltaHeadingRotationVals = new double[][]{{c, - s, 0.0}, {s, c, 0.0}, {0.0, 0.0, 1.0}};
        OrientationEKF.arrayAssign(deltaHeadingRotationVals, this.setHeadingDegreesTempM1);
        Matrix3x3d.mult(this.so3SensorFromWorld, this.setHeadingDegreesTempM1, this.so3SensorFromWorld);
    }

    public synchronized double[] getGLMatrix() {
        return this.glMatrixFromSo3(this.so3SensorFromWorld);
    }

    public synchronized double[] getPredictedGLMatrix(double secondsAfterLastGyroEvent) {
        double dT = secondsAfterLastGyroEvent;
        Vector3d pmu = this.getPredictedGLMatrixTempV1;
        pmu.set(this.lastGyro);
        pmu.scale(- dT);
        Matrix3x3d so3PredictedMotion = this.getPredictedGLMatrixTempM1;
        So3Util.sO3FromMu(pmu, so3PredictedMotion);
        Matrix3x3d so3PredictedState = this.getPredictedGLMatrixTempM2;
        Matrix3x3d.mult(so3PredictedMotion, this.so3SensorFromWorld, so3PredictedState);
        return this.glMatrixFromSo3(so3PredictedState);
    }

    public synchronized Matrix3x3d getRotationMatrix() {
        return this.so3SensorFromWorld;
    }

    public static void arrayAssign(double[][] data, Matrix3x3d m) {
        assert (3 == data.length);
        assert (3 == data[0].length);
        assert (3 == data[1].length);
        assert (3 == data[2].length);
        m.set(data[0][0], data[0][1], data[0][2], data[1][0], data[1][1], data[1][2], data[2][0], data[2][1], data[2][2]);
    }

    public synchronized boolean isAlignedToGravity() {
        return this.alignedToGravity;
    }

    public synchronized boolean isAlignedToNorth() {
        return this.alignedToNorth;
    }

    public synchronized void processGyro(Vector3d gyro, long sensorTimeStamp) {
        float kTimeThreshold = 0.04f;
        float kdTdefault = 0.01f;
        if (this.sensorTimeStampGyro != 0) {
            float dT = (float)(sensorTimeStamp - this.sensorTimeStampGyro) * 1.0E-9f;
            if (dT > 0.04f) {
                dT = this.gyroFilterValid ? this.filteredGyroTimestep : 0.01f;
            } else {
                this.filterGyroTimestep(dT);
            }
            this.mu.set(gyro);
            this.mu.scale(- dT);
            So3Util.sO3FromMu(this.mu, this.so3LastMotion);
            this.processGyroTempM1.set(this.so3SensorFromWorld);
            Matrix3x3d.mult(this.so3LastMotion, this.so3SensorFromWorld, this.processGyroTempM1);
            this.so3SensorFromWorld.set(this.processGyroTempM1);
            this.updateCovariancesAfterMotion();
            this.processGyroTempM2.set(this.mQ);
            this.processGyroTempM2.scale(dT * dT);
            this.mP.plusEquals(this.processGyroTempM2);
        }
        this.sensorTimeStampGyro = sensorTimeStamp;
        this.lastGyro.set(gyro);
    }

    private void updateAccelCovariance(double currentAccelNorm) {
        double currentAccelNormChange = Math.abs(currentAccelNorm - this.previousAccelNorm);
        this.previousAccelNorm = currentAccelNorm;
        double kSmoothingFactor = 0.5;
        this.movingAverageAccelNormChange = 0.5 * currentAccelNormChange + 0.5 * this.movingAverageAccelNormChange;
        double kMaxAccelNormChange = 0.15;
        double normChangeRatio = this.movingAverageAccelNormChange / 0.15;
        double accelNoiseSigma = Math.min(7.0, 0.75 + normChangeRatio * 6.25);
        this.mRaccel.setSameDiagonal(accelNoiseSigma * accelNoiseSigma);
    }

    public synchronized void processAcc(Vector3d acc, long sensorTimeStamp) {
        this.mz.set(acc);
        this.updateAccelCovariance(this.mz.length());
        if (this.alignedToGravity) {
            this.accObservationFunctionForNumericalJacobian(this.so3SensorFromWorld, this.mNu);
            double eps = 1.0E-7;
            for (int dof = 0; dof < 3; ++dof) {
                Vector3d delta = this.processAccVDelta;
                delta.setZero();
                delta.setComponent(dof, eps);
                So3Util.sO3FromMu(delta, this.processAccTempM1);
                Matrix3x3d.mult(this.processAccTempM1, this.so3SensorFromWorld, this.processAccTempM2);
                this.accObservationFunctionForNumericalJacobian(this.processAccTempM2, this.processAccTempV1);
                Vector3d withDelta = this.processAccTempV1;
                Vector3d.sub(this.mNu, withDelta, this.processAccTempV2);
                this.processAccTempV2.scale(1.0 / eps);
                this.mH.setColumn(dof, this.processAccTempV2);
            }
            this.mH.transpose(this.processAccTempM3);
            Matrix3x3d.mult(this.mP, this.processAccTempM3, this.processAccTempM4);
            Matrix3x3d.mult(this.mH, this.processAccTempM4, this.processAccTempM5);
            Matrix3x3d.add(this.processAccTempM5, this.mRaccel, this.mS);
            this.mS.invert(this.processAccTempM3);
            this.mH.transpose(this.processAccTempM4);
            Matrix3x3d.mult(this.processAccTempM4, this.processAccTempM3, this.processAccTempM5);
            Matrix3x3d.mult(this.mP, this.processAccTempM5, this.mK);
            Matrix3x3d.mult(this.mK, this.mNu, this.mx);
            Matrix3x3d.mult(this.mK, this.mH, this.processAccTempM3);
            this.processAccTempM4.setIdentity();
            this.processAccTempM4.minusEquals(this.processAccTempM3);
            Matrix3x3d.mult(this.processAccTempM4, this.mP, this.processAccTempM3);
            this.mP.set(this.processAccTempM3);
            So3Util.sO3FromMu(this.mx, this.so3LastMotion);
            Matrix3x3d.mult(this.so3LastMotion, this.so3SensorFromWorld, this.so3SensorFromWorld);
            this.updateCovariancesAfterMotion();
        } else {
            So3Util.sO3FromTwoVec(this.down, this.mz, this.so3SensorFromWorld);
            this.alignedToGravity = true;
        }
    }

    public synchronized void processMag(float[] mag, long sensorTimeStamp) {
        if (!this.alignedToGravity) {
            return;
        }
        this.mz.set(mag[0], mag[1], mag[2]);
        this.mz.normalize();
        Vector3d downInSensorFrame = new Vector3d();
        this.so3SensorFromWorld.getColumn(2, downInSensorFrame);
        Vector3d.cross(this.mz, downInSensorFrame, this.processMagTempV1);
        Vector3d perpToDownAndMag = this.processMagTempV1;
        perpToDownAndMag.normalize();
        Vector3d.cross(downInSensorFrame, perpToDownAndMag, this.processMagTempV2);
        Vector3d magHorizontal = this.processMagTempV2;
        magHorizontal.normalize();
        this.mz.set(magHorizontal);
        if (this.alignedToNorth) {
            this.magObservationFunctionForNumericalJacobian(this.so3SensorFromWorld, this.mNu);
            double eps = 1.0E-7;
            for (int dof = 0; dof < 3; ++dof) {
                Vector3d delta = this.processMagTempV3;
                delta.setZero();
                delta.setComponent(dof, eps);
                So3Util.sO3FromMu(delta, this.processMagTempM1);
                Matrix3x3d.mult(this.processMagTempM1, this.so3SensorFromWorld, this.processMagTempM2);
                this.magObservationFunctionForNumericalJacobian(this.processMagTempM2, this.processMagTempV4);
                Vector3d withDelta = this.processMagTempV4;
                Vector3d.sub(this.mNu, withDelta, this.processMagTempV5);
                this.processMagTempV5.scale(1.0 / eps);
                this.mH.setColumn(dof, this.processMagTempV5);
            }
            this.mH.transpose(this.processMagTempM4);
            Matrix3x3d.mult(this.mP, this.processMagTempM4, this.processMagTempM5);
            Matrix3x3d.mult(this.mH, this.processMagTempM5, this.processMagTempM6);
            Matrix3x3d.add(this.processMagTempM6, this.mR, this.mS);
            this.mS.invert(this.processMagTempM4);
            this.mH.transpose(this.processMagTempM5);
            Matrix3x3d.mult(this.processMagTempM5, this.processMagTempM4, this.processMagTempM6);
            Matrix3x3d.mult(this.mP, this.processMagTempM6, this.mK);
            Matrix3x3d.mult(this.mK, this.mNu, this.mx);
            Matrix3x3d.mult(this.mK, this.mH, this.processMagTempM4);
            this.processMagTempM5.setIdentity();
            this.processMagTempM5.minusEquals(this.processMagTempM4);
            Matrix3x3d.mult(this.processMagTempM5, this.mP, this.processMagTempM4);
            this.mP.set(this.processMagTempM4);
            So3Util.sO3FromMu(this.mx, this.so3LastMotion);
            Matrix3x3d.mult(this.so3LastMotion, this.so3SensorFromWorld, this.processMagTempM4);
            this.so3SensorFromWorld.set(this.processMagTempM4);
            this.updateCovariancesAfterMotion();
        } else {
            this.magObservationFunctionForNumericalJacobian(this.so3SensorFromWorld, this.mNu);
            So3Util.sO3FromMu(this.mNu, this.so3LastMotion);
            Matrix3x3d.mult(this.so3LastMotion, this.so3SensorFromWorld, this.processMagTempM4);
            this.so3SensorFromWorld.set(this.processMagTempM4);
            this.updateCovariancesAfterMotion();
            this.alignedToNorth = true;
        }
    }

    private double[] glMatrixFromSo3(Matrix3x3d so3) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                this.rotationMatrix[4 * c + r] = so3.get(r, c);
            }
        }
        this.rotationMatrix[11] = 0.0;
        this.rotationMatrix[7] = 0.0;
        this.rotationMatrix[3] = 0.0;
        this.rotationMatrix[14] = 0.0;
        this.rotationMatrix[13] = 0.0;
        this.rotationMatrix[12] = 0.0;
        this.rotationMatrix[15] = 1.0;
        return this.rotationMatrix;
    }

    private void filterGyroTimestep(float timeStep) {
        float kFilterCoeff = 0.95f;
        float kMinSamples = 10.0f;
        if (!this.timestepFilterInit) {
            this.filteredGyroTimestep = timeStep;
            this.numGyroTimestepSamples = 1;
            this.timestepFilterInit = true;
        } else {
            this.filteredGyroTimestep = 0.95f * this.filteredGyroTimestep + 0.050000012f * timeStep;
            if ((float)(++this.numGyroTimestepSamples) > 10.0f) {
                this.gyroFilterValid = true;
            }
        }
    }

    private void updateCovariancesAfterMotion() {
        this.so3LastMotion.transpose(this.updateCovariancesAfterMotionTempM1);
        Matrix3x3d.mult(this.mP, this.updateCovariancesAfterMotionTempM1, this.updateCovariancesAfterMotionTempM2);
        Matrix3x3d.mult(this.so3LastMotion, this.updateCovariancesAfterMotionTempM2, this.mP);
        this.so3LastMotion.setIdentity();
    }

    private void accObservationFunctionForNumericalJacobian(Matrix3x3d so3SensorFromWorldPred, Vector3d result) {
        Matrix3x3d.mult(so3SensorFromWorldPred, this.down, this.mh);
        So3Util.sO3FromTwoVec(this.mh, this.mz, this.accObservationFunctionForNumericalJacobianTempM);
        So3Util.muFromSO3(this.accObservationFunctionForNumericalJacobianTempM, result);
    }

    private void magObservationFunctionForNumericalJacobian(Matrix3x3d so3SensorFromWorldPred, Vector3d result) {
        Matrix3x3d.mult(so3SensorFromWorldPred, this.north, this.mh);
        So3Util.sO3FromTwoVec(this.mh, this.mz, this.magObservationFunctionForNumericalJacobianTempM);
        So3Util.muFromSO3(this.magObservationFunctionForNumericalJacobianTempM, result);
    }
}
