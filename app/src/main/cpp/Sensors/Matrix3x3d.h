//
// Created by houguoli on 2017/5/17.
//

#ifndef MYRENDER_MATRIX3X3D_H
#define MYRENDER_MATRIX3X3D_H


#include <cstring>
#include <cstdlib>
#include <algorithm>

class Matrix3x3d {
public:
    double m[9];

    Matrix3x3d() {
        memset(m, 0, sizeof(double) * 9);
    }


    Matrix3x3d(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22)
    {
        m[0] = m00;
        m[1] = m01;
        m[2] = m02;
        m[3] = m10;
        m[4] = m11;
        m[5] = m12;
        m[6] = m20;
        m[7] = m21;
        m[8] = m22;
    }

    Matrix3x3d(Matrix3x3d o) {
        m[0] = o.m[0];
        m[1] = o.m[1];
        m[2] = o.m[2];
        m[3] = o.m[3];
        m[4] = o.m[4];
        m[5] = o.m[5];
        m[6] = o.m[6];
        m[7] = o.m[7];
        m[8] = o.m[8];
    }


    void set(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22) {
        m[0] = m00;
        m[1] = m01;
        m[2] = m02;
        m[3] = m10;
        m[4] = m11;
        m[5] = m12;
        m[6] = m20;
        m[7] = m21;
        m[8] = m22;
    }

    void set(Matrix3x3d o) {
        m[0] = o.m[0];
        m[1] = o.m[1];
        m[2] = o.m[2];
        m[3] = o.m[3];
        m[4] = o.m[4];
        m[5] = o.m[5];
        m[6] = o.m[6];
        m[7] = o.m[7];
        m[8] = o.m[8];
    }

    void setZero() {
        m[8] = 0.0;
        m[7] = 0.0;
        m[6] = 0.0;
        m[5] = 0.0;
        m[4] = 0.0;
        m[3] = 0.0;
        m[2] = 0.0;
        m[1] = 0.0;
        m[0] = 0.0;
    }

    void setIdentity() {
        m[7] = 0.0;
        m[6] = 0.0;
        m[5] = 0.0;
        m[3] = 0.0;
        m[2] = 0.0;
        m[1] = 0.0;
        m[8] = 1.0;
        m[4] = 1.0;
        m[0] = 1.0;
    }

    void setSameDiagonal(double d) {
        m[4] = m[8] = d;
        m[0] = m[8];
    }

    double get(int row, int col) {
        return m[3 * row + col];
    }

    void set(int row, int col, double value) {
        m[3 * row + col] = value;
    }

    void getColumn(int col, Vector3d v) {
        v.x = m[col];
        v.y = m[col + 3];
        v.z = m[col + 6];
    }

    void setColumn(int col, Vector3d v) {
        m[col] = v.x;
        m[col + 3] = v.y;
        m[col + 6] = v.z;
    }

    void scale(double s) {
        double *arrd = m;
        arrd[0] = arrd[0] * s;
        double *arrd2 = m;
        arrd2[1] = arrd2[1] * s;
        double *arrd3 = m;
        arrd3[2] = arrd3[2] * s;
        double *arrd4 = m;
        arrd4[3] = arrd4[3] * s;
        double *arrd5 = m;
        arrd5[4] = arrd5[4] * s;
        double *arrd6 = m;
        arrd6[5] = arrd6[5] * s;
        double *arrd7 = m;
        arrd7[6] = arrd7[6] * s;
        double *arrd8 = m;
        arrd8[7] = arrd8[7] * s;
        double *arrd9 = m;
        arrd9[8] = arrd9[8] * s;
    }


    void plusEquals(Matrix3x3d b) {
        double *arrd = m;
        arrd[0] = arrd[0] + b.m[0];
        double *arrd2 = m;
        arrd2[1] = arrd2[1] + b.m[1];
        double *arrd3 = m;
        arrd3[2] = arrd3[2] + b.m[2];
        double *arrd4 = m;
        arrd4[3] = arrd4[3] + b.m[3];
        double *arrd5 = m;
        arrd5[4] = arrd5[4] + b.m[4];
        double *arrd6 = m;
        arrd6[5] = arrd6[5] + b.m[5];
        double *arrd7 = m;
        arrd7[6] = arrd7[6] + b.m[6];
        double *arrd8 = m;
        arrd8[7] = arrd8[7] + b.m[7];
        double *arrd9 = m;
        arrd9[8] = arrd9[8] + b.m[8];
    }


