//=============================================================================
//! \file svrApi.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================

#ifndef _SVR_API_H_
#define _SVR_API_H_

#include <stdlib.h>
#include <jni.h>

#define MJ_OVERLAY              1
#define SVR_MAJOR_VERSION       1
#define SVR_MINOR_VERSION       1
#define SVR_REVISION_VERSION    3

#define SVR_MAX_EYE_BUFFERS 4

#define SVR_NUM_OVERLAYS    3

struct ANativeWindow;

#ifdef MJ_OVERLAY
struct svrRect
{
    float fX, fY, fW, fH;
};
#endif

//! \brief Simple structure to hold 3-component vector data
struct svrVector3
{
    float x, y, z;
};

//! \brief Simple structure to hold 4-component vector data
struct svrVector4
{
    float x, y, z, w;
};

//! \brief Simple structure to hold quaternion data
struct svrQuaternion
{
    float x, y, z, w;
};

//! \brief Simple structure to hold 4x4 matrix data
struct svrMatrix4
{
    float M[4][4];
};

//! \brief Enum used to indicate which eye is being used
enum svrWhichEye
{
    kLeftEye = 0,
    kRightEye
};

//! \brief Structure containing the position and orientation of the head
struct svrHeadPose
{
    svrQuaternion   rotation;
    svrVector3      position;
};

//! \brief Enum used to indicate valid components of an svrHeadPose
enum svrTrackingMode
{
    kTrackingRotation = (1 << 0),
    kTrackingPosition = (1 << 1)
};

//! \brief Structure containing the full set of pose data
struct svrHeadPoseState
{
    svrHeadPose         pose;                   //!< Head pose
    int                 poseStatus;             //!< Bit field (svrTrackingMode) indicating head pose status
    int64_t             poseTimeStampNs;        //!< Time stamp in which the head pose was generated (nanoseconds)
    svrVector3          angularVelocity;        //!< Angular velocity
    svrVector3          linearVelocity;         //!< Linear velocity
    svrVector3          angularAcceleration;    //!< Angular acceleration
    svrVector3          linearAccelearation;    //!< Linear acceleration
    float               predictedTimeMs;        //!< Prediction time used to generate the head pose (milliseconds)
};

//! \brief Enum used for indicating the CPU/GPU performance levels
//! \sa svrBeginVr, svrSetPerformanceLevels
enum svrPerfLevel
{
    kPerfSystem = 0,        //!< System defined performance level (default)
    kPerfMinimum = 1,        //!< Minimum performance level (default 30-50% of max frequency)
    kPerfMedium = 2,        //!< Medium performance level (default 51-80% of max frequency)
    kPerfMaximum = 3         //!< Maximum performance level (default 81-100% of max frequency)
};

//! \brief Structure containing parameters needed to enable VR mode
//! \sa svrBeginVr
struct svrBeginParams
{
    int             mainThreadId;    //!< Thread Id of the primary render thread
    svrPerfLevel    cpuPerfLevel;    //!< Desired CPU performance level
    svrPerfLevel    gpuPerfLevel;    //!< Desired GPU performance level
    ANativeWindow*  nativeWindow;    //!< Pointer to the Android native window

    float* pVertexLeft;
    float* pVertexRight;
    unsigned int* pIndexLeft;
    unsigned int* pIndexRight;
    size_t numVertex;
    size_t numIndex;

};

//! \brief Options which can be set when submitting a frame to modify the behavior of asynchronous time warp
//! \sa svrSubmitFrame
enum svrFrameOption
{
    kDisableDistortionCorrection = (1 << 0),   //!< Disables the lens distortion correction (useful for debugging)
    kDisableReprojection = (1 << 1),   //!< Disables re-projection
    kEnableMotionToPhoton = (1 << 2),   //!< Enables motion to photon testing 
    kDisableChromaticCorrection = (1 << 3)   //!< Disables the lens chromatic aberration correction (performance optimization)
};

//! \brief Enum used to indicate the layout of the eye buffers being submitted to asynchronous time warp
//! \sa svrSubmitFrame
enum svrEyeBufferType
{
    kEyeBufferMono,                 //!< Single eye buffer which will be duplicated in the left and right eyes
    kEyeBufferStereoSeparate,       //!< Separate eye buffers for the left and right eyes
    kEyeBufferStereoSingle,         //!< Single double-wide eye buffer containing both the left and right eyes
    kEyeBufferArray,                //!< Single array where the 1st and 2nd slices contain the left and right eye buffers
};

//! \brief Enum used to indicate the type of texture passed in as an overlay buffer
//! \sa svrSubmitFrame
enum svrOverlayType
{
    kOverlayMono = 0,               //!< Same full screen image used on both eyes
    kOverlayStereo,                 //!< Separate fullscreen image overlay for the left and right eyes
    kOverlayLayers,                 //!< Each layer is rendered using the coordinates specified. Same render on each eye
};

//! \brief Enum used to indicate the format of texture passed in as an overlay buffer
//! \sa svrSubmitFrame
enum svrOverlayFormat
{
    kOverlayTexture = 0,            //!< Standard texture
    kOverlayImage,                  //!< EGL Image texture
};

//! \brief Enum used to indicate the type of warp/composition that should be used for a frame
enum svrWarpType
{
    kSimple                         //!< Basic single layer (world) warp 
};

//! \brief Overlay screen position (First two) and UV coordinates (Second two)
//! \sa svrSubmitFrame
struct svrOverlayLayout
{
    float               LowerLeft[4];                           //!< 0 = X-Position; 1 = Y-Position; 2 = U-Value; 3 = V-Value
    float               LowerRight[4];                          //!< 0 = X-Position; 1 = Y-Position; 2 = U-Value; 3 = V-Value
    float               UpperLeft[4];                           //!< 0 = X-Position; 1 = Y-Position; 2 = U-Value; 3 = V-Value
    float               UpperRight[4];                          //!< 0 = X-Position; 1 = Y-Position; 2 = U-Value; 3 = V-Value
};


