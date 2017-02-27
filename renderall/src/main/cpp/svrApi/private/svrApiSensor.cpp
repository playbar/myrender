//=============================================================================
// FILE: svrApiSensor.cpp
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#include <android/looper.h>
#include <android/sensor.h>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

#include "svrProfile.h"
#include "svrUtil.h"
#include "svrConfig.h"

#include "private/svrApiHelper.h"
#include "private/svrApiSensor.h"


using namespace Svr;

VAR(int, gSensorThreadCore, -1, kVariableNonpersistent);
VAR(int, gSensorThreadLooperWait, 250, kVariableNonpersistent);


struct SvrSensorSample
{
    int         sensorType;         // See "sensor.h" in the Android NDK
    float       sensorData[16];     // Same size as the Android sensor data (Caution: sizeof() used to copy so don't change!)
    int64_t     sensorTimeStamp;
};

// Are we logging the sensor sample data?
// #define LOG_SENSOR_DATA

#define SENSOR_STAT_UPDATE_TIME_MS      1000
#define SENSOR_STAT_NAME_SIZE           64
#define MAX_SENSOR_LOG_READINGS         1500    // We are expecting 1000 Hz on the data so make sure we have enough storage

struct SvrSensorStats
{
    char            sensorName[SENSOR_STAT_NAME_SIZE];
    unsigned int    lastReportTime;
    long long       lastEventTime;

    unsigned int    eventCount;
    unsigned int    eventLateCount;
    float           eventsPerSecond;

    long long       minReportDelay;
    long long       maxReportDelay;
    float           averageReportDelay;

    long long       minEventDelay;
    long long       maxEventDelay;
    float           averageEventDelay;

    svrVector4      currentValue;

#ifdef LOG_SENSOR_DATA
    float           eventTimes[MAX_SENSOR_LOG_READINGS];
    unsigned int    eventTimeLogIndx;
#endif // LOG_SENSOR_DATA

};

// Tracking sensor data/events/stats
SvrSensorStats  gAccelStats;
SvrSensorStats  gGyroStats;

//-----------------------------------------------------------------------------
unsigned int GetTimeMS()
//-----------------------------------------------------------------------------
{
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;

    if (gettimeofday(&t, NULL) == -1)
    {
        return 0;
    }

    return (unsigned int)(t.tv_sec * 1000LL + t.tv_usec / 1000LL);
}


//-----------------------------------------------------------------------------
long long GetTimeMicroSec()
//-----------------------------------------------------------------------------
{
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;

    if (gettimeofday(&t, NULL) == -1)
    {
        return 0;
    }

    return (long long)(t.tv_sec * 1000000LL + t.tv_usec);
}

#ifdef LOG_SENSOR_DATA
//-----------------------------------------------------------------------------
void LogSensorStats(SvrSensorStats *pStats)
//-----------------------------------------------------------------------------
{
    unsigned int timeNow = GetTimeMS();

    char fileName[512];
    sprintf(fileName, "/sdcard/SensorTiming/%s_Timing_%d.txt", pStats->sensorName, timeNow);

    FILE *pFile = fopen(fileName, "w+");
    if (pFile == NULL)
    {
        LOGE("****************************************");
        LOGE("Unable to create file: %s", fileName);
        LOGE("****************************************");
        return;
    }

    for (unsigned int whichSample = 0; whichSample < pStats->eventTimeLogIndx; whichSample++)
    {
        fprintf(pFile, "%0.2f \n", pStats->eventTimes[whichSample]);
    }

    fclose(pFile);
}
#endif // LOG_SENSOR_DATA


