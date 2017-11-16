/******************************************************************************/
/*! \file  QVRServiceClient.hpp */
/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
******************************************************************************/
#ifndef QVRSERVICE_CLIENT_HPP
#define QVRSERVICE_CLIENT_HPP

#include <stdint.h>
#include <sched.h>

/**************************************************************************//**
* Configuration Parameters
******************************************************************************/

/**************************************************************************//**
* QVRSERVICE_VRMODE_STATE
* -----------------------------------------------------------------------------
*   VRMODE_UNSUPPORTED
*      VR Mode is unsupported on this device.
*   VRMODE_STARTING
*      VR Mode is starting. The state will automatically transition to
*      VRMODE_STARTED once startup is complete.
*   VRMODE_STARTED
*      VR Mode is started/active. While in this state, only the client that
*      started VR Mode can modify the VR Mode state.
*   VRMODE_STOPPING
*      VR Mode is stopping. The state will automatically transition to
*      VRMODE_STOPPED once complete.
*   VRMODE_STOPPED
*      VR Mode is stopped/inactive. While in this state, any client can modify
*      the VR Mode state.
******************************************************************************/
typedef enum {
    VRMODE_UNSUPPORTED = 0,
    VRMODE_STARTING,
    VRMODE_STARTED,
    VRMODE_STOPPING,
    VRMODE_STOPPED,
} QVRSERVICE_VRMODE_STATE;

/**************************************************************************//**
* QVRSERVICE_TRACKING_MODE
* -----------------------------------------------------------------------------
*   TRACKING_MODE_NONE
*      Tracking is disabled. Calls to GetSensorTrackingData() will fail.
*   TRACKING_MODE_ROTATIONAL
*      Rotational mode provides tracking information using 3 degrees of freedom,
*      i.e. "what direction am I facing?". When this mode is used, the rotation
*      quaternion in the qvrservice_sensor_tracking_data_t structure will be
*      filled in when GetSensorTrackingData() is called.
*   TRACKING_MODE_POSITIONAL
*      Positional mode provides tracking information using 6 degrees of freedom,
*      i.e. "what direction am I facing, and where am I relative to my starting
*      point?" When this mode is used, both the rotation quaternion and
*      translation vector in the qvrservice_sensor_tracking_data_t structure
*      will be filled in when GetSensorTrackingData() is called.
******************************************************************************/
typedef enum {
    TRACKING_MODE_NONE = 0,
    TRACKING_MODE_ROTATIONAL,
    TRACKING_MODE_POSITIONAL,
} QVRSERVICE_TRACKING_MODE;

/**************************************************************************//**
* qvrservice_sensor_data_raw_t
* -----------------------------------------------------------------------------
* This structure contains raw sensor values in the following format:
*   gts:        Timestamp of the gyroscope data
*   ats:        Timestamp of the accelerometer data
*   gx, gy, gz: Gyroscope X, Y, and Z
*   ax, ay, az: Accelerometer X, Y, and Z
*   reserved:   Reserved for future use
******************************************************************************/
typedef struct {
    uint64_t gts;
    uint64_t ats;
    float gx;
    float gy;
    float gz;
    float ax;
    float ay;
    float az;
    uint8_t reserved[24];
} qvrservice_sensor_data_raw_t;

/**************************************************************************//**
* qvrservice_sensor_tracking_data_t
* -----------------------------------------------------------------------------
* This structure contains a quaternion (x,y,z,w) representing rotational pose
* and position vector representing translational pose. ts is the timestamp of
* the corresponding sensor value originating from the sensor stack.
* The cofficients may be used for computing a forward-predicted pose.
******************************************************************************/
typedef struct {
    float rotation[4];
    float translation[3];
    uint64_t ts;
    float prediction_coff_s[3];
    float prediction_coff_b[3];
    float prediction_coff_bdt[3];
    float prediction_coff_bdt2[3];
} qvrservice_sensor_tracking_data_t;

/**************************************************************************//**
* QVRSERVICE_DISP_INTERRUPT_ID
* -----------------------------------------------------------------------------
*   DISP_INTERRUPT_VSYNC
*      This is the display VSYNC signal. It fires at the beginning of every
*      frame.
*      Interrupt config data: pointer to qvrservice_vsync_interrupt_config_t
*   DISP_INTERRUPT_LINEPTR
*      This interrupt can be configured to interrupt after a line of data has
*      been transmitted to the display.
*      Interrupt config data: pointer to qvrservice_lineptr_interrupt_config_t
******************************************************************************/
typedef enum {
    DISP_INTERRUPT_VSYNC = 0,
    DISP_INTERRUPT_LINEPTR,
    DISP_INTERRUPT_MAX
} QVRSERVICE_DISP_INTERRUPT_ID;

