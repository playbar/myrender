/*
 * Decompiled with CFR 0_119.
 */
package com.bn.mojing.sensors.internal;

public class Vector3d {
    public double x;
    public double y;
    public double z;

    public Vector3d() {
    }

    public Vector3d(double xx, double yy, double zz) {
        this.set(xx, yy, zz);
    }

    public void set(double xx, double yy, double zz) {
        this.x = xx;
        this.y = yy;
        this.z = zz;
    }

    public void setComponent(int i, double val) {
        if (i == 0) {
            this.x = val;
        } else if (i == 1) {
            this.y = val;
        } else {
            this.z = val;
        }
    }

    public void setZero() {
        this.z = 0.0;
        this.y = 0.0;
        this.x = 0.0;
    }

    public void set(Vector3d other) {
        this.x = other.x;
        this.y = other.y;
        this.z = other.z;
    }

    public void scale(double s) {
        this.x *= s;
        this.y *= s;
        this.z *= s;
    }

    public void normalize() {
        double d = this.length();
        if (d != 0.0) {
            this.scale(1.0 / d);
        }
    }

    public static double dot(Vector3d a, Vector3d b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    public double length() {
        return Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
    }

    public boolean sameValues(Vector3d other) {
        return this.x == other.x && this.y == other.y && this.z == other.z;
    }

    public static void add(Vector3d a, Vector3d b, Vector3d result) {
        result.set(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    public static void sub(Vector3d a, Vector3d b, Vector3d result) {
        result.set(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    public static void cross(Vector3d a, Vector3d b, Vector3d result) {
        result.set(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    public static void ortho(Vector3d v, Vector3d result) {
        int k = Vector3d.largestAbsComponent(v) - 1;
        if (k < 0) {
            k = 2;
        }
        result.setZero();
        result.setComponent(k, 1.0);
        Vector3d.cross(v, result, result);
        result.normalize();
    }

    public String toString() {
        return String.format("%+05f %+05f %+05f", this.x, this.y, this.z);
    }

    public static int largestAbsComponent(Vector3d v) {
        double xAbs = Math.abs(v.x);
        double yAbs = Math.abs(v.y);
        double zAbs = Math.abs(v.z);
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

    public double maxNorm() {
        return Math.max(Math.abs(this.x), Math.max(Math.abs(this.y), Math.abs(this.z)));
    }
}
