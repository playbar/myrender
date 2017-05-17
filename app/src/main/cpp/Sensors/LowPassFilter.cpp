//
// Created by houguoli on 2017/5/17.
//

#include "LowPassFilter.h"

uint64_t GetTimeNano()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t result = t.tv_sec * 1000000000LL + t.tv_nsec;
    return result;
}

double LowPassFilter::NANOS_TO_SECONDS = GetTimeNano() * 1e-9;