/**************************************************************************//**
* disp_interrupt_callback_fn
* -----------------------------------------------------------------------------
* Callback for handling a display interrupt
*    pCtx:    The context passed in to SetDisplayInterruptConfig()
*    ts:      The timestamp of the hardware interrupt
******************************************************************************/
typedef void (*disp_interrupt_callback_fn)(void *pCtx, uint64_t ts);

/**************************************************************************//**
* qvrservice_vsync_interrupt_config_t
* -----------------------------------------------------------------------------
* This structure is used to configure vsync interrupts
*    cb:      Callback to call when interrupt occurs. Set to NULL to disable
*             interrupts.
*    ctx:     Context passed to callback
******************************************************************************/
typedef struct {
    disp_interrupt_callback_fn cb;
    void *ctx;
} qvrservice_vsync_interrupt_config_t;

/**************************************************************************//**
* qvrservice_lineptr_interrupt_config_t
* -----------------------------------------------------------------------------
* This structure is used to configure lineptr interrupts
*    cb:      Callback to call when interrupt occurs. Set to NULL to disable
*             interrupts.
*    ctx:     Context passed to callback
*    line:    Line number to interrupt on. Valid values are 1 to N, where N is
*             the width of the display (in pixels)
******************************************************************************/
typedef struct {
    disp_interrupt_callback_fn cb;
    void *ctx;
    uint32_t line;
} qvrservice_lineptr_interrupt_config_t;

/**************************************************************************//**
* QVRSERVICE_CLIENT_STATUS
* -----------------------------------------------------------------------------
*   STATUS_DISCONNECTED
*      The client was unexpectedly disconnected from server. If this occurs,
*      the QVRServiceClient object must be deleted.
*   STATUS_STATE_CHANGED
*      The VR Mode state has changed. arg1 will contain the new state,
*      arg2 will contain the previous state.
*   STATUS_SENSOR_ERROR
*      The sensor stack has detected an error. The arg parameters will be set
*      to useful values to help identify the error (TBD).
******************************************************************************/
typedef enum {
    STATUS_DISCONNECTED = 0,
    STATUS_STATE_CHANGED,
    STATUS_SENSOR_ERROR,
    STATUS_MAX = 0xffffffff
} QVRSERVICE_CLIENT_STATUS;

/**************************************************************************//**
* client_status_callback
* -----------------------------------------------------------------------------
* Callback for handling a client status event
*    pCtx:    [in] The context passed in to SetClientStatusCallback()
*    status:  [in] Status value specifying the reason for the callback
*    arg1:    [in] Argument 1 (depends on status)
*    arg2:    [in] Argument 2 (depends on status)
******************************************************************************/
typedef void (*client_status_callback_fn)(void *pCtx,
    QVRSERVICE_CLIENT_STATUS status, uint32_t arg1, uint32_t arg2);

/**************************************************************************//**
* Client APIs to communicate with QVRService. Typical call flow is as follows:
*   1. Create QVRServiceClient object
*   2. Call GetVRMode() to verify VR mode is supported and in the STOPPED state
*   3. Call SetClientStatusCallback() to get notified of status events
*   4. Call GetTrackingMode()/SetTrackinMode() to configure tracking
*   5. Call SetDisplayInterruptConfig() to handle display interrupts
*   6. Call StartVRMode() to start VR mode
*       - Handle display interrupt events
*       - Call GetSensorTrackingData() to read latest tracking data
*   7. Call StopVRMode() when to end VR mode
*   8. Delete QVRServiceClient object
******************************************************************************/


class QVRServiceClient {

public:
    QVRServiceClient();
    ~QVRServiceClient();

    /**********************************************************************//**
    * SetClientStatusCallback()
    * -------------------------------------------------------------------------
    * \param
    *    cb:       [in] Callback function to be called to handle status events
    *    pCtx:     [in] Context to be passed to callback function
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function may be called at any time. The client will maintain only
    *    one callback, so subsequent calls to this function will overwrite any
    *    previous callbacks set. cb may be set to NULL to disable status
    *    callbacks.
    * Notes
    *    None
    **************************************************************************/
    int32_t SetClientStatusCallback(client_status_callback_fn cb, void *pCtx);

    /**********************************************************************//**
    * GetVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    None
    * \return
    *    Current VR Mode state
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See QVRSERVICE_VRMODE_STATE for more info
    **************************************************************************/
    QVRSERVICE_VRMODE_STATE GetVRMode();

    /**********************************************************************//**
    * StartVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    None
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function may be called at any time if the VR Mode is in the
    *    VRMODE_STOPPED state. Calling this function while the VR Mode is in
    *    any other state will return an error.
    * Notes
    *    The caller should not assume that VR Mode configuration (e.g. calls
    *    to SetParam(), SetDisplayInterruptConfig()) will persist through
    *    start/stop cycles. Therefore it is recommended to always reconfigure
    *    VR Mode to suit the caller's use case prior to or after calling
    *    StartVRMode(). See QVRSERVICE_VRMODE_STATE for more info.
    **************************************************************************/
    int32_t StartVRMode();

