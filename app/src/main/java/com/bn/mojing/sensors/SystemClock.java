/*
 * Decompiled with CFR 0_119.
 */
package com.bn.mojing.sensors;

public class SystemClock
implements Clock {
    @Override
    public long nanoTime() {
        return System.nanoTime();
    }
}