//-----------------------------------------------------------------------------
void UpdateSensorStats(SvrSensorStats *pStats, float *pSensorData)
//-----------------------------------------------------------------------------
{
    unsigned int timeNowMS = GetTimeMS();
    long long timeNowMicro = GetTimeMicroSec();

    // If this is the first time in just set the time and get out
    if (pStats->lastReportTime == 0 || pStats->lastEventTime == 0)
    {
        pStats->lastReportTime = timeNowMS;
        pStats->lastEventTime = timeNowMicro;
        return;
    }

    // Update the current value
    pStats->currentValue.x = pSensorData[0];
    pStats->currentValue.y = pSensorData[1];
    pStats->currentValue.z = pSensorData[2];

    // Need to know how much time has elapsed since last event
    long long elapsedEvent = timeNowMicro - pStats->lastEventTime;
    if (elapsedEvent < 250LL)
    {
        // Basically throwing out any event that comes in faster than a quarter millisecond
        //LOGI("***** %s Sensor event with zero elapsed time! *****", pStats->sensorName);
        // return;
    }
    pStats->lastEventTime = timeNowMicro;

    if (elapsedEvent > 2000LL)
    {
        // We consider this one late
        pStats->eventLateCount++;
    }

    // Need to know how much time has elapsed since last report
    unsigned int elapsedReport = timeNowMS - pStats->lastReportTime;


    // Event count is just an increment
    pStats->eventCount++;

    // Events per second (watch out for zero time passed)
    if (elapsedReport > 0)
    {
        pStats->eventsPerSecond = (float)pStats->eventCount / ((float)elapsedReport * 1000.0f);
    }

    if (elapsedEvent < pStats->minReportDelay)
        pStats->minReportDelay = elapsedEvent;

    if (elapsedEvent > pStats->maxReportDelay)
        pStats->maxReportDelay = elapsedEvent;

    // Recalculate the average
    float thisDelay = (float)elapsedEvent / 1000.0f;
    float thisAverage = pStats->averageReportDelay * (pStats->eventCount - 1);
    thisAverage += thisDelay;
    thisAverage /= pStats->eventCount;
    pStats->averageReportDelay = thisAverage;

#ifdef LOG_SENSOR_DATA
    if (pStats->eventTimeLogIndx < MAX_SENSOR_LOG_READINGS)
    {
        pStats->eventTimes[pStats->eventTimeLogIndx] = thisDelay;
        pStats->eventTimeLogIndx++;
    }
#endif // LOG_SENSOR_DATA

    // Is it time to update the actual statistics?
    if (elapsedReport > SENSOR_STAT_UPDATE_TIME_MS)
    {
        pStats->minEventDelay = pStats->minReportDelay;
        pStats->maxEventDelay = pStats->maxReportDelay;
        pStats->averageEventDelay = pStats->averageReportDelay;
        // LOGI("%s Stats (%d events in %d ms): Min = %lld us; Max = %lld us; Average = %0.2f ms; Late = %d", pStats->sensorName, pStats->eventCount, elapsedReport, pStats->minEventDelay, pStats->maxEventDelay, pStats->averageEventDelay, pStats->eventLateCount);

#ifdef LOG_SENSOR_DATA
        LogSensorStats(pStats);

        memset(pStats->eventTimes, 0, sizeof(pStats->eventTimes));
        pStats->eventTimeLogIndx = 0;
#endif // LOG_SENSOR_DATA

        pStats->lastReportTime = timeNowMS;

        pStats->eventCount = 0;
        pStats->eventLateCount = 0;
        pStats->minReportDelay = 1000.0f;
        pStats->maxReportDelay = 0.0f;

    }

}


//-----------------------------------------------------------------------------
void HandleAccelerometerEvent(ASensorEvent sensorEvent)
//-----------------------------------------------------------------------------
{
    // Pass to the tracker class
    float landscapeData[3];
   
    landscapeData[0] = -sensorEvent.data[1];
    landscapeData[1] = sensorEvent.data[0];
    landscapeData[2] = sensorEvent.data[2];
   
    UpdateSensorStats(&gAccelStats, landscapeData);
}

//-----------------------------------------------------------------------------
void HandleGyroscopeEvent(ASensorEvent sensorEvent)
//-----------------------------------------------------------------------------
{
    // Pass to the tracker class
    float landscapeData[3];
   
    landscapeData[0] = -sensorEvent.data[1];
    landscapeData[1] = sensorEvent.data[0];
    landscapeData[2] = sensorEvent.data[2];
    
    UpdateSensorStats(&gGyroStats, landscapeData);
}

//-----------------------------------------------------------------------------
int SensorCallback(int fd, int events, void* data)
//-----------------------------------------------------------------------------
{
    ASensorEvent sensorEvent;

    PROFILE_ENTER(GROUP_SENSORS, 0, "SensorCallback");

    while ( !gAppContext->modeContext->sensorThreadExit &&
            ASensorEventQueue_getEvents(gAppContext->modeContext->sensorEventQueue, &sensorEvent, 1) > 0)
    {
        SvrSensorSample sample;
        sample.sensorType = sensorEvent.type;
        memcpy(sample.sensorData, sensorEvent.data, sizeof(sample.sensorData));
        sample.sensorTimeStamp = sensorEvent.timestamp;

        switch (sensorEvent.type)
        {
        case ASENSOR_TYPE_ACCELEROMETER:
            HandleAccelerometerEvent(sensorEvent);
            break;
        case ASENSOR_TYPE_GYROSCOPE:
            HandleGyroscopeEvent(sensorEvent);
            break;
        }
    }

    PROFILE_EXIT(GROUP_SENSORS);

    return 1;
}

