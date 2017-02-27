//=============================================================================
// FILE: svrApiPredictiveSensor.cpp
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifdef USE_QVR_SERVICE

#include <android/looper.h>
#include <android/sensor.h>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

#include "svrApi.h"
#include "svrProfile.h"
#include "svrUtil.h"
#include "svrConfig.h"

#include "private/svrApiCore.h"
#include "private/svrApiPredictiveSensor.h"
#include "private/svrApiTimeWarp.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <dlfcn.h>

using namespace Svr;


typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

// Sensor Parameters
VAR(float, gSensorOrientationCorrectX, 0.0f, kVariableNonpersistent);   //Adjustment if sensors are physically rotated (degrees)
VAR(float, gSensorOrientationCorrectY, 0.0f, kVariableNonpersistent);   //Adjustment if sensors are physically rotated (degrees)
VAR(float, gSensorOrientationCorrectZ, 0.0f, kVariableNonpersistent);   //Adjustment if sensors are physically rotated (degrees)
VAR(int, gSensorHomePosition, 0, kVariableNonpersistent);   // Base device configuration. 0 = Landscape Left; 1 = Landscape Right

void qIntegrate(float *_q0, float* _q1, float* _q2, float* _q3,float gx, float gy, float gz ){

    float recipNorm;
    float q0 = *_q0;
    float q1 = *_q1;
    float q2 = *_q2;
    float q3 = *_q3;

    float qa = q0;
    float qb = q1;
    float qc = q2;

    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    // Normalise quaternion
    recipNorm = 1.0f/sqrtf(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;

    *_q0 = q0;
    *_q1 = q1;
    *_q2 = q2;
    *_q3 = q3;
}

bool  _predict_motion(
    float* gyro_data, const float *quatIn, float *quatOut, float delay,
    const float* gyro_data_s, const float* gyro_data_b, const float* gyro_data_bdt, const float* gyro_data_bdt2)
    
{
    //LOGI("prediction time: %f\n\n", delay);
    delay/=1000.f;

    float gx, gy, gz;

    float pred_gyro_data[3];
    float f1 = 1.1f;
    float f2 = 1.1f;
    float f3 = 5.0f;
    for (int i = 0; i < 3; ++i)
    {
        pred_gyro_data[i] = (gyro_data_s[i] *delay
                              + (gyro_data_b[i] * delay * delay) / (f1 * 2.0f)
                              + (gyro_data_bdt[i] * (delay * delay * delay)) / (f1 * f2 * 3.0f)
                              + (gyro_data_bdt2[i] * (delay * delay * delay * delay)) / (f1 * f2 * f3 * 4.0f));
    }

    //LOGI("holt_winters: delay = %f gyro_data_s = (%f %f %f) gyro_data_b = (%f %f %f) gyro_data = (%f %f %f) pred_gyro_data = (%f %f %f)", delay, gyro_data_s[0],gyro_data_s[1],gyro_data_s[2], gyro_data_b[0],gyro_data_b[1],gyro_data_b[2], gyro_data[0],gyro_data[1],gyro_data[2], pred_gyro_data[0], pred_gyro_data[1], pred_gyro_data[2]);

    gx =  pred_gyro_data[0]/2/4;
    gy =  pred_gyro_data[1]/2/4;
    gz =  pred_gyro_data[2]/2/4;

    float q0=quatIn[3]; //w
    float q1=quatIn[0]; //x
    float q2=quatIn[1]; //y
    float q3=quatIn[2]; //z

    qIntegrate(&q0, &q1, &q2, &q3, gx, gy, gz);
    qIntegrate(&q0, &q1, &q2, &q3, gx, gy, gz);
    qIntegrate(&q0, &q1, &q2, &q3, gx, gy, gz);
    qIntegrate(&q0, &q1, &q2, &q3, gx, gy, gz);


    quatOut[3]=q0; //w
    quatOut[0]=q1; //x
    quatOut[1]=q2; //y
    quatOut[2]=q3; //z

    return true;
}

bool GetTrackingFromPredictiveSensor(float fw_prediction_delay, uint64_t *pSampleTimeStamp, glm::fquat& quat, glm::vec3& position)
{
    bool res = false;

    if (gAppContext->qvrService == NULL)
    {
        LOGI("QVRService: Unable to read tracking data.  Service has not been initialized");
        return false;
    }

    qvrservice_sensor_tracking_data_t *t;
    if (gAppContext->qvrService->GetSensorTrackingData(&t) < 0) 
    {
        LOGI("QVRService: Error getting tracking data");
        return false;
    }

    *pSampleTimeStamp = t->ts;

    const float* rotation= t->rotation;
    const float* translation = t->translation;

    // Currently GetSensorTrackingData() doesn't always return -1 on an error.  It does return
    // and invalid rotation of [0,0,0,0].  Therefore we are being defensive and checking the return;
    if(rotation[0] == 0.0f && rotation[1] == 0.0f && rotation[2] == 0.0f && rotation[3] == 0.0f)
    {
        LOGI("QVRService: Invalid tracking (rotation) data");
        return false;
    }

    /*
    * For both 6dof and 3dof, service returns pose in Android Potrait orientation (x up, y left, z towards user)
    */
#if 1
    float quatOut[4] = {0.f};
    float gyro_data[3] = {0.f};

    fw_prediction_delay /= 1.25;

    _predict_motion(gyro_data, rotation, quatOut, fw_prediction_delay, t->prediction_coff_s, t->prediction_coff_b, t->prediction_coff_bdt, t->prediction_coff_bdt2);
    quat.x = quatOut[0];
    quat.y = quatOut[1];
    quat.z = quatOut[2];
    quat.w = quatOut[3];

#else //no prediction
    quat.x = rotation[0];
    quat.y = rotation[1];
    quat.z = rotation[2];
    quat.w = rotation[3];
#endif

    // Adjust for the physical orientation of the sensor
    if(gSensorOrientationCorrectX != 0.0)
    {
        glm::quat adjustRotation = glm::angleAxis(glm::radians(gSensorOrientationCorrectX), glm::vec3(1.0f, 0.0f, 0.0f));
        // LOGI("Adjusting sensor orientation by (%0.4f, %0.4f, %0.4f, %0.4f", adjustRotation.x, adjustRotation.y, adjustRotation.z, adjustRotation.w);
        quat *= adjustRotation;
    }

    if(gSensorOrientationCorrectY != 0.0)
    {
        glm::quat adjustRotation = glm::angleAxis(glm::radians(gSensorOrientationCorrectY), glm::vec3(0.0f, 1.0f, 0.0f));
        // LOGI("Adjusting sensor orientation by (%0.4f, %0.4f, %0.4f, %0.4f", adjustRotation.x, adjustRotation.y, adjustRotation.z, adjustRotation.w);
        quat *= adjustRotation;
    }

    if(gSensorOrientationCorrectZ != 0.0)
    {
        glm::quat adjustRotation = glm::angleAxis(glm::radians(gSensorOrientationCorrectZ), glm::vec3(0.0f, 0.0f, 1.0f));
        // LOGI("Adjusting sensor orientation by (%0.4f, %0.4f, %0.4f, %0.4f", adjustRotation.x, adjustRotation.y, adjustRotation.z, adjustRotation.w);
        quat *= adjustRotation;
    }


    //convert quat to rotation matrix
    glm::mat3 rotation_mat = mat3_cast(quat);
    rotation_mat = glm::transpose(rotation_mat); //converting from col major to row major format
    const float *r= (const float*)glm::value_ptr(rotation_mat);

    //now convert from Android Potrait to Android Landscape orientation

    float input[16] = {0.f};
    input[0] = r[0];
    input[1] = r[1];
    input[2] = r[2];
    input[3] = -translation[0];

    input[4] = r[3];
    input[5] = r[4];
    input[6] = r[5];
    input[7] = -translation[1];

    input[8] = r[6];
    input[9] = r[7];
    input[10] = r[8];
    input[11] = -translation[2];

    input[12] = 0.f;
    input[13] = 0.f;
    input[14] = 0.f;
    input[15] = 1.f;

    // This is confusing!  The matrix represents the CORRECTION to a rotation.
    // So the "Left" matrix is really a "Right" rotation matrix.
    // R(Theta) = | Cos(Theta)  -Sin(Theta) |
    //            | Sin(Theta)   Cos(Theta) |

    float LandRotLeft[16] = {    0.0f,  1.0f,  0.0f,  0.0f,
                                -1.0f,  0.0f,  0.0f,  0.0f,
                                 0.0f,  0.0f,  1.0f,  0.0f,
                                 0.0f,  0.0f,  0.0f,  1.0f};

    float LandRotLeftInv[16]= {  0.0f, -1.0f,  0.0f,  0.0f,
                                 1.0f,  0.0f,  0.0f,  0.0f,
                                 0.0f,  0.0f,  1.0f,  0.0f,
                                 0.0f,  0.0f,  0.0f,  1.0f};

    // Z 90 degrees
    float LandRotRight[16] = {   0.0f, -1.0f,  0.0f,  0.0f,
                                 1.0f,  0.0f,  0.0f,  0.0f,
                                 0.0f,  0.0f,  1.0f,  0.0f,
                                 0.0f,  0.0f,  0.0f,  1.0f};
    
    float LandRotRightInv[16]= { 0.0f,  1.0f,  0.0f,  0.0f,
                                -1.0f,  0.0f,  0.0f,  0.0f,
                                 0.0f,  0.0f,  1.0f,  0.0f,
                                 0.0f,  0.0f,  0.0f,  1.0f};

    glm::mat4 input_glm = glm::make_mat4(input);
    glm::mat4 m = input_glm;

    // Based on the device, adjust the sensor orientation to match
    switch (gSensorHomePosition)
    {
    case kSensorLandscapeLeft:
        {
            // Landscape Left
            //LOGE("Correcting for Landscape Left!");
            glm::mat4 LandRotLeft_glm = glm::make_mat4(LandRotLeft);
            glm::mat4 LandRotLeftInv_glm = glm::make_mat4(LandRotLeftInv);
            m = LandRotLeft_glm * input_glm * LandRotLeftInv_glm;
        }
        break;

    case kSensorLandscapeRight:
        {
            // Landscape Right
            //LOGE("Correcting for Landscape Right!");
            glm::mat4 LandRotRight_glm = glm::make_mat4(LandRotRight);
            glm::mat4 LandRotRightInv_glm = glm::make_mat4(LandRotRightInv);
            m = LandRotRight_glm * input_glm * LandRotRightInv_glm;

            // Need an extra 180 degrees around X
            glm::mat4 t = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(1.0f, 0.0f, 0.0f));
            m = m  * t;
        }
        break;
    }


    if(gAppContext->deviceInfo.displayOrientation == 90)
    {
        // Landscape Left
        //LOGE("Correcting for Landscape Left!");
        // glm::mat4 LandRotLeft_glm = glm::make_mat4(LandRotLeft);
        // glm::mat4 LandRotLeftInv_glm = glm::make_mat4(LandRotLeftInv);
        // m = LandRotLeft_glm * input_glm * LandRotLeftInv_glm;
    }
    else if(gAppContext->deviceInfo.displayOrientation == 270)
    {
        // Landscape Right
        //LOGE("Correcting for Landscape Right!");
        // glm::mat4 LandRotRight_glm = glm::make_mat4(LandRotRight);
        // glm::mat4 LandRotRightInv_glm = glm::make_mat4(LandRotRightInv);
        // m = LandRotRight_glm * input_glm * LandRotRightInv_glm;

        // Need an extra 180 degrees around X
        // glm::mat4 t = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(1.0f, 0.0f, 0.0f));
        // m = m  * t;
    }
    else if (gAppContext->deviceInfo.displayOrientation == 0)
    {
        // Landscape left: sensor input Landscape
        //LOGE("Correcting for Landscape : orientation 0 degree!");
        // glm::mat4 LandRotLeft_glm = glm::make_mat4(LandRotLeft);
        // glm::mat4 LandRotLeftInv_glm = glm::make_mat4(LandRotLeftInv);
        // m = LandRotLeft_glm * input_glm * LandRotLeftInv_glm;
    }
    else if (gAppContext->deviceInfo.displayOrientation == 180)
    {
        // Landscape right: sensor input Landscape
        //LOGE("Correcting for Landscape : orientation 180 degree!");
        // glm::mat4 LandRotRight_glm = glm::make_mat4(LandRotRight);
        // glm::mat4 LandRotRightInv_glm = glm::make_mat4(LandRotRightInv);
        // m = LandRotRight_glm * input_glm * LandRotRightInv_glm;

        // Need an extra 180 degrees around Z
        // glm::mat4 t = glm::rotate(glm::mat4(1.0f), ((float)M_PI), glm::vec3(0.0f, 0.0f, 1.0f));
        // m = m  * t;
    }


    const float *p = (const float*)glm::value_ptr(m);

    position = glm::vec3(p[3], p[7], p[11]);
    quat = glm::quat_cast(m);


    res = true;

    return res;
}

//--------------------------------------------------------------------------------------------------------
int svrGetPredictiveHeadPoseAsQuat(float predictedTimeMs, uint64_t *pSampleTimeStamp, glm::fquat& orientation, glm::vec3& position)
//--------------------------------------------------------------------------------------------------------
{
    if(!GetTrackingFromPredictiveSensor(predictedTimeMs, pSampleTimeStamp, orientation, position))
    {
        LOGE("Error in getting pose from predictive sensor!");
        return 0;
    }
    
    return 1;
}

#endif // USE_QVR_SERVICE
