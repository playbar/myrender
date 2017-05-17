//
// Created by houguoli on 2017/5/17.
//

#ifndef MYRENDER_VECTOR3D_H
#define MYRENDER_VECTOR3D_H


#include <cmath>
#include <string>
#include <cstdlib>

class Vector3d
{
double x;
double y;
double z;

Vector3d() {
    x = 0;
    y = 0;
    z = 0;
    }

 Vector3d(double xx, double yy, double zz) {
        set(xx, yy, zz);
    }

void set(double xx, double yy, double zz) {
        x = xx;
        y = yy;
        z = zz;
    }

void setComponent(int i, double val) {
        if (i == 0) {
            x = val;
        } else if (i == 1) {
            y = val;
        } else {
            z = val;
        }
    }

void setZero() {
        z = 0.0;
        y = 0.0;
        x = 0.0;
    }

void set(Vector3d other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }

void scale(double s) {
        x *= s;
        y *= s;
        z *= s;
    }

void normalize() {
        double d = length();
        if (d != 0.0) {
            scale(1.0 / d);
        }
    }

static double dot(Vector3d a, Vector3d b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

double length() {
        return sqrt(x * x + y * y + z * z);
    }

bool sameValues(Vector3d other) {
        return x == other.x && y == other.y && z == other.z;
    }

static void add(Vector3d a, Vector3d b, Vector3d result) {
        result.set(a.x + b.x, a.y + b.y, a.z + b.z);
    }

static void sub(Vector3d a, Vector3d b, Vector3d result) {
        result.set(a.x - b.x, a.y - b.y, a.z - b.z);
    }

static void cross(Vector3d a, Vector3d b, Vector3d result) {
        result.set(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

static void ortho(Vector3d v, Vector3d result) {
        int k = Vector3d::largestAbsComponent(v) - 1;
        if (k < 0) {
            k = 2;
        }
        result.setZero();
        result.setComponent(k, 1.0);
        Vector3d::cross(v, result, result);
        result.normalize();
    }

std::string toString() {
    char szTmp[64] = {0};
    printf(szTmp, "%+05f %+05f %+05f", x, y, z);
    return std::string(szTmp);
    }

static int largestAbsComponent(Vector3d v) {
        double xAbs = abs(v.x);
        double yAbs = abs(v.y);
        double zAbs = abs(v.z);
        if (xAbs > yAbs) {
            if (xAbs > zAbs) {
                return 0;
            }
            return 2;
        }
        if (yAbs > zAbs) {
            return 1;
        }
        return 2;
    }

double maxNorm() {
        return std::max(abs(x), std::max(abs(y), abs(z)));
    }
};


#endif //MYRENDER_VECTOR3D_H