//! \brief Per-frame data needed for time warp, distortion/aberration correction
//! \sa svrSubmitFrame
struct svrFrameParams
{
    int                 frameIndex;                             //!< Frame Index
    int                 minVsyncs;                              //!< Minimum number of vysnc events before displaying the frame (1=display refresh, 2=half refresh, etc...)

    svrEyeBufferType    eyeBufferType;                          //!< Layout for the supplied eye buffer(s)
    int                 eyeBufferArray[SVR_MAX_EYE_BUFFERS];    //!< Array of eye buffer identifiers.  Number and layout dictated by the eyeBufferType

    svrOverlayType      overlayType;                            //!< Type of overlay buffer
    svrOverlayFormat    overlayFormat;                          //!< Format of overlay buffer
    int                 overlayBuffer[SVR_NUM_OVERLAYS];        //!< Overlay buffer identifier.  This buffer will be rendered on top of eye buffer
    svrOverlayLayout    overlayLayout[SVR_NUM_OVERLAYS];        //!< Overlay coordinate layout
#ifdef MJ_OVERLAY
    svrRect             overlayRect[2];
#endif
    unsigned int        frameOptions;                           //!< Options for adjusting the frame warp behavior (bitfield of svrFrameOption)
    svrHeadPoseState    headPoseState;                          //!< Head pose state used to generate the frame  
    svrWarpType         warpType;                               //!< Type of warp to be used on the frame
};

//! \brief Initialization parameters that are constant over the life-cycle of the application
//! \sa svrInitialize
struct svrInitParams
{
    JavaVM*         javaVm;                 //!< Java Virtual Machine pointer
    JNIEnv*         javaEnv;                //!< Java Environment
    jobject         javaActivityObject;     //!< Reference to the Android activity
};

//! \brief Basic device information to allow the client code to optimally setup their simulation and rendering pipelines
struct svrDeviceInfo
{
    int     displayWidthPixels;             //!< Physical width of the display (pixels)
    int     displayHeightPixels;            //!< Physical height of the display (pixels)
    float   displayRefreshRateHz;           //!< Refresh rate of the display
    int     displayOrientation;             //!< Display orientation (degrees at initialization - 0,90,180,270)
    int     targetEyeWidthPixels;           //!< Recommended eye buffer width (pixels)
    int     targetEyeHeightPixels;          //!< Recommended eye buffer height (pixels)
    float   targetFovXRad;                  //!< Recommended horizontal FOV
    float   targetFovYRad;                  //!< Recommended vertical FOV
    int     deviceOSVersion;                //!< Android OS Version of the device
};

#ifndef SVRP_EXPORT
#define SVRP_EXPORT
#endif

#ifdef __cplusplus 
extern "C" {
#endif

    //! \brief Returns the VR SDK version string
    SVRP_EXPORT const char* svrGetVersion();

    //! \brief Initializes VR components 
    //! \param pInitParams svrInitParams structure
    SVRP_EXPORT bool svrInitialize(const svrInitParams* pInitParams);

    //! \brief Releases VR components
    SVRP_EXPORT void svrShutdown();

    //! \brief Queries for device specific information
    //! \return svrDeviceInfo structure containing device specific information (resolution, fov, etc..)
    SVRP_EXPORT svrDeviceInfo   svrGetDeviceInfo();

    //! \brief Requests specific brackets of CPU/GPU performance
    //! \param cpuPerfLevel Requested performance level for CPU
    //! \param gpuPerfLevel Requested performance level for GPU
    SVRP_EXPORT void svrSetPerformanceLevels(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel);

    //! \brief Enables VR services
    //! \param pBeginParams svrBeginParams structure
    SVRP_EXPORT void svrBeginVr(const svrBeginParams* pBeginParams);

    //! \brief Disables VR services
    SVRP_EXPORT void svrEndVr();

    //! \brief Calculates a predicted time when the current frame would be displayed
    //! \return Predicted display time for the current frame in milliseconds
    SVRP_EXPORT float svrGetPredictedDisplayTime();

    //! \brief Calculates a predicted head pose
    //! \param predictedTimeMs Time ahead of the current time in ms to predict a head pose for
    //! \return The predicted head pose and relevant pose state information
    SVRP_EXPORT svrHeadPoseState svrGetPredictedHeadPose(float predictedTimeMs);

    //! \brief Re centers the head pose at the current position
    SVRP_EXPORT void svrRecenterPose();

    //! \brief Returns the supported tracking types
    //! \return Bitfield of svrTrackingType values indicating the supported tracking modes
    SVRP_EXPORT unsigned int svrGetSupportedTrackingModes();

    //! \brief Sets the current head tracking mode
    //! \param trackingModes Bitfield of svrTrackingType values indicating the tracking modes to enable
    SVRP_EXPORT void svrSetTrackingMode(unsigned int trackingModes);

    //! \brief Submits a frame to asynchronous time warp
    //! \param pFrameParams svrFrameParams structure
    SVRP_EXPORT void svrSubmitFrame(const svrFrameParams* pFrameParams);

    SVRP_EXPORT void svrUpdateWarpmesh(
        const float* pVertexLeft,
        const float* pVertexRight,
        const size_t numVertex,
        const unsigned int* pIndex,
        const size_t numIndex
        );

    SVRP_EXPORT bool svrCheckServiceIsAvaliable();

    SVRP_EXPORT void svrSetTimewarp(bool enable);

#ifdef __cplusplus 
}
#endif

#endif //_SVR_API_H_