    /**********************************************************************//**
    * StopVRMode()
    * -------------------------------------------------------------------------
    * \param
    *    None
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See QVRSERVICE_VRMODE_STATE for more info
    **************************************************************************/
    int32_t StopVRMode();

    /***********************************************************************//**
    * GetTrackingMode()
    * --------------------------------------------------------------------------
    * \param
    *    pCurrentMode:     [out] If non-NULL, will be set to the currently
    *                      configured tracking mode
    *    pSupportedModes:  [out] If non-NULL, will be set to a bitmask
    *                      representing which tracking modes are supported by
    *                      the device.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function may be called at any time.
    * Notes
    *    None
    ***************************************************************************/
    int32_t GetTrackingMode(QVRSERVICE_TRACKING_MODE *pCurrentMode,
        uint32_t *pSupportedModes);

    /***********************************************************************//**
    * SetTrackingMode()
    * --------------------------------------------------------------------------
    * \param
    *    mode:     [in] Tracking mode to set
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function must be called prior to calling StartVRMode().
    * Notes
    *    None
    ***************************************************************************/
    int32_t SetTrackingMode(QVRSERVICE_TRACKING_MODE mode);

    /***********************************************************************//**
    * SetDisplayInterruptConfig()
    * --------------------------------------------------------------------------
    * \param
    *    id:       [in] Display interrupt to use (see
    *              QVRSERVICE_DISP_INTERRUPT_ID)
    *    pCfg:     [in] Configuration information for display interrupt
    *    cfgSize:  [in] Size of the data passed in the pCfg pointer
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    This function may be called at any time, but callbacks will only occur
    *    when VR Mode is in the VRMODE_STARTED state.
    * Notes
    *    If VR Mode is in the VRMODE_STARTED state, this function will only
    *    work if called by the same client that started VR Mode.
    ***************************************************************************/
    int32_t SetDisplayInterruptConfig(QVRSERVICE_DISP_INTERRUPT_ID id,
        void *pCfg, uint32_t cfgSize);

    /***********************************************************************//**
    * SetThreadPriority()
    * --------------------------------------------------------------------------
    * \param
    *    tid:      [in] Thread ID of the thread whose priority will be changed
    *    policy:   [in] Scheduling policy to set. Use SCHED_FIFO or SCHED_RR for
    *              real-time performance.
    *    priority: [in] Priority value. For real-time policies, the values can
    *              be in the range of 1-99 (higher the number, higher the
    *              priority).
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    See sched.h for more info.
    ***************************************************************************/
    int32_t SetThreadPriority(int tid, int policy, int priority);

    /**********************************************************************//**
    * GetParam()
    * -------------------------------------------------------------------------
    * \param
    *    pName:    [in] NUL-terminated name of the parameter length/value to
    *              retrieve. Must not be NULL.
    *    pLen:     [inout] If pValue is NULL, pLen will be filled in with the
    *              number of bytes (including the NUL terminator) required to
    *              hold the value of the parameter specified by pName. If pValue
    *              is non-NULL, pLen must point to an integer that represents
    *              the length of the buffer pointed to by pValue. pLen must not
    *              be NULL.
    *    pValue:   [in] Buffer to receive value.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    None
    **************************************************************************/
    int32_t GetParam(const char* pName, uint32_t* pLen, char* pValue);

    /**********************************************************************//**
    * SetParam()
    * -------------------------------------------------------------------------
    * \param
    *    pName:    [in] NUL-terminated name of parameter value to set. Must not
    *              be NULL.
    *    pValue:   [in] NUL-terminated value. Must not be NULL.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    Some parameters may only be able to be set when VR Mode is in the
    *    VRMODE_STOPPED state. Additionally, some parameters may only be able to
    *    be set when VR Mode is in the VRMODE_STARTED state if SetParam() is
    *    called from the same client that started VR Mode.
    * Notes
    *    None
    **************************************************************************/
    int32_t SetParam(const char* pName, const char* pValue);

    /**********************************************************************//**
    * GetSensorRawData()
    * -------------------------------------------------------------------------
    * \param
    *    ppData:   [out] Address of pointer to qvrservice_sensor_data_raw_t
    *              structure that will contain the raw data values.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    None
    **************************************************************************/
    int32_t GetSensorRawData(qvrservice_sensor_data_raw_t **ppData);

    /**********************************************************************//**
    * GetSensorTrackingData()
    * -------------------------------------------------------------------------
    * \param
    *    ppData:   [out] Address of pointer to qvrservice_sensor_tracking_data_t
    *              structure that will contain the tracking data values.
    * \return
    *    Returns 0 upon success, -1 otherwise
    * Timing requirements
    *    None. This function may be called at any time.
    * Notes
    *    None
    **************************************************************************/
    int32_t GetSensorTrackingData(qvrservice_sensor_tracking_data_t **ppData);

private:
    void* mReserved;
};

#endif /* QVRSERVICE_CLIENT_HPP */
