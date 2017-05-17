//
// Created by houguoli on 2017/5/17.
//

#ifndef MYRENDER_LOWPASSFILTER_H
#define MYRENDER_LOWPASSFILTER_H


#include "Vector3d.h"

uint64_t GetTimeNano();

class LowPassFilter {
public:
    static double NANOS_TO_SECONDS;
    double timeConstantSecs;
    Vector3dJ filteredData;
    long lastTimestampNs;
    int numSamples;
    Vector3dJ temp;

    LowPassFilter(double cutoffFrequency) {
        timeConstantSecs = 1.0 / (6.283185307179586 * cutoffFrequency);
    }

    void reset(double cutoffFrequency)
    {
        timeConstantSecs = 1.0 / (6.283185307179586 * cutoffFrequency);
    }

    int getNumSamples() {
        return numSamples;
    }

    void addSample(Vector3dJ &sampleData, long timestampNs) {
        addWeightedSample(sampleData, timestampNs, 1.0);
    }

    void addWeightedSample(Vector3dJ &sampleData, long timestampNs, double weight) {
        ++numSamples;
        if (numSamples == 1) {
            filteredData.set(sampleData);
            lastTimestampNs = timestampNs;
            return;
        }
        double weightedDeltaSecs = weight * (double)(timestampNs - lastTimestampNs) * NANOS_TO_SECONDS;
        double alpha = weightedDeltaSecs / (timeConstantSecs + weightedDeltaSecs);
        filteredData.scale(1.0 - alpha);
        temp.set(sampleData);
        temp.scale(alpha);
        Vector3dJ::add(temp, filteredData, filteredData);
        lastTimestampNs = timestampNs;
    }

    Vector3dJ& getFilteredData() {
        return filteredData;
    }
};


#endif //MYRENDER_LOWPASSFILTER_H