//-----------------------------------------------------------------------------
void* SensorThreadMain(void* arg)
//-----------------------------------------------------------------------------
{
    PROFILE_THREAD_NAME(gTelemetryData.context, 0, "Sensors");
    PROFILE_ENABLE_GROUP(GROUP_SENSORS, 1);

    gAppContext->modeContext->sensorThreadLooper = ALooper_prepare(0);

    if (gSensorThreadCore >= 0)
    {
        LOGI("Setting Sensor Affinity to %d", gSensorThreadCore);
        svrSetThreadAffinity(gSensorThreadCore);
    }

    // Clean up the statistics
    memset(&gAccelStats, 0, sizeof(SvrSensorStats));
    memset(&gGyroStats, 0, sizeof(SvrSensorStats));

    sprintf(gAccelStats.sensorName, "Accelerometer");
    sprintf(gGyroStats.sensorName , "Gyroscope");


    //Signal the waiting thread that the looper has been created for this thread
    //and we're ready to receive events
    pthread_mutex_lock(&gAppContext->modeContext->sensorThreadReadyMutex);
    gAppContext->modeContext->sensorThreadReady = true;
    pthread_cond_signal(&gAppContext->modeContext->sensorThreadReadyCv);
    pthread_mutex_unlock(&gAppContext->modeContext->sensorThreadReadyMutex);

    LOGI("Sensor Thread starting loop...");
    while (1)
    {
        int id;
        int events;

        // Poll the events, which causes the callback function to be invoked.
        // This routine basically never returns until ALooper_wake() is called.
        // When called this returns with ALOOPER_POLL_WAKE.
        id = ALooper_pollAll(gSensorThreadLooperWait, NULL, &events, 0);
        switch (id)
        {
        case ALOOPER_POLL_WAKE:     // -1
            // This should not be spam since only called when exiting
            LOGI("ALooper_pollAll() returned ALOOPER_POLL_WAKE! %d Events", events);
            break;
        
        case ALOOPER_POLL_CALLBACK: // -2
            LOGI("ALooper_pollAll() returned ALOOPER_POLL_CALLBACK! %d Events", events);
            break;
        
        case ALOOPER_POLL_TIMEOUT:  // -3
            // Since this happens all the time we don't want to spam
            // LOGI("ALooper_pollAll() returned ALOOPER_POLL_TIMEOUT! %d Events", events);
            break;
        
        case ALOOPER_POLL_ERROR:    // -4
            LOGI("ALooper_pollAll() returned ALOOPER_POLL_ERROR! %d Events", events);
            break;
        
        default:
            LOGI("ALooper_pollAll() returned Unknown (%d)! %d Events", id, events);
            break;
        }

        //Check if the main thread has requested an exit
        if (gAppContext->modeContext->sensorThreadExit)
        {
            // There is a timing issue here.  If we leave too quickly then pthread_join() below
            // hangs forever.  Simply logging stuff to logcat is enough of a delay. Therefore,
            // add a wait here.
            timespec t, rem;
            t.tv_sec = 0;
            t.tv_nsec = 250000000;  // 250 million nanoseconds is 250 milliseconds
            nanosleep(&t, &rem);

            LOGI("Sensor Thread Exiting!");
            return 0;
        }
    }   // while (1)
}

