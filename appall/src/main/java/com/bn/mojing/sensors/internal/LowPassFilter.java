/*
 * Decompiled with CFR 0_119.
 */
package com.bn.mojing.sensors.internal;

import java.util.concurrent.TimeUnit;

public class LowPassFilter {
    private static final double NANOS_TO_SECONDS = 1.0 / (double)TimeUnit.NANOSECONDS.convert(1, TimeUnit.SECONDS);
    private final double timeConstantSecs;
    private final Vector3d filteredData = new Vector3d();
    private long lastTimestampNs;
    private int numSamples;
    private final Vector3d temp = new Vector3d();

    public LowPassFilter(double cutoffFrequency) {
        this.timeConstantSecs = 1.0 / (6.283185307179586 * cutoffFrequency);
    }

    public int getNumSamples() {
        return this.numSamples;
    }

    public void addSample(Vector3d sampleData, long timestampNs) {
        this.addWeightedSample(sampleData, timestampNs, 1.0);
    }

    public void addWeightedSample(Vector3d sampleData, long timestampNs, double weight) {
        ++this.numSamples;
        if (this.numSamples == 1) {
            this.filteredData.set(sampleData);
            this.lastTimestampNs = timestampNs;
            return;
        }
        double weightedDeltaSecs = weight * (double)(timestampNs - this.lastTimestampNs) * NANOS_TO_SECONDS;
        double alpha = weightedDeltaSecs / (this.timeConstantSecs + weightedDeltaSecs);
        this.filteredData.scale(1.0 - alpha);
        this.temp.set(sampleData);
        this.temp.scale(alpha);
        Vector3d.add(this.temp, this.filteredData, this.filteredData);
        this.lastTimestampNs = timestampNs;
    }

    public Vector3d getFilteredData() {
        return this.filteredData;
    }
}