    void minusEquals(Matrix3x3d b) {
        double *arrd = m;
        arrd[0] = arrd[0] - b.m[0];
        double *arrd2 = m;
        arrd2[1] = arrd2[1] - b.m[1];
        double *arrd3 = m;
        arrd3[2] = arrd3[2] - b.m[2];
        double *arrd4 = m;
        arrd4[3] = arrd4[3] - b.m[3];
        double *arrd5 = m;
        arrd5[4] = arrd5[4] - b.m[4];
        double *arrd6 = m;
        arrd6[5] = arrd6[5] - b.m[5];
        double *arrd7 = m;
        arrd7[6] = arrd7[6] - b.m[6];
        double *arrd8 = m;
        arrd8[7] = arrd8[7] - b.m[7];
        double *arrd9 = m;
        arrd9[8] = arrd9[8] - b.m[8];
    }

    void transpose() {
        double tmp = m[1];
        m[1] = m[3];
        m[3] = tmp;
        tmp = m[2];
        m[2] = m[6];
        m[6] = tmp;
        tmp = m[5];
        m[5] = m[7];
        m[7] = tmp;
    }

    void transpose(Matrix3x3d result) {
        double m1 = m[1];
        double m2 = m[2];
        double m5 = m[5];
        result.m[0] = m[0];
        result.m[1] = m[3];
        result.m[2] = m[6];
        result.m[3] = m1;
        result.m[4] = m[4];
        result.m[5] = m[7];
        result.m[6] = m2;
        result.m[7] = m5;
        result.m[8] = m[8];
    }

    static void add(Matrix3x3d a, Matrix3x3d b, Matrix3x3d result) {
        result.m[0] = a.m[0] + b.m[0];
        result.m[1] = a.m[1] + b.m[1];
        result.m[2] = a.m[2] + b.m[2];
        result.m[3] = a.m[3] + b.m[3];
        result.m[4] = a.m[4] + b.m[4];
        result.m[5] = a.m[5] + b.m[5];
        result.m[6] = a.m[6] + b.m[6];
        result.m[7] = a.m[7] + b.m[7];
        result.m[8] = a.m[8] + b.m[8];
    }

    static void mult(Matrix3x3d a, Matrix3x3d b, Matrix3x3d result) {
        result.set(a.m[0] * b.m[0] + a.m[1] * b.m[3] + a.m[2] * b.m[6], a.m[0] * b.m[1] + a.m[1] * b.m[4] + a.m[2] * b.m[7], a.m[0] * b.m[2] + a.m[1] * b.m[5] + a.m[2] * b.m[8], a.m[3] * b.m[0] + a.m[4] * b.m[3] + a.m[5] * b.m[6], a.m[3] * b.m[1] + a.m[4] * b.m[4] + a.m[5] * b.m[7], a.m[3] * b.m[2] + a.m[4] * b.m[5] + a.m[5] * b.m[8], a.m[6] * b.m[0] + a.m[7] * b.m[3] + a.m[8] * b.m[6], a.m[6] * b.m[1] + a.m[7] * b.m[4] + a.m[8] * b.m[7], a.m[6] * b.m[2] + a.m[7] * b.m[5] + a.m[8] * b.m[8]);
    }

    static void mult(Matrix3x3d a, Vector3d v, Vector3d result) {
        double x = a.m[0] * v.x + a.m[1] * v.y + a.m[2] * v.z;
        double y = a.m[3] * v.x + a.m[4] * v.y + a.m[5] * v.z;
        double z = a.m[6] * v.x + a.m[7] * v.y + a.m[8] * v.z;
        result.x = x;
        result.y = y;
        result.z = z;
    }

    double determinant() {
        return get(0, 0) * (get(1, 1) * get(2, 2) - get(2, 1) * get(1, 2)) - get(0, 1) * (get(1, 0) * get(2, 2) - get(1, 2) * get(2, 0)) + get(0, 2) * (get(1, 0) * get(2, 1) - get(1, 1) * get(2, 0));
    }

    bool invert(Matrix3x3d result) {
        double d = determinant();
        if (d == 0.0) {
            return false;
        }
        double invdet = 1.0 / d;
        result.set((m[4] * m[8] - m[7] * m[5]) * invdet, (- m[1] * m[8] - m[2] * m[7]) * invdet, (m[1] * m[5] - m[2] * m[4]) * invdet, (- m[3] * m[8] - m[5] * m[6]) * invdet, (m[0] * m[8] - m[2] * m[6]) * invdet, (- m[0] * m[5] - m[3] * m[2]) * invdet, (m[3] * m[7] - m[6] * m[4]) * invdet, (- m[0] * m[7] - m[6] * m[1]) * invdet, (m[0] * m[4] - m[3] * m[1]) * invdet);
        return true;
    }

    double maxNorm() {
        double maxVal = abs(m[0]);
        int len = sizeof(m) / sizeof(m[0]);
        for (int i = 1; i < len; ++i) {
            maxVal = std::max(maxVal, abs(m[i]));
        }
        return maxVal;
    }
};


#endif //MYRENDER_MATRIX3X3D_H