//-----------------------------------------------------------------------------
bool svrStartSensors(bool UseDSPSensors)
//-----------------------------------------------------------------------------
{
    gAppContext->modeContext->sensorManager = ASensorManager_getInstance();

    //Start up the sensor looper thread
    LOGI("    Starting sensor looper thread...");
    pthread_mutex_init(&gAppContext->modeContext->sensorThreadReadyMutex, NULL);
    pthread_cond_init(&gAppContext->modeContext->sensorThreadReadyCv, NULL);
    gAppContext->modeContext->sensorThreadReady = false;
    
    gAppContext->modeContext->sensorThreadExit = false;
    int status = pthread_create(&gAppContext->modeContext->sensorThread, NULL, &SensorThreadMain, NULL);
    if (status != 0)
    {
        LOGE("svrStartSensors: Failed to create sensor thread");
    }
    pthread_setname_np(gAppContext->modeContext->sensorThread, "svrSensor");
    
    //Wait for the sensor thread to be setup and ready
    pthread_mutex_lock(&gAppContext->modeContext->sensorThreadReadyMutex);
    while (!gAppContext->modeContext->sensorThreadReady)
    {
        pthread_cond_wait(&gAppContext->modeContext->sensorThreadReadyCv, &gAppContext->modeContext->sensorThreadReadyMutex);
    }
    pthread_mutex_unlock(&gAppContext->modeContext->sensorThreadReadyMutex);

    //Grab the default sensor for each type
    gAppContext->modeContext->accVectorSensor = ASensorManager_getDefaultSensor(gAppContext->modeContext->sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    gAppContext->modeContext->gyroVectorSensor = ASensorManager_getDefaultSensor(gAppContext->modeContext->sensorManager, ASENSOR_TYPE_GYROSCOPE);

   
    //Create the event queue
    gAppContext->modeContext->sensorEventQueue = ASensorManager_createEventQueue(gAppContext->modeContext->sensorManager, gAppContext->modeContext->sensorThreadLooper, 0, SensorCallback, (void*)gAppContext->modeContext);

    if (gAppContext->modeContext->accVectorSensor != NULL)
    {
        LOGI("    Enabling acceleration sensor...");
        ASensorEventQueue_enableSensor(gAppContext->modeContext->sensorEventQueue, gAppContext->modeContext->accVectorSensor);

        int minDelay = ASensor_getMinDelay(gAppContext->modeContext->accVectorSensor);

        // Since DSP grabs sensor data tell Android not to try so hard
        // Default seems to be 200 Hz so go to once a second
        if (UseDSPSensors)
            minDelay *= 200;

        LOGI("Min Acceleration Sensor Delay : %0.2f ms", (float)minDelay / 1000.0f);
        ASensorEventQueue_setEventRate(gAppContext->modeContext->sensorEventQueue, gAppContext->modeContext->accVectorSensor, minDelay);
    }

    if (gAppContext->modeContext->gyroVectorSensor != NULL)
    {
        LOGI("    Enabling gyroscope sensor...");
        ASensorEventQueue_enableSensor(gAppContext->modeContext->sensorEventQueue, gAppContext->modeContext->gyroVectorSensor);

        int minDelay = ASensor_getMinDelay(gAppContext->modeContext->gyroVectorSensor);

        // Since DSP grabs sensor data tell Android not to try so hard
        // Default seems to be 200 Hz so go to once a second
        if (UseDSPSensors)
            minDelay *= 200;

        LOGI("Min Gyroscope Sensor Delay : %0.2f ms", (float)minDelay / 1000.0f);
        ASensorEventQueue_setEventRate(gAppContext->modeContext->sensorEventQueue, gAppContext->modeContext->gyroVectorSensor, minDelay);
    }

    LOGI("    Sensors started");
    return true;
}

//-----------------------------------------------------------------------------
bool svrStopSensors()
//-----------------------------------------------------------------------------
{
    //Signal our sensor thread to stop
    LOGI("    Setting sensor thread exit flag...");
    gAppContext->modeContext->sensorThreadExit = true;
    LOGI("    Waking the looper...");
    ALooper_wake(gAppContext->modeContext->sensorThreadLooper);
    LOGI("    Waiting for the sensor thread to exit...");
    pthread_join(gAppContext->modeContext->sensorThread, NULL);

    //Disable the sensors
    if (gAppContext->modeContext->accVectorSensor != NULL)
    {
        LOGI("    Disabling acceleration sensor...");
        ASensorEventQueue_disableSensor(gAppContext->modeContext->sensorEventQueue, gAppContext->modeContext->accVectorSensor);
    }

    if (gAppContext->modeContext->gyroVectorSensor != NULL)
    {
        LOGI("    Disabling gyroscope sensor...");
        ASensorEventQueue_disableSensor(gAppContext->modeContext->sensorEventQueue, gAppContext->modeContext->gyroVectorSensor);
    }

    //Destroy the event queue
    // THIS NEVER RETURNS!
    // LOGI("    Destroying sensor event queue...");
    // ASensorManager_destroyEventQueue(gAppContext->modeContext->sensorManager, gAppContext->modeContext->sensorEventQueue);
    gAppContext->modeContext->sensorEventQueue = NULL;

    LOGI("    Sensors stopped");
    return true;
}

//-----------------------------------------------------------------------------
glm::fquat svrGetHeadPoseAsQuat(float predictedTimeMs)
//-----------------------------------------------------------------------------
{
    glm::fquat retPoseQuat = glm::fquat();

    if (gAppContext == NULL || gAppContext->modeContext == NULL)
    {
        return retPoseQuat;
    }

    // TODO: Implement based on CPU sensors

    return retPoseQuat;
}



