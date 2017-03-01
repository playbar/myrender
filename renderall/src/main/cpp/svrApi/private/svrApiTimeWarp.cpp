//=============================================================================
// FILE: svrApiTimeWarp.cpp
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <android/native_window.h>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

#include "svrCpuTimer.h"
#include "svrGpuTimer.h"
#include "svrGeometry.h"
#include "svrProfile.h"
#include "svrShader.h"
#include "svrUtil.h"
#include "svrConfig.h"

#include "private/svrApiCore.h"
#include "private/svrApiHelper.h"
#include "private/svrApiTimeWarp.h"

#if !defined( EGL_OPENGL_ES3_BIT_KHR )
#define EGL_OPENGL_ES3_BIT_KHR		0x0040
#endif

#if !defined(EGL_CONTEXT_PRIORITY_LEVEL_IMG)
#define EGL_CONTEXT_PRIORITY_LEVEL_IMG          0x3100
#define EGL_CONTEXT_PRIORITY_HIGH_IMG           0x3101
#define EGL_CONTEXT_PRIORITY_MEDIUM_IMG         0x3102
#define EGL_CONTEXT_PRIORITY_LOW_IMG            0x3103
#endif

#if !defined(GL_BINNING_CONTROL_HINT_QCOM)
#define GL_BINNING_CONTROL_HINT_QCOM            0x8FB0
#define GL_BINNING_QCOM                         0x8FB1
#define GL_VISIBILITY_OPTIMIZED_BINNING_QCOM    0x8FB2
#define GL_RENDER_DIRECT_TO_FRAMEBUFFER_QCOM    0x8FB3
#endif

#if !defined(EGL_MUTABLE_RENDER_BUFFER_BIT_KHR)
#define EGL_MUTABLE_RENDER_BUFFER_BIT_KHR       0x1000
#endif

// Render Timings
#define MAX_TIMINGS_TO_AVERAGE      100
#define NANOSECONDS_BETWEEN_AVERAGE 1000000000LL

// Use Mojing Merged-mesh,
// 0: use 4 draw call version,
// 1: use one drawcall version with CAC,
// 2: one drawcall *without* CAC
#define USE_MOJING_MERGED_MESH                  2

#if (USE_MOJING_MERGED_MESH == 0)
#define TEST_MESHTYPE_0                         1
#endif


using namespace Svr;

// TimeWarp Properties
VAR(bool, gDisableLensCorrection, false, kVariableNonpersistent);       //Override to disable any lens correction during time warp
VAR(bool, gDirectModeWarp, true, kVariableNonpersistent);               //Override to enable/disable "direct" non-binning mode for time warp rendering
VAR(bool, gSingleBufferWindow, true, kVariableNonpersistent);           //Override to enable/disable use of single buffer surface for time warp
VAR(int, gTimeWarpThreadCore, 2, kVariableNonpersistent);               //Core affinity for time warp thread (-1=system defined)
VAR(bool, gTimeWarpClearBuffer, false, kVariableNonpersistent);         //Enable/disable clearing the time warp surface before rendering
VAR(bool, gBusyWait, false, kVariableNonpersistent);                    //Enable/disable "busy" wait between eyes (on some builds use of nanosleep function can oversleep by as much as 10ms if FIFO isn't set on the sleeping thread)
VAR(bool, gEnableWarpThreadFifo, true, kVariableNonpersistent);         //Enable/disable setting SCHED_FIFO scheduling policy on the warp thread (must be true if gBusyWait is false to avoid tearing)
VAR(int, gRecenterFrames, 0, kVariableNonpersistent);                   //Number of frames to disable reprojection after sensors are recentered

// Warp Mesh
VAR(int, gWarpMeshRows, 128, kVariableNonpersistent);                   //Number of rows in the warp mesh grid
VAR(int, gWarpMeshCols, 128, kVariableNonpersistent);                   //Number of columns in the warp mesh grid
VAR(int, gWarpMeshType, 0, kVariableNonpersistent);                     //Warp meshe type: 0 = Columns (Left To Right); 1 = Columns (Right To Left); 2 = Rows (Top To Bottom); 3 = Rows (Bottom To Top)

VAR(float, gMeshIpdOffset, 0.0f, kVariableNonpersistent);               //IPD adjustment to account for dual display panels

// Stencil Mesh
VAR(bool, gStencilMeshEnabled, false, kVariableNonpersistent);          //Enable/disable stencil mesh optimization on eye buffers
VAR(float, gStencilMeshRadius, 0.95f, kVariableNonpersistent);          //Radius of stencil mesh used for eye buffers
VAR(float, gStencilMeshColorR, 0.0f, kVariableNonpersistent);           //Fill color for stencil mesh (if enabled)
VAR(float, gStencilMeshColorG, 0.0f, kVariableNonpersistent);           //Fill color for stencil mesh (if enabled)
VAR(float, gStencilMeshColorB, 0.0f, kVariableNonpersistent);           //Fill color for stencil mesh (if enabled)
VAR(float, gStencilMeshColorA, 1.0f, kVariableNonpersistent);           //Fill color for stencil mesh (if enabled)

// Lens Parameters
VAR(float, gLensOffsetX, 0.0f, kVariableNonpersistent);                 //Horizontal lens offset
VAR(float, gLensOffsetY, 0.0f, kVariableNonpersistent);                 //Vertical lens offset
VAR(float, gAspectRatio, 1.0f, kVariableNonpersistent);                 //Eye buffer aspect ratio

// Lens Distortion Polynomial: K0 + K1*r + K2*r^2 + K3*r^3 + K4*r^4 + K5*r^5 + K6*r^6
VAR(bool, gLensInverse, false, kVariableNonpersistent);
VAR(float, gLensPolyK0, 1.0f, kVariableNonpersistent);
VAR(float, gLensPolyK1, 0.0f, kVariableNonpersistent);
VAR(float, gLensPolyK2, 0.22f, kVariableNonpersistent);
VAR(float, gLensPolyK3, 0.0f, kVariableNonpersistent);
VAR(float, gLensPolyK4, 0.24f, kVariableNonpersistent);
VAR(float, gLensPolyK5, 0.0f, kVariableNonpersistent);
VAR(float, gLensPolyK6, 0.0f, kVariableNonpersistent);

// Chromatic Aberration Correction: 1.0 + K0 + K2*r^2
VAR(float, gChromaticPolyK0_R, 0.994f, kVariableNonpersistent);
VAR(float, gChromaticPolyK1_R, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK2_R, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK3_R, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK4_R, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK5_R, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK6_R, 0.0f, kVariableNonpersistent);

VAR(float, gChromaticPolyK0_G, 1.003f, kVariableNonpersistent);
VAR(float, gChromaticPolyK1_G, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK2_G, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK3_G, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK4_G, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK5_G, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK6_G, 0.0f, kVariableNonpersistent);

VAR(float, gChromaticPolyK0_B, 1.014f, kVariableNonpersistent);
VAR(float, gChromaticPolyK1_B, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK2_B, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK3_B, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK4_B, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK5_B, 0.0f, kVariableNonpersistent);
VAR(float, gChromaticPolyK6_B, 0.0f, kVariableNonpersistent);

//Logging Options
VAR(bool, gLogEyeOverSleep, false, kVariableNonpersistent);             //Enable/disable checking and logging of oversleep during eye buffer rendering
VAR(bool, gLogFrameDoubles, false, kVariableNonpersistent);             //Enables output of LogCat messages when TimeWarp frames are reused
VAR(bool, gLogMeshCreation, false, kVariableNonpersistent);             //Enables output of LogCat messages when meshes are created

// External value from config file in svrApiPredictiveSensor
#ifdef USE_QVR_SERVICE
extern int gSensorHomePosition;
#else
int gSensorHomePosition = 0;
#endif

// Extern Hidden Stencil mesh data
extern float gHiddenMeshVerts[];
extern int gNumHiddenMeshVerts;
extern unsigned int gHiddenMeshIndices[];
extern int gNumHiddenMeshIndices;

// TODO: Should this be configurable?
#define STENCIL_MESH_MAX_RADIUS 0.95f

// Warp data used by the TimeWarpThread
SvrAsyncWarpResources gWarpData;

// Convert the config file integer into an enumeration others can use
svrWarpMeshOrder gMeshOrderEnum = kMeshOrderLeftToRight;


int gFifoPriorityWarp = 98;
int gNormalPriorityWarp = 0;    // Cause they want something :)
int gUseCAC = 2;
bool gOverlayEnabled = false;
bool gbChromaEnabled = false;
svrFrameParamsInternal* gpWarpFrame = NULL;

// Need to ignore reprojection if just recentered.
// Handle here since may have frames in flight
extern int gRecenterTransition;

int SetUseCAC(int icac)
{
    gUseCAC = icac;
}


// Common local function that lives in svrApiCore.cpp
void L_SetThreadPriority(const char *pName, int policy, int priority);

#ifdef MOTION_TO_PHOTON_TEST
// Motion To Photon Test
VAR(float, gMotionToPhotonC, 10.0f, kVariableNonpersistent);            //Scale factor driving intensity of color from angular velocity
VAR(float, gMotionToPhotonAccThreshold, 0.0002f, kVariableNonpersistent);  //Minimum threshold for motion to be considered significant enough to light the display

float       gMotionToPhotonW = 0.0f;
glm::fquat  gMotionToPhotonLast;
uint64_t    gMotionToPhotonTimeStamp = 0;

#endif // MOTION_TO_PHOTON_TEST

// Each vert has the following structure
#if USE_MOJING_MERGED_MESH
#define NUM_VERT_ATTRIBS    5
#else
#define NUM_VERT_ATTRIBS    4
#endif


struct warpMeshVert
{
    float Position[2];
    float TexCoordR[2];
    float TexCoordG[2];
    float TexCoordB[2];
#if USE_MOJING_MERGED_MESH
	float EyeIndex[2];
#endif
};

enum svrMeshMode
{
    kMeshModeColumns = 0,
    kMeshModeRows
};

enum svrMeshDistortion
{
    kNoDistortion = 0,
    kBarrleDistortion
};

// These shaders live in svrApiTimeWarpShaders.cpp
extern const char svrEyeBufferSeparateVs_Mojing[];
extern const char svrEyeBufferSeparateFs_Mojing[];
extern const char svrEyeBufferSeparateVs_Mojing_NoOverlay[];
extern const char svrEyeBufferSeparateFs_Mojing_NoOverlay[];
extern const char svrEyeBufferSeparateVs_Mojing_NoCAC[];
extern const char svrEyeBufferSeparateFs_Mojing_NoCAC[];
extern const char svrEyeBufferSeparateVs_Mojing_NoOverlayAndCAC[];
extern const char svrEyeBufferSeparateFs_Mojing_NoOverlayAndCAC[];

extern const char svrBlitQuadVs[];
extern const char svrBlitQuadFs[];
extern const char svrBlitQuadFs_Image[];
extern const char svrStencilVs[];
extern const char svrStencilFs[];
extern const char warpShaderSeparateVs[];
extern const char warpShaderSeparateFs[];
extern const char warpShaderSeparateVs_NoChroma[];
extern const char warpShaderSeparateFs_NoChroma[];
extern const char warpShaderSeparateVs_Overlay[];
extern const char warpShaderSeparateFs_Overlay[];
extern const char warpShaderSeparateFs_ImageOverlay[];
extern const char warpShaderSeparateVs_Overlay_NoChroma[];
extern const char warpShaderSeparateFs_Overlay_NoChroma[];
extern const char warpShaderSeparateFs_ImageOverlay_NoChroma[];
extern const char warpShaderSingleVs[];
extern const char warpShaderSingleFs[];
extern const char warpShaderSingleVs_NoChroma[];
extern const char warpShaderSingleFs_NoChroma[];
extern const char warpShaderSingleVs_Overlay[];
extern const char warpShaderSingleFs_Overlay[];
extern const char warpShaderSingleFs_ImageOverlay[];
extern const char warpShaderSingleVs_Overlay_NoChroma[];
extern const char warpShaderSingleFs_Overlay_NoChroma[];
extern const char warpShaderSingleFs_ImageOverlay_NoChroma[];
extern const char warpShaderArrayVs[];
extern const char warpShaderArrayFs[];
extern const char warpShaderArrayVs_NoChroma[];
extern const char warpShaderArrayFs_NoChroma[];
extern const char warpShaderArrayVs_Overlay[];
extern const char warpShaderArrayFs_Overlay[];
extern const char warpShaderArrayFs_ImageOverlay[];
extern const char warpShaderArrayVs_Overlay_NoChroma[];
extern const char warpShaderArrayFs_Overlay_NoChroma[];
extern const char warpShaderArrayFs_ImageOverlay_NoChroma[];

#ifdef HIDE_STENCIL_WIP
//-----------------------------------------------------------------------------
void InitializeStencilMesh(SvrGeometry& stencilGeom, svrWhichEye whichEye)
//-----------------------------------------------------------------------------
{
    // ********************************
    // Vertices
    // ********************************
    float *pLocalVerts = new float[3 * gNumHiddenMeshVerts];
    if (pLocalVerts == NULL)
    {
        LOGE("Unable to allocate memory for %d stencil mesh vertices!", gNumHiddenMeshVerts);
        return;
    }

    float vertRadius = 0.0f;
    float vertPos[3];

    for (int whichVert = 0; whichVert < gNumHiddenMeshVerts; whichVert++)
    {
        // Where is this vert?
        vertPos[0] = gHiddenMeshVerts[3 * whichVert + 0];
        vertPos[1] = gHiddenMeshVerts[3 * whichVert + 1];
        vertPos[2] = gHiddenMeshVerts[3 * whichVert + 2];

        // What is the radius of this vert?
        vertRadius = (vertPos[0] * vertPos[0]) + (vertPos[1] * vertPos[1]) + (vertPos[2] * vertPos[2]);
        vertRadius = sqrt(vertRadius);

        // If this is one of the inside vertices then adjust it to the new radius
        if (vertRadius < STENCIL_MESH_MAX_RADIUS)
        {
            // LOGE("Stencil vertice %d has radius %0.2f => Correcting to %0.2f.", whichVert, vertRadius, gStencilMeshRadius);

            // Create a normalized vector in this direction
            if (vertRadius != 0.0f)
            {
                vertPos[0] /= vertRadius;
                vertPos[1] /= vertRadius;
                vertPos[2] /= vertRadius;
            }

            vertPos[0] *= gStencilMeshRadius;
            vertPos[1] *= gStencilMeshRadius;
            vertPos[2] *= gStencilMeshRadius;

            // This mesh has the odd reflect back in the center
            float invertAmount;
            switch (whichEye)
            {
            case kLeftEye:
                if (vertPos[0] > 1.0f)
                {
                    invertAmount = vertPos[0] - 1.0f;
                    // LOGE("Stencil Mesh X value: %0.2f => %0.2f", vertPos[0], 1.0f - invertAmount);
                    vertPos[0] = 1.0f - invertAmount;
                }
                break;

            case kRightEye:
                if (vertPos[0] < -1.0f)
                {
                    invertAmount = -1.0f - vertPos[0];
                    // LOGE("Stencil Mesh X value: %0.2f => %0.2f", vertPos[0], -1.0f + invertAmount);
                    vertPos[0] = -1.0f + invertAmount;
                }
                break;
            }

            // These are screen coordinates, so bound them
            if (vertPos[0] < -1.0f)
                vertPos[0] = -1.0f;
            if (vertPos[0] > 1.0f)
                vertPos[0] = 1.0f;

            if (vertPos[1] < -1.0f)
                vertPos[1] = -1.0f;
            if (vertPos[1] > 1.0f)
                vertPos[1] = 1.0f;
        }
        else
        {
            // LOGE("Stencil vertice %d has radius %0.2f => Greater than %0.2f so not adjusting.", whichVert, vertRadius, STENCIL_MESH_MAX_RADIUS);
        }


        pLocalVerts[3 * whichVert + 0] = vertPos[0];
        pLocalVerts[3 * whichVert + 1] = vertPos[1];
        pLocalVerts[3 * whichVert + 2] = vertPos[2];
    }

    // ********************************
    // Indices
    // ********************************
    unsigned int * pLocalIndices = new unsigned int[gNumHiddenMeshIndices];
    if (pLocalIndices == NULL)
    {
        LOGE("Unable to allocate memory for %d stencil mesh indices!", gNumHiddenMeshIndices);
        delete[] pLocalVerts;
        return;
    }

    // Indices are from an OBJ file and so the indices are NOT 0 based.
    for (int whichIndex = 0; whichIndex < gNumHiddenMeshIndices; whichIndex++)
    {
        pLocalIndices[whichIndex] = gHiddenMeshIndices[whichIndex] - 1;
    }

    // ********************************
    // Geometry
    // ********************************
    SvrProgramAttribute attribs[1] =
    {
        {
            kPosition,                  //index
            3,                          //size
            GL_FLOAT,                   //type
            false,                      //normalized
            3 * sizeof(float),			//stride
            0							//offset
        }
    };

    stencilGeom.Initialize( &attribs[0], 1,
                            pLocalIndices, gNumHiddenMeshIndices,
                            pLocalVerts, 3 * sizeof(float) * gNumHiddenMeshVerts, gNumHiddenMeshVerts);

    // Clean up memory
    delete[] pLocalVerts;
    delete[] pLocalIndices;
}
#endif // HIDE_STENCIL_WIP

#ifdef HIDE_STENCIL_WIP
//-----------------------------------------------------------------------------
void RenderStencilMesh(SvrAsyncWarpResources *pWarpData, svrEyeBufferType eyeBufferType, svrWhichEye whichEye)
//-----------------------------------------------------------------------------
{
    Svr::SvrShader *pCurrentShader = &pWarpData->stencilShader;

    if (pCurrentShader->GetShaderId() == 0)
    {
        // Shader has not been set up
        LOGE("Unable to render stencil mesh: Stencil Shader NOT initialized!");
        return;
    }

    if (glIsProgram(pCurrentShader->GetShaderId()) == GL_FALSE)
    {
        // Shader is not a valid program
        LOGE("Unable to render stencil mesh: Stencil Shader NOT valid!");
        return;
    }

    // What is the depth range of the current target
    float DepthRange[2];
    glGetFloatv(GL_DEPTH_RANGE, DepthRange);
    // LOGE("Current Depth Range: [%0.2f, %0.2f]", DepthRange[0], DepthRange[1]);

    // This assumes the correct eye buffer has been bound with viewport and scissor set

    // Bind the stencil shader...
    pCurrentShader->Bind();

    // ... set uniforms ...
    glm::vec4 scaleoffset = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    pCurrentShader->SetUniformVec4(1, scaleoffset);

    glm::vec4 depthValue = glm::vec4(DepthRange[0], DepthRange[0], DepthRange[0], DepthRange[0]);
    pCurrentShader->SetUniformVec4(2, depthValue);

    glm::vec4 stencilColor = glm::vec4(gStencilMeshColorR, gStencilMeshColorG, gStencilMeshColorB, gStencilMeshColorA);
    pCurrentShader->SetUniformVec4(3, stencilColor);

    // ... no textures ...

    // ... render the stencil mesh
    switch (whichEye)
    {
    case kLeftEye:
        pWarpData->stencilMeshGeomLeft.Submit();
        break;

    case kRightEye:
        pWarpData->stencilMeshGeomRight.Submit();
        break;
    }

    // No longer want to use the shader
    pCurrentShader->Unbind();
}
#endif // HIDE_STENCIL_WIP

#ifdef HIDE_STENCIL_WIP
//-----------------------------------------------------------------------------
void svrBeginEye(svrEyeBufferType eyeBufferType, svrWhichEye whichEye)
//-----------------------------------------------------------------------------
{
    if (gAppContext == NULL || gAppContext->inVrMode == false)
    {
        LOGE("svrBeginEye Failed: Called when not in VR mode!");
        return;
    }

    SvrAsyncWarpResources* pWarpData = &gWarpData;

    if (gStencilMeshEnabled && pWarpData->stencilShader.GetShaderId() == 0)
    {
        // This needs to be created on this thread so the render context is correct!
        InitializeStencilMesh(pWarpData->stencilMeshGeomLeft, kLeftEye);
        InitializeStencilMesh(pWarpData->stencilMeshGeomRight, kRightEye);

        LOGE("Initializing stencilShader");
        if (!pWarpData->stencilShader.Initialize(svrStencilVs, svrStencilFs, "svrStencilVs", "svrStencilFs"))
        {
            LOGE("Failed to initialize stencilShader");
            return;
        }
    }


    if (gStencilMeshEnabled)
    {
        RenderStencilMesh(&gWarpData, eyeBufferType, whichEye);
    }
}
#endif // HIDE_STENCIL_WIP

#ifdef HIDE_STENCIL_WIP
//-----------------------------------------------------------------------------
void svrEndEye(svrEyeBufferType eyeBufferType, svrWhichEye whichEye)
//-----------------------------------------------------------------------------
{
    if (gAppContext == NULL || gAppContext->inVrMode == false)
    {
        LOGE("svrEndEye Failed: Called when not in VR mode!");
        return;
    }
}
#endif // HIDE_STENCIL_WIP

//-----------------------------------------------------------------------------
bool svrUpdateEyeContextSurface()
//-----------------------------------------------------------------------------
{
    EGLDisplay eyeDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    //First we need to bind the eye calling threads context to a small pubffer surface so the
    //warp context can take over the window surface
    gAppContext->modeContext->eyeRenderContext = eglGetCurrentContext();
    LOGI("   eyeRenderContext : %x", (unsigned int)gAppContext->modeContext->eyeRenderContext);

    gAppContext->modeContext->eyeRenderWarpSurface = eglGetCurrentSurface(EGL_DRAW);

    EGLint largestPBuffer = -1;
    eglQuerySurface(eyeDisplay, gAppContext->modeContext->eyeRenderWarpSurface, EGL_LARGEST_PBUFFER, &largestPBuffer);

    //Can't find any other way to directly query if the current surface for the eye buffer context is a pbuffer other
    //than checking the largest pbuffer attribute.  If the value is unmodified then the surface is a window surface
    //otherwise a pbuffer is already current and we don't need to do anything else.
    if (largestPBuffer == -1)
    {
        //Save the original surface the application created for the window and its configuration
        gAppContext->modeContext->eyeRenderOrigSurface = eglGetCurrentSurface(EGL_DRAW);
        LOGI("   eyeRenderSurface : %x", (unsigned int)gAppContext->modeContext->eyeRenderOrigSurface);
        eglQuerySurface(eyeDisplay, gAppContext->modeContext->eyeRenderOrigSurface, EGL_CONFIG_ID,
            &gAppContext->modeContext->eyeRenderOrigConfigId);


        EGLConfig pbufferEyeSurfaceConfig = 0;

        EGLint configId;
        eglQueryContext(eyeDisplay, gAppContext->modeContext->eyeRenderContext, EGL_CONFIG_ID, &configId);

        EGLint numConfigs;
        EGLConfig configs[1024];
        eglGetConfigs(eyeDisplay, configs, 1024, &numConfigs);
        int i = 0;
        for (; i < numConfigs; i++)
        {
            EGLint tmpConfigId;
            eglGetConfigAttrib(eyeDisplay, configs[i], EGL_CONFIG_ID, &tmpConfigId);
            if (tmpConfigId == configId)
            {
                pbufferEyeSurfaceConfig = configs[i];
            }
        }
        LOGI("Warp Context Replace Config ID : %d", i);

        const EGLint surfaceAttribs[] =
        {
            EGL_WIDTH, 16,
            EGL_HEIGHT, 16,
            EGL_NONE
        };

        gAppContext->modeContext->eyeRenderWarpSurface = eglCreatePbufferSurface(eyeDisplay, pbufferEyeSurfaceConfig, surfaceAttribs);
        if (gAppContext->modeContext->eyeRenderWarpSurface == EGL_NO_SURFACE)
        {
            LOGE("svrUpdateEyeContextSurface: Failed to create eye context pbuffer surface");
            return false;
        }

        if (eglMakeCurrent(eyeDisplay, gAppContext->modeContext->eyeRenderWarpSurface,
            gAppContext->modeContext->eyeRenderWarpSurface, gAppContext->modeContext->eyeRenderContext) == EGL_FALSE)
        {
            LOGE("svrUpdateEyeContextSurface: eglMakeCurrent failed for eye context");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
bool svrCreateWarpContext()
//-----------------------------------------------------------------------------
{
    EGLint      format;
    EGLSurface  surface;
    EGLContext  context;
    EGLConfig   config = 0;

    svrDeviceInfo di = svrGetDeviceInfo();
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    //Find an appropriate config for our warp context
    EGLConfig configs[512];
    EGLint numConfigs = 0;

    eglGetConfigs(display, NULL, 0, &numConfigs);
    eglGetConfigs(display, configs, 512, &numConfigs);

    //If we are running on Android-M (23) then we can't mutate the surface that was built for the eye buffer context
    //to a single buffer surface so we have to destroy the surface that was bound to the window and create a new surface.
    //If we are running on Android-N (24) or greater then just use the existing surface and mutate it.
    if (true)//di.deviceOSVersion < 24)
    {
        const EGLint attribs[] = {
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 0,
            EGL_NONE
        };

        if (gAppContext->modeContext->eyeRenderOrigSurface != EGL_NO_SURFACE)
        {
            //Since the existing surface that was created for the eye buffer context is bound to the
            //OS window we won't be able to bind another surface to the window without first destroying the
            //original
            eglDestroySurface(display, gAppContext->modeContext->eyeRenderOrigSurface);
            gAppContext->modeContext->eyeRenderOrigSurface = EGL_NO_SURFACE;
        }

        //Find an appropriate surface configuration
        int configId;
        for (configId = 0; configId < numConfigs; configId++)
        {
            EGLint value = 0;

            eglGetConfigAttrib(display, configs[configId], EGL_RENDERABLE_TYPE, &value);
            if ((value & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR)
            {
                continue;
            }

            eglGetConfigAttrib(display, configs[configId], EGL_SURFACE_TYPE, &value);
            if ((value & (EGL_WINDOW_BIT)) != (EGL_WINDOW_BIT))
            {
                continue;
            }

            int	j = 0;
            for (; attribs[j] != EGL_NONE; j += 2)
            {
                eglGetConfigAttrib(display, configs[configId], attribs[j], &value);
                if (value != attribs[j + 1])
                {
                    break;
                }
            }
            if (attribs[j] == EGL_NONE)
            {
                config = configs[configId];
                break;
            }
        }

        if (config == 0)
        {
            LOGE("svrCreateWarpContext: Failed to find suitable EGL config");
            return -1;
        }

        //Create the new surface
        EGLint renderBufferType = EGL_BACK_BUFFER;
        if (gSingleBufferWindow)
        {
            renderBufferType = EGL_SINGLE_BUFFER;
        }

        const EGLint windowAttribs[] = {
            EGL_RENDER_BUFFER, renderBufferType,
            EGL_NONE
        };

        surface = eglCreateWindowSurface(display, config, gAppContext->modeContext->nativeWindow, windowAttribs);
        if (surface == EGL_NO_SURFACE)
        {
            LOGE("svrCreateWarpContext: Failed to create EGL surface");
            return false;
        }
    }
    else
    {
        //We will use the existing surface that was created for the eye buffer context
        surface = gAppContext->modeContext->eyeRenderOrigSurface;

        //Find the EGL config for the warp context that matches what was used by the eye
        //buffer context
        for (int i=0; i < numConfigs; i++)
        {
            EGLint tmpConfigId;
            eglGetConfigAttrib(display, configs[i], EGL_CONFIG_ID, &tmpConfigId);
            if (tmpConfigId == gAppContext->modeContext->eyeRenderOrigConfigId)
            {
                config = configs[i];
                break;
            }
        }

        //Check if the incoming configuration supports GLES3
        EGLint renderableType = 0;
        eglGetConfigAttrib(display, config, EGL_RENDERABLE_TYPE, &renderableType);
        if ( (renderableType & EGL_OPENGL_ES3_BIT_KHR) == 0)
        {
            LOGE("svrCreateWarpContext: Eye Buffer Surface doesn't support GLES3");
        }

        //Log the surface channel bit depths (prefer an 888 surface with no depth)
        EGLint r, g, b, d, s;
        eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &r);
        eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &g);
        eglGetConfigAttrib(display, config, EGL_RED_SIZE, &b);
        eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &d);
        eglGetConfigAttrib(display, config, EGL_SAMPLES, &s);

        LOGI("svrCreateWarpContext: Eye Buffer Surface Format : R-%d,G-%d,B-%d,D-%d,S-%d", r, g, b, d, s);
    }

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(gAppContext->modeContext->nativeWindow, di.displayWidthPixels, di.displayHeightPixels, format);

    //Create the time warp context
    EGLint contextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_CONTEXT_PRIORITY_LEVEL_IMG, EGL_CONTEXT_PRIORITY_HIGH_IMG,
        EGL_NONE
    };

    context = eglCreateContext(display, config, gAppContext->modeContext->eyeRenderContext, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        LOGE("svrCreateWarpContext: Failed to create EGL context");
        return false;
    }

    //Check the context priority that was actually assigned to ensure we are high priority
    EGLint resultPriority;
    eglQueryContext(display, context, EGL_CONTEXT_PRIORITY_LEVEL_IMG, &resultPriority);
    switch (resultPriority)
    {
        case EGL_CONTEXT_PRIORITY_HIGH_IMG:
            LOGI("svrCreateWarpContext: context has high priority");
            break;
        case EGL_CONTEXT_PRIORITY_MEDIUM_IMG:
            LOGE("svrCreateWarpContext: context has medium priority");
            break;
        case EGL_CONTEXT_PRIORITY_LOW_IMG:
            LOGE("svrCreateWarpContext: context has low priority");
            break;
        default:
            LOGE("svrCreateWarpContext: unknown context priority");
            break;
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        LOGE("svrCreateWarpContext: eglMakeCurrent failed");
        return false;
    }

    //If we are on Android-N switch the surface to a single buffer surface
    if (gSingleBufferWindow && gAppContext->deviceInfo.deviceOSVersion >= 24)
    {
        bool bRes = eglSurfaceAttrib(display, surface, EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER);
        if (!bRes)
        {
            LOGE("svrCreateWarpContext: eglSurfaceAttrib EGL_RENDER_BUFFER to EGL_SINGLE_BUFFER failed.");
            SvrCheckEglError(__FILE__, __LINE__);
        }
    }

    //Verify that we got what we actually asked for
    EGLint acutalRenderBuffer;
    eglQueryContext(display, context, EGL_RENDER_BUFFER, &acutalRenderBuffer);
    if (gSingleBufferWindow)
    {
        if (gAppContext->deviceInfo.deviceOSVersion >= 24)
        {
            LOGI("svrCreateWarpContext: Requested mutable EGL_BACK_BUFFER");
        }
        else
        {
            LOGI("svrCreateWarpContext: Requested EGL_SINGLE_BUFFER");
        }
    }
    else
    {
        LOGI("svrCreateWarpContext: Requested EGL_BACK_BUFFER");
    }

    if (acutalRenderBuffer == EGL_SINGLE_BUFFER)
    {
        LOGI("    svrCreateWarpContext: Got EGL_SINGLE_BUFFER");
    }
    else if (acutalRenderBuffer == EGL_BACK_BUFFER)
    {
        LOGI("    svrCreateWarpContext: Got EGL_BACK_BUFFER");
    }
    else if (acutalRenderBuffer == EGL_NONE)
    {
        LOGE("    svrCreateWarpContext: Got EGL_NONE");
    }
    else
    {
        LOGE("    svrCreateWarpContext: Got Unknown");
    }


    //Grab some data from the surface and signal we've finished setting up the warp context
    eglQuerySurface(display, surface, EGL_WIDTH, &gAppContext->modeContext->warpRenderSurfaceWidth);
    eglQuerySurface(display, surface, EGL_HEIGHT, &gAppContext->modeContext->warpRenderSurfaceHeight);
    LOGI("svrCreateWarpContext: Warp Surface Dimensions ( %d x %d )", gAppContext->modeContext->warpRenderSurfaceWidth, gAppContext->modeContext->warpRenderSurfaceHeight);

    gAppContext->modeContext->warpRenderContext = context;
    gAppContext->modeContext->warpRenderSurface = surface;
    LOGI("svrCreateWarpContext: Timewarp context successfully created");

    pthread_mutex_lock(&gAppContext->modeContext->warpThreadContextMutex);
    gAppContext->modeContext->warpContextCreated = true;
    pthread_cond_signal(&gAppContext->modeContext->warpThreadContextCv);
    pthread_mutex_unlock(&gAppContext->modeContext->warpThreadContextMutex);

    return 0;
}

//-----------------------------------------------------------------------------
void svrDestroyWarpContext()
//-----------------------------------------------------------------------------
{
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    // No longe need the warp surface to be the current surface.
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if (gAppContext != NULL && gAppContext->modeContext != NULL)
    {
        eglDestroySurface(display, gAppContext->modeContext->warpRenderSurface);
        gAppContext->modeContext->warpRenderSurface = EGL_NO_SURFACE;
        eglDestroyContext(display, gAppContext->modeContext->warpRenderContext);
        gAppContext->modeContext->warpRenderContext = EGL_NO_CONTEXT;
    }
    else
    {
        LOGE("svrDestroyWarpContext called without valid application or VR mode context");
    }
}

//-----------------------------------------------------------------------------
void GenerateWarpCoords(float *pBaseUV, float *pWarpR, float *pWarpG, float *pWarpB)
//-----------------------------------------------------------------------------
{
    // Adjust the UV coordinates to account for the aspect ratio of the screen
    float aspect = (gAppContext->deviceInfo.displayWidthPixels * 0.5) / gAppContext->deviceInfo.displayHeightPixels;

    // Based on the aspect ratio, shrink the range of one axis and then adjust by half to center the image.
    // This makes sure that the image is square
    if (aspect < 1.0f)
    {
        pBaseUV[0] = (pBaseUV[0] * aspect) + (1.0f - aspect) * 0.5f;
    }
    else
    {
        // Need to adjust the vertical axis
        aspect = 1.0f / aspect;
        pBaseUV[1] = (pBaseUV[1] * aspect) + (1.0f - aspect) * 0.5f;
    }

    // ******************************************
    // Calculate distortion coordinates
    // ******************************************
    // [0, 1] => [-1, 1]
    float DistCoordX = 2.0f * pBaseUV[0] - 1.0f;
    float DistCoordY = 2.0f * pBaseUV[1] - 1.0f;

    if (gDisableLensCorrection)
    {
        pWarpR[0] = DistCoordX;
        pWarpR[1] = DistCoordY;

        pWarpG[0] = DistCoordX;
        pWarpG[1] = DistCoordY;

        pWarpB[0] = DistCoordX;
        pWarpB[1] = DistCoordY;

        return;
    }

    // Adjust origin to center of lens rather than center of screen
    DistCoordX -= gLensOffsetX;
    DistCoordY -= gLensOffsetY;

    // Adjust for aspect ratio
    DistCoordX /= gAspectRatio;
    DistCoordY /= gAspectRatio;


    // Need the radius for the distortion polynomial
    float DistSqrd = (DistCoordX * DistCoordX) + (DistCoordY * DistCoordY);
    float Radius1 = sqrt(DistSqrd);
    float Radius2 = Radius1 * Radius1;
    float Radius3 = Radius2 * Radius1;
    float Radius4 = Radius3 * Radius1;
    float Radius5 = Radius4 * Radius1;
    float Radius6 = Radius5 * Radius1;

    // ******************************************
    // Calculate distortion scale
    // ******************************************
    // Lens Distortion Polynomial: K0 + K1*r + K2*r^2 + K3*r^3 + K4*r^4 + K5*r^5 + K6*r^6
    float DistortionScale = 0.0f;
    DistortionScale += gLensPolyK0;
    DistortionScale += gLensPolyK1 * Radius1;
    DistortionScale += gLensPolyK2 * Radius2;
    DistortionScale += gLensPolyK3 * Radius3;
    DistortionScale += gLensPolyK4 * Radius4;
    DistortionScale += gLensPolyK5 * Radius5;
    DistortionScale += gLensPolyK6 * Radius6;

    if (gLensInverse && DistortionScale != 0.0f)
        DistortionScale = 1.0f / DistortionScale;

    // ******************************************
    // Calculate Chromatic Adjustment
    // ******************************************
    float ChromaScale[3] = { 1.0f, 1.0f, 1.0f };

    // Chromatic Aberration Correction: K0 + K1*r + K2*r^2 + K3*r^3 + K4*r^4
    ChromaScale[0] = DistortionScale * (gChromaticPolyK0_R +
                                        gChromaticPolyK1_R * Radius1 +
                                        gChromaticPolyK2_R * Radius2 +
                                        gChromaticPolyK3_R * Radius3 +
                                        gChromaticPolyK4_R * Radius4 +
                                        gChromaticPolyK5_R * Radius5 +
                                        gChromaticPolyK6_R * Radius6);      // Red
    ChromaScale[1] = DistortionScale * (gChromaticPolyK0_G +
                                        gChromaticPolyK1_G * Radius1 +
                                        gChromaticPolyK2_G * Radius2 +
                                        gChromaticPolyK3_G * Radius3 +
                                        gChromaticPolyK4_G * Radius4 +
                                        gChromaticPolyK5_G * Radius5 +
                                        gChromaticPolyK6_G * Radius6);      // Green
    ChromaScale[2] = DistortionScale * (gChromaticPolyK0_B +
                                        gChromaticPolyK1_B * Radius1 +
                                        gChromaticPolyK2_B * Radius2 +
                                        gChromaticPolyK3_B * Radius3 +
                                        gChromaticPolyK4_B * Radius4 +
                                        gChromaticPolyK5_B * Radius5 +
                                        gChromaticPolyK6_B * Radius6);      // Blue

    // ******************************************
    // Scale distortion by scale
    // ******************************************
    float ChromaCoordX[3];
    float ChromaCoordY[3];

    for (int WhichCoord = 0; WhichCoord < 3; WhichCoord++)
    {
        ChromaCoordX[WhichCoord] = ChromaScale[WhichCoord] * DistCoordX;
        ChromaCoordY[WhichCoord] = ChromaScale[WhichCoord] * DistCoordY;
    }

    // ******************************************
    // Undistort coordinates
    // ******************************************
    float ActualCoordX[3];
    float ActualCoordY[3];
    for (int WhichCoord = 0; WhichCoord < 3; WhichCoord++)
    {
        // Adjust for aspect ratio
        ActualCoordX[WhichCoord] = ChromaCoordX[WhichCoord] * gAspectRatio;
        ActualCoordY[WhichCoord] = ChromaCoordY[WhichCoord] * gAspectRatio;

        // Adjust origin to center of lens rather than center of screen
        ActualCoordX[WhichCoord] += gLensOffsetX;
        ActualCoordY[WhichCoord] += gLensOffsetY;

        //We want to leave the UVs in NDC space for timewarp
        //The timewarp perspective matrix will put the UV coordinates
        //back to [0,1]
    }

    // ******************************************
    // Set the output values
    // ******************************************
    pWarpR[0] = ActualCoordX[0];
    pWarpR[1] = ActualCoordY[0];

    pWarpG[0] = ActualCoordX[1];
    pWarpG[1] = ActualCoordY[1];

    pWarpB[0] = ActualCoordX[2];
    pWarpB[1] = ActualCoordY[2];
}

//-----------------------------------------------------------------------------
bool IsVertInRange(warpMeshVert vertData)
//-----------------------------------------------------------------------------
{
    // Make these a little bigger so the actual mesh will be slightly
    // larger than we need (can now see edges if you push into goggles)
    float MinUV = -1.1f;
    float MaxUV = 1.1f;

    if (vertData.TexCoordR[0] >= MinUV && vertData.TexCoordR[0] <= MaxUV &&
        vertData.TexCoordR[1] >= MinUV && vertData.TexCoordR[1] <= MaxUV &&
        vertData.TexCoordG[0] >= MinUV && vertData.TexCoordG[0] <= MaxUV &&
        vertData.TexCoordG[1] >= MinUV && vertData.TexCoordG[1] <= MaxUV &&
        vertData.TexCoordB[0] >= MinUV && vertData.TexCoordB[0] <= MaxUV &&
        vertData.TexCoordB[1] >= MinUV && vertData.TexCoordB[1] <= MaxUV)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
void InitializeBlitMesh(SvrGeometry& blitGeom)
//-----------------------------------------------------------------------------
{
    struct quadVert
    {
        float x, y, u, v;
    };

    static quadVert quadVertices[4] =
    {
        { -1.0f, -1.0f, 0.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f, 1.0f, },
        { 1.0f, -1.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f }
    };

    static unsigned int quadIndices[6] =
    {
        0, 2, 1, 1, 2, 3
    };

    SvrProgramAttribute attribs[2] =
    {
        {
            kPosition,                  //index
            2,                          //size
            GL_FLOAT,                   //type
            false,                      //normalized
            sizeof(quadVert),			//stride
            0							//offset
        },
        {
            kTexcoord0,                 //index
            2,                          //size
            GL_FLOAT,                   //type
            false,                      //normalized
            sizeof(quadVert),			//stride
            8							//offset
        }
    };

    blitGeom.Initialize(&attribs[0], 2,
        &quadIndices[0], 6,
        &quadVertices[0], sizeof(quadVert)* 4, 4);
}

//-----------------------------------------------------------------------------
void AddOneQuad(unsigned int currentIndex, unsigned int *indexData, warpMeshVert *vertData, unsigned int numIndices, unsigned int numRows, unsigned int numCols)
//-----------------------------------------------------------------------------
{
    unsigned int A = 0;
    unsigned int B = 0;
    unsigned int C = 0;

    // First Triangle
    A = currentIndex;
    B = currentIndex + 1;
    C = currentIndex + (numCols + 1);
    if (IsVertInRange(vertData[A]) && IsVertInRange(vertData[B]) && IsVertInRange(vertData[C]))
    {
        // All the verts have valid UV values, add this triangle to the list
        indexData[numIndices++] = A;
        indexData[numIndices++] = B;
        indexData[numIndices++] = C;

        if (gLogMeshCreation)
        {
            LOGI("Adding Triangle: (%d, %d, %d)", A, B, C);
        }
    }
    else if (gLogMeshCreation)
    {
        LOGI("Triangle Rejected: (%d, %d, %d)", A, B, C);
    }

    // Second Triangle
    A = currentIndex + 1;
    B = currentIndex + 1 + (numCols + 1);
    C = currentIndex + (numCols + 1);
    if (IsVertInRange(vertData[A]) && IsVertInRange(vertData[B]) && IsVertInRange(vertData[C]))
    {
        // All the verts have valid UV values, add this triangle to the list
        indexData[numIndices++] = A;
        indexData[numIndices++] = B;
        indexData[numIndices++] = C;

        if (gLogMeshCreation)
        {
            LOGI("Adding Triangle: (%d, %d, %d)", A, B, C);
        }
    }
    else if (gLogMeshCreation)
    {
        LOGI("Triangle Rejected: (%d, %d, %d)", A, B, C);
    }

}

//-----------------------------------------------------------------------------
void InitializeWarpMesh(int eye, SvrGeometry& warpGeom, unsigned int numRows, unsigned int numCols, float *pMinPos, float *pMaxPos, float *pMinUV, float *pMaxUV, svrMeshMode meshMode, svrMeshDistortion distortType)
//-----------------------------------------------------------------------------
{
    // Make sure we are at least 2x2
    if (numRows < 2)
        numRows = 2;
    if (numCols < 2)
        numCols = 2;

    unsigned int numVerts = (numRows + 1) * (numCols + 1);
    LOGI("Creating TimeWarp Render Mesh: %d verts in a (%d rows x %d columns) grid", numVerts, numRows, numCols);

    // Allocate memory for the vertex data
    warpMeshVert *vertData = new warpMeshVert[numVerts];
    if (vertData == NULL)
    {
        LOGE("Unable to allocate memory for render mesh!");
        return;
    }
    memset(vertData, 0, numVerts * sizeof(warpMeshVert));

    float deltaX = (pMaxPos[0] - pMinPos[0]) / (float)numCols;
    float deltaY = (pMaxPos[1] - pMinPos[1]) / (float)numRows;

    float deltaU = (pMaxUV[0] - pMinUV[0]) / (float)numCols;
    float deltaV = (pMaxUV[1] - pMinUV[1]) / (float)numRows;

    bool oldDistortFlag = gDisableLensCorrection;

    // ********************************
    // Fill in the vertex data
    // ********************************
    float baseUV[2];
    for (unsigned int whichX = 0; whichX < (numCols + 1); whichX++)
    {
        for (unsigned int whichY = 0; whichY < (numRows + 1); whichY++)
        {
            // Based on the mesh type we want to do columns or rows
            unsigned int whichVert = whichX * (numRows + 1) + whichY;   // Columns
            // unsigned int whichVert = whichY * (numCols + 1) + whichX;   // Rows

            vertData[whichVert].Position[0] = pMinPos[0] + deltaX * whichX;
            vertData[whichVert].Position[1] = pMinPos[1] + deltaY * whichY;

            baseUV[0] = pMinUV[0] + deltaU * whichX;
            baseUV[1] = pMinUV[1] + deltaV * whichY;

            switch (distortType)
            {
            case kNoDistortion:
            default:
                // Don't want distortion on these, but don't want to change the flag
                gDisableLensCorrection = true;
                GenerateWarpCoords(baseUV, vertData[whichVert].TexCoordR, vertData[whichVert].TexCoordG, vertData[whichVert].TexCoordB);
                gDisableLensCorrection = oldDistortFlag;
                break;

            case kBarrleDistortion:
                // Warp the base texture coordinates based on lens properties
                GenerateWarpCoords(baseUV, vertData[whichVert].TexCoordR, vertData[whichVert].TexCoordG, vertData[whichVert].TexCoordB);
                break;
            }

        }   // Each Column
    }   // Each Row

    if (gLogMeshCreation)
    {
        for (unsigned int whichVert = 0; whichVert < numVerts; whichVert++)
        {
            LOGI("Vert %d of %d: Pos = (%0.5f, %0.5f); UV = (%0.5f, %0.5f);", whichVert + 1, numVerts, vertData[whichVert].Position[0], vertData[whichVert].Position[1], vertData[whichVert].TexCoordR[0], vertData[whichVert].TexCoordR[1]);
        }
        LOGI("****************************************");
        LOGI("****************************************");
    }

    // ********************************
    // Generate the indices for triangles
    // ********************************
    // Each row has numCols quads, each quad is two triangles, each triangle is three indices
    unsigned int maxIndices = numRows * numCols * 2 * 3;
    unsigned int numIndices = 0;

    // Allocate memory for the index data
    unsigned int *indexData = new unsigned int[maxIndices];
    if (indexData == NULL)
    {
        LOGE("Unable to allocate memory for render mesh indices!");
        delete [] vertData;
        return;
    }
    memset(indexData, 0, maxIndices * sizeof(unsigned int));

    // Fill in the index data (Notice loops don't do last ones since we look right and up)
    unsigned int currentIndex = 0;

    switch (meshMode)
    {
    // ****************************************************
    case kMeshModeColumns:
    // ****************************************************
        if (gMeshOrderEnum == kMeshOrderLeftToRight)
        {
            for (unsigned int whichX = 0; whichX < numCols; whichX++)
            {
                for (unsigned int whichY = 0; whichY < numRows; whichY++)
                {
                    // What index is the bottom left corner of this quad
                    currentIndex = whichX * (numRows + 1) + whichY;   // Columns
                    // currentIndex = whichY * (numCols + 1) + whichX;   // Rows

                    AddOneQuad(currentIndex, indexData, vertData, numIndices, numRows, numCols);
                    numIndices += 6;

                }   // Each Column
            }   // Each Row
        }   // Columns Left To Right
        else if (gMeshOrderEnum == kMeshOrderRightToLeft) // Columns Right To Left
        {
            for (int whichX = numCols - 1; whichX >= 0; whichX--)
            {
                for (unsigned int whichY = 0; whichY < numRows; whichY++)
                {
                    // What index is the bottom left corner of this quad
                    currentIndex = whichX * (numRows + 1) + whichY;   // Columns
                    // currentIndex = whichY * (numCols + 1) + whichX;   // Rows

                    AddOneQuad(currentIndex, indexData, vertData, numIndices, numRows, numCols);
                    numIndices += 6;

                }   // Each Column
            }   // Each Row
        }   // Columns Left To Right
        break;

    // ****************************************************
    case kMeshModeRows:
    // ****************************************************
        if (gMeshOrderEnum == kMeshOrderTopToBottom)
        {
            for (int whichY = numRows - 1; whichY >= 0; whichY--)
            {
                for (unsigned int whichX = 0; whichX < numCols; whichX++)
                {
                    // What index is the bottom left corner of this quad
                    currentIndex = whichX * (numRows + 1) + whichY;   // Columns
                    // currentIndex = whichY * (numCols + 1) + whichX;   // Rows

                    AddOneQuad(currentIndex, indexData, vertData, numIndices, numRows, numCols);
                    numIndices += 6;

                }   // Each Column
            }   // Each Row
        }   // Rows Top to Bottom
        else if (gMeshOrderEnum == kMeshOrderBottomToTop)
        {
            for (unsigned int whichY = 0; whichY < numRows; whichY++)
            {
                for (unsigned int whichX = 0; whichX < numCols; whichX++)
                {
                    // What index is the bottom left corner of this quad
                    currentIndex = whichX * (numRows + 1) + whichY;   // Columns
                    // currentIndex = whichY * (numCols + 1) + whichX;   // Rows

                    AddOneQuad(currentIndex, indexData, vertData, numIndices, numRows, numCols);
                    numIndices += 6;

                }   // Each Column
            }   // Each Row
        }   // Rows Bottom to Top
        break;
    }

    LOGI("Warp mesh has %d indices (out of maximum of %d)", numIndices, maxIndices);

    // Need to create the layout attributes for the data.
    // Must fit these in to reserved types:
    //      kPosition  => Position
    //      kNormal    => TexCoordR
    //      kColor     => TexCoordG
    //      kTexcoord0 => TexCoordB
    //      kTexcoord1 => Not Used
    SvrProgramAttribute attribs[NUM_VERT_ATTRIBS] =
    {
        //  Index       Size    Type        Normalized      Stride              Offset
        {   kPosition,  2,      GL_FLOAT,   false,          sizeof(warpMeshVert),   0       },
        {   kNormal,    2,      GL_FLOAT,   false,          sizeof(warpMeshVert),   8       },
        {   kColor,     2,      GL_FLOAT,   false,          sizeof(warpMeshVert),   16      },
        {   kTexcoord0, 2,      GL_FLOAT,   false,          sizeof(warpMeshVert),   24      }
#if USE_MOJING_MERGED_MESH
        ,{   kTexcoord1, 2,      GL_FLOAT,   false,          sizeof(warpMeshVert),   32      }
#endif
    };

    //
    if (eye == 0)
    {
        warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                            indexData, numIndices,
                            vertData, sizeof(warpMeshVert) * numVerts,
                            numVerts);
    }
    // MojingLeft
    else if ( (eye & 0x0F) == 1)
    {
    	int iEyeType = eye & 0xF0;
    	if (iEyeType == 0x10)
    	{// UP
    		warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                            &(gVertexTable->m_pIndex[gVertexTable->m_iIndexSize / 2]),  
                            gVertexTable->m_iIndexSize / 2, // ̷ӽ¶¼ԃسқ
                            gVertexTable->m_pVertexLeft, 
                            4 * gVertexTable->m_iVertexSize,
                            gVertexTable->m_iVertexSize / 10);
    	}
    	else if (iEyeType == 0x20)
    	{// Lower
    		warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                           gVertexTable->m_pIndex,  
                            gVertexTable->m_iIndexSize / 2, // ̷ӽ¶¼ԃسқ
                            gVertexTable->m_pVertexLeft, 
                            4 * gVertexTable->m_iVertexSize,
							gVertexTable->m_iVertexSize / 10);
    	}
    	else
        {// Full
        	warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                            gVertexTable->m_pIndex,  gVertexTable->m_iIndexSize, // ̷ӽ¶¼ԃسқ
                            gVertexTable->m_pVertexLeft, 4 * gVertexTable->m_iVertexSize,
							gVertexTable->m_iVertexSize / 10);
			LOGI("Set MojingMesh to LEFT DONE");
		}
    }
    // MojingRight
    else if ( (eye & 0x0F) == 2)
    {
    	int iEyeType = eye & 0xF0;
        if (iEyeType == 0x10)
    	{// UP
    		LOGI("Set MojingMesh to TOP-RIGHT");
			
			warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                            &(gVertexTable->m_pIndex[gVertexTable->m_iIndexSize / 2]),  
                            gVertexTable->m_iIndexSize / 2,
                            gVertexTable->m_pVertexRight, 
                            4 * gVertexTable->m_iVertexSize,
							gVertexTable->m_iVertexSize / 10);
    	}
    	else if (iEyeType == 0x20)
    	{// Lower
    		warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                           gVertexTable->m_pIndex,  
                            gVertexTable->m_iIndexSize / 2,
                            gVertexTable->m_pVertexRight, 
                            4 * gVertexTable->m_iVertexSize,
							gVertexTable->m_iVertexSize / 10);
    	}
    	else
        {// Full
        	warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                           gVertexTable->m_pIndex,  gVertexTable->m_iIndexSize, 
                            gVertexTable->m_pVertexRight, 4 * gVertexTable->m_iVertexSize,
							gVertexTable->m_iVertexSize / 10);
        }
    }
    else if ((eye & 0x0F) == 0x03)
    {// 垂直区分的分区 
    	int iEyeType = eye & 0xF0;
    	if (iEyeType == 0x10 || iEyeType == 0x20 )
    	{// UP
			LOGI("Set MojingMesh to %s", (iEyeType == 0x10) ?  "UP" : "LOW");
    		int iAttributeSize = 10;
    		int iAttributeSizeNew = 10;
    		int iVertexLines = (int)sqrt(gVertexTable->m_iVertexSize / iAttributeSize);
    		
    		int iVertexCells_W = iVertexLines - 1;
    		int iVertexCells_H = iVertexCells_W / 2;
    		
    		int iVertexLines_W = iVertexCells_W + 1;
    		int iVertexLines_H = iVertexCells_H + 1;
    		    		
    		int iIndexCount = 2 * iVertexCells_W * iVertexCells_H * 6;
    		int iVertexCount = 2 * iVertexLines_W * iVertexLines_H;
			
			int iSrcOffset =  iEyeType == 0x10 ? (iVertexLines_H - 1)* iVertexLines_W : 0;
			int iDestOffset = 0;
			unsigned int *pIndex = new unsigned int[iIndexCount];
			float *pVertex = new float[iVertexCount * iAttributeSizeNew];
			// Copy Left Eye    		
			for (int iY = 0 ; iY < iVertexLines_H ; iY++)
			{
				int iLineOffset = iY * iVertexLines_W;
				for (int iX = 0 ; iX < iVertexLines_W ; iX++)
				{
					int iOrder = iLineOffset + iX;
					int iOrderSrc = (iOrder  + iSrcOffset) * iAttributeSize;
					int iOrderNew = (iOrder + iDestOffset)* iAttributeSizeNew;
					memcpy(pVertex + iOrderNew , 
							gVertexTable->m_pVertexLeft + iOrderSrc , 
							sizeof(float) * iAttributeSize);
//  					pVertex[iOrderNew + 8] = 0.0f;
//  					pVertex[iOrderNew + 9] = 0.0f;
				}
			}
			// Copy Right Eye
			iDestOffset = iVertexLines_H * iVertexLines_W;
			for (int iY = 0 ; iY < iVertexLines_H ; iY++)
			{
				int iLineOffset = iY * iVertexLines_W ;
				for (int iX = 0 ; iX < iVertexLines_W ; iX++)
				{
					int iOrder = iLineOffset + iX;
					int iOrderSrc = (iOrder + iSrcOffset) * iAttributeSize;
					int iOrderNew = (iOrder + iDestOffset)* iAttributeSizeNew;
					memcpy(pVertex + iOrderNew , 
							gVertexTable->m_pVertexRight + iOrderSrc , 
							sizeof(float) * iAttributeSize);
//  					pVertex[iOrderNew + 8] = 1.0f;
//  					pVertex[iOrderNew + 9] = 0.0f;
				}
			}
    		// Make Index Left Eye
    		memcpy(pIndex ,gVertexTable->m_pIndex , iIndexCount / 2 * sizeof(int));
    		int iIndex = iIndexCount / 2;
    		while (iIndex < iIndexCount)
    		{
    			pIndex[iIndex] = pIndex[iIndex - iIndexCount / 2] + iVertexCount / 2;
    			iIndex++;
    		}

            warpGeom.Initialize(&attribs[0], NUM_VERT_ATTRIBS,
                           pIndex,
                           iIndexCount,
                           pVertex,
                           4 * iVertexCount * iAttributeSizeNew,
                           iVertexCount);

            delete[] pIndex;
            delete[] pVertex;
        }
    }

    // Clean up local memory
    delete [] vertData;
    delete [] indexData;

}

//-----------------------------------------------------------------------------
glm::mat4 CalculateWarpMatrix(glm::fquat& origPose, glm::fquat& latestPose)
//-----------------------------------------------------------------------------
{
    glm::fquat diff = origPose * glm::inverse(latestPose);
    glm::fquat invDiff = glm::inverse(diff);

    invDiff.z *= -1.0f;

    return glm::mat4_cast(invDiff);
}

//-----------------------------------------------------------------------------
glm::mat4 CalculateOverlayMatrix(svrRect rcOverlay)
//-----------------------------------------------------------------------------
{
    // This was incorrectly associated with the recommended FoV for the eye buffer.
    // This should always be 90 degrees because it is a conversion from NDC to viewport space.
    // Change it here rather than change the function call to reduce breakage in apps.


    //Project the UVs in NDC onto the far plane and convert from NDC to viewport space
    glm::mat4 retMtx , mtxScale , mtxMove;

    retMtx[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    retMtx[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    retMtx[2] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    retMtx[3] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    float fScaleX = fmax(0.000001, rcOverlay.fW);
    float fScaleY = fmax(0.000001, rcOverlay.fH);

    float fMoveX = fScaleX - 1 + rcOverlay.fX * 2;
    float fMoveY = fScaleY - 1 + rcOverlay.fY * 2;

    mtxScale[0] = glm::vec4(1.0f / fScaleX , 0.0f, 0.0f, 0.0f);
    mtxScale[1] = glm::vec4(0.0f, 1.0f / fScaleY, 0.0f, 0.0f);
    mtxScale[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    mtxScale[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    retMtx = mtxScale * retMtx;

    mtxMove[0] = glm::vec4(1.0f, 0.0f, 0.0f, -fMoveX);
    mtxMove[1] = glm::vec4(0.0f, 1.0f, 0.0f, fMoveY);
    mtxMove[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    mtxMove[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


    retMtx = mtxMove * retMtx;

    return glm::transpose(retMtx);
}

//-----------------------------------------------------------------------------
glm::mat4 CalculateProjectionMatrix(float fovRad)
//-----------------------------------------------------------------------------
{
    // This was incorrectly associated with the recommended FoV for the eye buffer.
    // This should always be 90 degrees because it is a conversion from NDC to viewport space.
    // Change it here rather than change the function call to reduce breakage in apps.
    fovRad = 90.0f * DEG_TO_RAD;

    //Project the UVs in NDC onto the far plane and convert from NDC to viewport space
    glm::mat4 retMtx;
    float tanHalfFov = tanf(0.5* fovRad);

    retMtx[0] = glm::vec4(1.0f / tanHalfFov, 0.0f, 0.0f, 0.0f);
    retMtx[1] = glm::vec4(0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f);
    retMtx[2] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    retMtx[3] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    return glm::transpose(retMtx);
}

//-----------------------------------------------------------------------------
bool InitializeAsyncWarpData(SvrAsyncWarpResources* pWarpData)
//-----------------------------------------------------------------------------
{
    if( gpWarpFrame == NULL )
        return false;
    float minPos[2] = { -1.0f, -1.0f };
    float maxPos[2] = { 1.0f, 1.0f };

    float minUV[2] = { 0.0f, 0.0f };
    float maxUV[2] = { 1.0f, 1.0f };

    int undistortedRows = 2;
    int undistortedCols = 2;

    switch (gMeshOrderEnum)
    {
    case kMeshOrderLeftToRight:
    case kMeshOrderRightToLeft:
        LOGI("Creating TimeWarp Render Mesh: Left");    // Columns Left to Right [-1, 0]
        minPos[0] = -1.0f;  minPos[1] = -1.0f;
        maxPos[0] = 0.0f;  maxPos[1] = 1.0f;
        minUV[0] = 0.0f;   minUV[1] = 0.0f;
        maxUV[0] = 1.0f;   maxUV[1] = 1.0f;
        InitializeWarpMesh(1, pWarpData->warpMeshes[kMeshLeft], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kBarrleDistortion);
        InitializeWarpMesh(1, pWarpData->warpMeshes[kMeshLeft_Low], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kNoDistortion);

        minPos[0] = -1.0f;  minPos[1] = -1.0f;
        maxPos[0] = 0.0f;  maxPos[1] = 1.0f;
        minUV[0] = 1.0f;   minUV[1] = 1.0f;
        maxUV[0] = 0.0f;   maxUV[1] = 0.0f;
        InitializeWarpMesh(1, pWarpData->warpMeshes[kMeshLeft_Inv], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kBarrleDistortion);
        InitializeWarpMesh(1, pWarpData->warpMeshes[kMeshLeft_Low_Inv], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kNoDistortion);

        LOGI("Creating TimeWarp Render Mesh: Right");   // Columns Left to Right [0, 1]
        minPos[0] = 0.0f;  minPos[1] = -1.0f;
        maxPos[0] = 1.0f;  maxPos[1] = 1.0f;
        minUV[0] = 0.0f;   minUV[1] = 0.0f;
        maxUV[0] = 1.0f;   maxUV[1] = 1.0f;
        InitializeWarpMesh(2, pWarpData->warpMeshes[kMeshRight], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kBarrleDistortion);
        InitializeWarpMesh(2, pWarpData->warpMeshes[kMeshRight_Low], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kNoDistortion);

        minPos[0] = 0.0f;  minPos[1] = -1.0f;
        maxPos[0] = 1.0f;  maxPos[1] = 1.0f;
        minUV[0] = 1.0f;   minUV[1] = 1.0f;
        maxUV[0] = 0.0f;   maxUV[1] = 0.0f;
        InitializeWarpMesh(2, pWarpData->warpMeshes[kMeshRight_Inv], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kBarrleDistortion);
        InitializeWarpMesh(2, pWarpData->warpMeshes[kMeshRight_Low_Inv], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeColumns, kNoDistortion);

        break;

    case kMeshOrderTopToBottom:
    case kMeshOrderBottomToTop:
        LOGI("Creating TimeWarp Render Mesh: Upper Left");    // Rows Top to Bottom [Upper Left]
        minPos[0] = -1.0f;  minPos[1] = 0.0f;
        maxPos[0] = 0.0f;  maxPos[1] = 1.0f;
        minUV[0] = 0.0f;  minUV[1] = 0.5f;
        maxUV[0] = 1.0f;  maxUV[1] = 1.0f;
        InitializeWarpMesh(1 | 0x10, pWarpData->warpMeshes[kMeshUL], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(1 | 0x10, pWarpData->warpMeshes[kMeshUL_Low], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

        minPos[0] = -1.0f;  minPos[1] = 0.0f;
        maxPos[0] = 0.0f;  maxPos[1] = 1.0f;
        minUV[0] = 1.0f;  minUV[1] = 1.0f;
        maxUV[0] = 0.0f;  maxUV[1] = 0.5f;
        InitializeWarpMesh(1 | 0x10, pWarpData->warpMeshes[kMeshUL_Inv], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(1 | 0x10, pWarpData->warpMeshes[kMeshUL_Low_Inv], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

        LOGI("Creating TimeWarp Render Mesh: Upper Right");    // Rows Top to Bottom [Upper Right]
        minPos[0] = 0.0f;  minPos[1] = 0.0f;
        maxPos[0] = 1.0f;  maxPos[1] = 1.0f;
        minUV[0] = 0.0f;  minUV[1] = 0.5f;
        maxUV[0] = 1.0f;  maxUV[1] = 1.0f;
        InitializeWarpMesh(2 | 0x10, pWarpData->warpMeshes[kMeshUR], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(2 | 0x10, pWarpData->warpMeshes[kMeshUR_Low], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

        minPos[0] = 0.0f;  minPos[1] = 0.0f;
        maxPos[0] = 1.0f;  maxPos[1] = 1.0f;
        minUV[0] = 1.0f;  minUV[1] = 1.0f;
        maxUV[0] = 0.0f;  maxUV[1] = 0.5f;
        InitializeWarpMesh(2 | 0x10, pWarpData->warpMeshes[kMeshUR_Inv], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(2 | 0x10, pWarpData->warpMeshes[kMeshUR_Low_Inv], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

        LOGI("Creating TimeWarp Render Mesh: Lower Left");    // Rows Top to Bottom [Lower Left]
        minPos[0] = -1.0f;  minPos[1] = -1.0f;
        maxPos[0] = 0.0f;  maxPos[1] = 0.0f;
        minUV[0] = 0.0f;  minUV[1] = 0.0f;
        maxUV[0] = 1.0f;  maxUV[1] = 0.5f;
        InitializeWarpMesh(1 | 0x20, pWarpData->warpMeshes[kMeshLL], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(1 | 0x20, pWarpData->warpMeshes[kMeshLL_Low], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

        minPos[0] = -1.0f;  minPos[1] = -1.0f;
        maxPos[0] = 0.0f;  maxPos[1] = 0.0f;
        minUV[0] = 1.0f;  minUV[1] = 0.5f;
        maxUV[0] = 0.0f;  maxUV[1] = 0.0f;
        InitializeWarpMesh(1 | 0x20, pWarpData->warpMeshes[kMeshLL_Inv], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(1 | 0x20, pWarpData->warpMeshes[kMeshLL_Low_Inv], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

        LOGI("Creating TimeWarp Render Mesh: Lower Right");    // Rows Top to Bottom [Lower Right]
        minPos[0] = 0.0f;  minPos[1] = -1.0f;
        maxPos[0] = 1.0f;  maxPos[1] = 0.0f;
        minUV[0] = 0.0f;  minUV[1] = 0.0f;
        maxUV[0] = 1.0f;  maxUV[1] = 0.5f;
        InitializeWarpMesh(2 | 0x20, pWarpData->warpMeshes[kMeshLR], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(2 | 0x20, pWarpData->warpMeshes[kMeshLR_Low], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

        minPos[0] = 0.0f;  minPos[1] = -1.0f;
        maxPos[0] = 1.0f;  maxPos[1] = 0.0f;
        minUV[0] = 1.0f;  minUV[1] = 0.5f;
        maxUV[0] = 0.0f;  maxUV[1] = 0.0f;
        InitializeWarpMesh(2 | 0x20, pWarpData->warpMeshes[kMeshLR_Inv], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
        InitializeWarpMesh(2 | 0x20, pWarpData->warpMeshes[kMeshLR_Low_Inv], undistortedRows, undistortedCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kNoDistortion);

		// Create merged mesh
        LOGI("--madi-- create merged warp mesh: %d, %d", (int)kMeshU, (int)kMeshL);
		InitializeWarpMesh(3 | 0x10, pWarpData->warpMeshes[kMeshU], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);
    	InitializeWarpMesh(3 | 0x20, pWarpData->warpMeshes[kMeshL], gWarpMeshRows, gWarpMeshCols, minPos, maxPos, minUV, maxUV, kMeshModeRows, kBarrleDistortion);

        break;
    }

    ////////////////////////////
    // ********************************
    // Separate Eye Buffers
    // ********************************
    if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferMono || gpWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSeparate)
    {
        if (!gOverlayEnabled)
        {
            if( gUseCAC == 1)
            {
                // Merged mesh without overlay
                if (!pWarpData->warpShaders[kShaderSeparate_Mojing_NoOverlay].Initialize(svrEyeBufferSeparateVs_Mojing_NoOverlay, svrEyeBufferSeparateFs_Mojing_NoOverlay, "svrEyeBufferSeparateVs_Mojing_NoOverlay", "svrEyeBufferSeparateFs_Mojing_NoOverlay"))
                {
                    LOGE("Failed to initialize warpShaderSeparate_Mojing_NoOverlay for merged mesh");
                    return false;
                }
            }
            else if( gUseCAC == 2 )
            {
                if (!pWarpData->warpShaders[kShaderSeparate_Mojing_NoOverlayAndCAC].Initialize(svrEyeBufferSeparateVs_Mojing_NoOverlayAndCAC, svrEyeBufferSeparateFs_Mojing_NoOverlayAndCAC, "svrEyeBufferSeparateVs_Mojing_NoOverlayAndCAC", "svrEyeBufferSeparateFs_Mojing_NoOverlayAndCAC"))
                {
                    LOGE("Failed to initialize warpShaderSeparate_Mojing_NoOverlayAndCAC for merged mesh");
                    return false;
                }
            }
            else
            {
                if( gbChromaEnabled)
                {
                    if (!pWarpData->warpShaders[kShaderSeparate].Initialize(warpShaderSeparateVs, warpShaderSeparateFs, "warpShaderSeparateVs", "warpShaderSeparateFs"))
                    {
                        LOGE("Failed to initialize warpShaderSeparate");
                        return false;
                    }
                }
                else
                {
                    if (!pWarpData->warpShaders[kShaderSeparate_NoChroma].Initialize(warpShaderSeparateVs_NoChroma, warpShaderSeparateFs_NoChroma, "warpShaderSeparateVs_NoChroma", "warpShaderSeparateFs_NoChroma"))
                    {
                        LOGE("Failed to initialize warpShaderSeparate_NoChroma");
                        return false;
                    }
                }
            }
        }
        else if (gOverlayEnabled)
        {
#if (USE_MOJING_MERGED_MESH == 1)
            //LOGI("--madi-- use drawcall-merged-WITH-overlay shader");
//            pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing];
            if (!pWarpData->warpShaders[kShaderSeparate_Mojing].Initialize(svrEyeBufferSeparateVs_Mojing, svrEyeBufferSeparateFs_Mojing, "svrEyeBufferSeparateVs_Mojing", "svrEyeBufferSeparateFs_Mojing"))
            {
                LOGE("Failed to initialize warpShaderSeparate_Mojing for merged mesh");
                return false;
            }
#elif (USE_MOJING_MERGED_MESH == 2)
//            pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing_NoCAC];
            if (!pWarpData->warpShaders[kShaderSeparate_Mojing_NoCAC].Initialize(svrEyeBufferSeparateVs_Mojing_NoCAC, svrEyeBufferSeparateFs_Mojing_NoCAC, "svrEyeBufferSeparateVs_Mojing_NoCAC", "svrEyeBufferSeparateFs_Mojing_NoCAC"))
            {
                LOGE("Failed to initialize warpShaderSeparate_Mojing_NoOverlay for merged mesh");
                return false;
            }
#else
            if (gpWarpFrame->frameParams.overlayFormat == kOverlayImage && gpWarpFrame->frameParams.overlayType != kOverlayLayers)
            {
//                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSeparate_ImageOverlay : kShaderSeparate_ImageOverlay_NoChroma];
                if( gbChromaEnabled)
                {
                    if (!pWarpData->warpShaders[kShaderSeparate_ImageOverlay].Initialize(warpShaderSeparateVs_Overlay, warpShaderSeparateFs_ImageOverlay, "warpShaderSeparateVs_ImageOverlay", "warpShaderSeparateFs_ImageOverlay"))
                    {
                        LOGE("Failed to initialize kShaderSeparate_ImageOverlay");
                        return false;
                    }
                }
                else
                {
                    if (!pWarpData->warpShaders[kShaderSeparate_ImageOverlay_NoChroma].Initialize(warpShaderSeparateVs_Overlay_NoChroma, warpShaderSeparateFs_ImageOverlay_NoChroma, "warpShaderSeparateVs_ImageOverlay_NoChroma", "warpShaderSeparateFs_ImageOverlay_NoChroma"))
                    {
                        LOGE("Failed to initialize kShaderSeparate_ImageOverlay_NoChroma");
                        return false;
                    }
                }
            }
            else
            {
//                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSeparate_Overlay : kShaderSeparate_Overlay_NoChroma];
                if( gbChromaEnabled )
                {
                    if (!pWarpData->warpShaders[kShaderSeparate_Overlay].Initialize(warpShaderSeparateVs_Overlay, warpShaderSeparateFs_Overlay, "warpShaderSeparateVs_Overlay", "warpShaderSeparateFs_Overlay"))
                    {
                        LOGE("Failed to initialize kShaderSeparate_Overlay");
                        return false;
                    }
                }
                else
                {
                    if (!pWarpData->warpShaders[kShaderSeparate_Overlay_NoChroma].Initialize(warpShaderSeparateVs_Overlay_NoChroma, warpShaderSeparateFs_Overlay_NoChroma, "warpShaderSeparateVs_Overlay_NoChroma", "warpShaderSeparateFs_Overlay_NoChroma"))
                    {
                        LOGE("Failed to initialize kShaderSeparate_Overlay_NoChroma");
                        return false;
                    }
                }
            }
#endif


        }
    }

        // ********************************
        // Single Eye Buffer
        // ********************************
    else if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSingle)
    {
        if (!gOverlayEnabled)
        {
//            pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSingle : kShaderSingle_NoChroma];
            if( gbChromaEnabled )
            {
                if (!pWarpData->warpShaders[kShaderSingle].Initialize(warpShaderSingleVs, warpShaderSingleFs, "warpShaderSingleVs", "warpShaderSingleFs"))
                {
                    LOGE("Failed to initialize warpShaderSingle");
                    return false;
                }
            }
            else
            {
                if (!pWarpData->warpShaders[kShaderSingle_NoChroma].Initialize(warpShaderSingleVs_NoChroma, warpShaderSingleFs_NoChroma, "warpShaderSingleVs_NoChroma", "warpShaderSingleFs_NoChroma"))
                {
                    LOGE("Failed to initialize warpShaderSingle_NoChroma");
                    return false;
                }
            }
        }

        else if (gOverlayEnabled)
        {
            if (gpWarpFrame->frameParams.overlayFormat == kOverlayImage && gpWarpFrame->frameParams.overlayType != kOverlayLayers)
            {
//                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSingle_ImageOverlay : kShaderSingle_ImageOverlay_NoChroma];
                if( gbChromaEnabled)
                {
                    if (!pWarpData->warpShaders[kShaderSingle_ImageOverlay].Initialize(warpShaderSingleVs_Overlay, warpShaderSingleFs_ImageOverlay, "warpShaderSingleVs_ImageOverlay", "warpShaderSingleFs_ImageOverlay"))
                    {
                        LOGE("Failed to initialize kShaderSingle_ImageOverlay");
                        return false;
                    }
                }
                else
                {
                    if (!pWarpData->warpShaders[kShaderSingle_ImageOverlay_NoChroma].Initialize(warpShaderSingleVs_Overlay_NoChroma, warpShaderSingleFs_ImageOverlay_NoChroma, "warpShaderSingleVs_ImageOverlay_NoChroma", "warpShaderSingleFs_ImageOverlay_NoChroma"))
                    {
                        LOGE("Failed to initialize kShaderSingle_ImageOverlay_NoChroma");
                        return false;
                    }
                }
            }
            else
            {
//                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSingle_Overlay : kShaderSingle_Overlay_NoChroma];
                if( gbChromaEnabled )
                {
                    if (!pWarpData->warpShaders[kShaderSingle_Overlay].Initialize(warpShaderSingleVs_Overlay, warpShaderSingleFs_Overlay, "warpShaderSingleVs_Overlay", "warpShaderSingleFs_Overlay"))
                    {
                        LOGE("Failed to initialize kShaderSingle_Overlay");
                        return false;
                    }
                }
                else
                {
                    if (!pWarpData->warpShaders[kShaderSingle_Overlay_NoChroma].Initialize(warpShaderSingleVs_Overlay_NoChroma, warpShaderSingleFs_Overlay_NoChroma, "warpShaderSingleVs_Overlay_NoChroma", "warpShaderSingleFs_Overlay_NoChroma"))
                    {
                        LOGE("Failed to initialize kShaderSingle_Overlay_NoChroma");
                        return false;
                    }
                }
            }
        }
    }

        // ********************************
        // Eye Buffer Array
        // ********************************
    else if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferArray)
    {
        if (!gOverlayEnabled)
        {
//            pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderArray : kShaderArray_NoChroma];
            if( gbChromaEnabled)
            {
                if (!pWarpData->warpShaders[kShaderArray].Initialize(warpShaderArrayVs, warpShaderArrayFs, "warpShaderArrayVs", "warpShaderArrayFs"))
                {
                    LOGE("Failed to initialize warpShaderArray");
                    return false;
                }
            }
            else
            {
                //no shader       kShaderArray_NoChroma
            }
        }


        else if (gOverlayEnabled)
        {
            if (gpWarpFrame->frameParams.overlayFormat == kOverlayImage && gpWarpFrame->frameParams.overlayType != kOverlayLayers)
            {
//                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderArray_ImageOverlay : kShaderArray_ImageOverlay_NoChroma];
                if( gbChromaEnabled)
                {
                    if (!pWarpData->warpShaders[kShaderArray_ImageOverlay].Initialize(warpShaderArrayVs_Overlay, warpShaderArrayFs_ImageOverlay, "warpShaderArrayVs_ImageOverlay", "warpShaderArrayFs_ImageOverlay"))
                    {
                        LOGE("Failed to initialize kShaderArray_ImageOverlay");
                        return false;
                    }
                }
                else
                {
                   // no shader  kShaderArray_ImageOverlay_NoChroma
                }
            }
            else
            {
//                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderArray_Overlay : kShaderArray_Overlay_NoChroma];
                if( gbChromaEnabled )
                {
                    if (!pWarpData->warpShaders[kShaderArray_Overlay].Initialize(warpShaderArrayVs_Overlay, warpShaderArrayFs_Overlay, "warpShaderArrayVs_Overlay", "warpShaderArrayFs_Overlay"))
                    {
                        LOGE("Failed to initialize kShaderArray_Overlay");
                        return false;
                    }
                }
                else
                {
                    // no shader  kShaderArray_Overlay_NoChroma
                }
            }
        }
    }
    else
    {
        LOGE("Invalid State!  No shader determined.  Defaulting to separate eye buffers");
//        pCurrentShader = &warpData.warpShaders[kShaderSeparate];
        if (!pWarpData->warpShaders[kShaderSeparate].Initialize(warpShaderSeparateVs, warpShaderSeparateFs, "warpShaderSeparateVs", "warpShaderSeparateFs"))
        {
            LOGE("Failed to initialize warpShaderSeparate");
            return false;
        }
    }


//
//    ////////////////////////////////////
//    //Separate Eye Buffers from separate textures
//    if (!pWarpData->warpShaders[kShaderSeparate].Initialize(warpShaderSeparateVs, warpShaderSeparateFs, "warpShaderSeparateVs", "warpShaderSeparateFs"))
//    {
//        LOGE("Failed to initialize warpShaderSeparate");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSeparate_NoChroma].Initialize(warpShaderSeparateVs_NoChroma, warpShaderSeparateFs_NoChroma, "warpShaderSeparateVs_NoChroma", "warpShaderSeparateFs_NoChroma"))
//    {
//        LOGE("Failed to initialize warpShaderSeparate_NoChroma");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSeparate_Overlay].Initialize(warpShaderSeparateVs_Overlay, warpShaderSeparateFs_Overlay, "warpShaderSeparateVs_Overlay", "warpShaderSeparateFs_Overlay"))
//    {
//        LOGE("Failed to initialize kShaderSeparate_Overlay");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSeparate_Overlay_NoChroma].Initialize(warpShaderSeparateVs_Overlay_NoChroma, warpShaderSeparateFs_Overlay_NoChroma, "warpShaderSeparateVs_Overlay_NoChroma", "warpShaderSeparateFs_Overlay_NoChroma"))
//    {
//        LOGE("Failed to initialize kShaderSeparate_Overlay_NoChroma");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSeparate_ImageOverlay].Initialize(warpShaderSeparateVs_Overlay, warpShaderSeparateFs_ImageOverlay, "warpShaderSeparateVs_ImageOverlay", "warpShaderSeparateFs_ImageOverlay"))
//    {
//        LOGE("Failed to initialize kShaderSeparate_ImageOverlay");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSeparate_ImageOverlay_NoChroma].Initialize(warpShaderSeparateVs_Overlay_NoChroma, warpShaderSeparateFs_ImageOverlay_NoChroma, "warpShaderSeparateVs_ImageOverlay_NoChroma", "warpShaderSeparateFs_ImageOverlay_NoChroma"))
//    {
//        LOGE("Failed to initialize kShaderSeparate_ImageOverlay_NoChroma");
//        return false;
//    }
//
//    //Single double wide eye buffer in a single separate texture
//    if (!pWarpData->warpShaders[kShaderSingle].Initialize(warpShaderSingleVs, warpShaderSingleFs, "warpShaderSingleVs", "warpShaderSingleFs"))
//    {
//        LOGE("Failed to initialize warpShaderSingle");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSingle_NoChroma].Initialize(warpShaderSingleVs_NoChroma, warpShaderSingleFs_NoChroma, "warpShaderSingleVs_NoChroma", "warpShaderSingleFs_NoChroma"))
//    {
//        LOGE("Failed to initialize warpShaderSingle_NoChroma");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSingle_Overlay].Initialize(warpShaderSingleVs_Overlay, warpShaderSingleFs_Overlay, "warpShaderSingleVs_Overlay", "warpShaderSingleFs_Overlay"))
//    {
//        LOGE("Failed to initialize kShaderSingle_Overlay");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSingle_Overlay_NoChroma].Initialize(warpShaderSingleVs_Overlay_NoChroma, warpShaderSingleFs_Overlay_NoChroma, "warpShaderSingleVs_Overlay_NoChroma", "warpShaderSingleFs_Overlay_NoChroma"))
//    {
//        LOGE("Failed to initialize kShaderSingle_Overlay_NoChroma");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSingle_ImageOverlay].Initialize(warpShaderSingleVs_Overlay, warpShaderSingleFs_ImageOverlay, "warpShaderSingleVs_ImageOverlay", "warpShaderSingleFs_ImageOverlay"))
//    {
//        LOGE("Failed to initialize kShaderSingle_ImageOverlay");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderSingle_ImageOverlay_NoChroma].Initialize(warpShaderSingleVs_Overlay_NoChroma, warpShaderSingleFs_ImageOverlay_NoChroma, "warpShaderSingleVs_ImageOverlay_NoChroma", "warpShaderSingleFs_ImageOverlay_NoChroma"))
//    {
//        LOGE("Failed to initialize kShaderSingle_ImageOverlay_NoChroma");
//        return false;
//    }
//
//    //Separate Eye Buffers from separate slices of an array texture
//    if (!pWarpData->warpShaders[kShaderArray].Initialize(warpShaderArrayVs, warpShaderArrayFs, "warpShaderArrayVs", "warpShaderArrayFs"))
//    {
//        LOGE("Failed to initialize warpShaderArray");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderArray_Overlay].Initialize(warpShaderArrayVs_Overlay, warpShaderArrayFs_Overlay, "warpShaderArrayVs_Overlay", "warpShaderArrayFs_Overlay"))
//    {
//        LOGE("Failed to initialize kShaderArray_Overlay");
//        return false;
//    }
//
//    if (!pWarpData->warpShaders[kShaderArray_ImageOverlay].Initialize(warpShaderArrayVs_Overlay, warpShaderArrayFs_ImageOverlay, "warpShaderArrayVs_ImageOverlay", "warpShaderArrayFs_ImageOverlay"))
//    {
//        LOGE("Failed to initialize kShaderArray_ImageOverlay");
//        return false;
//    }
//
//	// Merged mesh
//	if (!pWarpData->warpShaders[kShaderSeparate_Mojing].Initialize(svrEyeBufferSeparateVs_Mojing, svrEyeBufferSeparateFs_Mojing, "svrEyeBufferSeparateVs_Mojing", "svrEyeBufferSeparateFs_Mojing"))
//    {
//        LOGE("Failed to initialize warpShaderSeparate_Mojing for merged mesh");
//        return false;
//    }
//    // Merged mesh without overlay
//    if (!pWarpData->warpShaders[kShaderSeparate_Mojing_NoOverlay].Initialize(svrEyeBufferSeparateVs_Mojing_NoOverlay, svrEyeBufferSeparateFs_Mojing_NoOverlay, "svrEyeBufferSeparateVs_Mojing_NoOverlay", "svrEyeBufferSeparateFs_Mojing_NoOverlay"))
//    {
//        LOGE("Failed to initialize warpShaderSeparate_Mojing_NoOverlay for merged mesh");
//        return false;
//    }
//    // Merged mesh no CAC
//    if (!pWarpData->warpShaders[kShaderSeparate_Mojing_NoCAC].Initialize(svrEyeBufferSeparateVs_Mojing_NoCAC, svrEyeBufferSeparateFs_Mojing_NoCAC, "svrEyeBufferSeparateVs_Mojing_NoCAC", "svrEyeBufferSeparateFs_Mojing_NoCAC"))
//    {
//        LOGE("Failed to initialize warpShaderSeparate_Mojing_NoOverlay for merged mesh");
//        return false;
//    }
//    // no overlay and cac
//    if (!pWarpData->warpShaders[kShaderSeparate_Mojing_NoOverlayAndCAC].Initialize(svrEyeBufferSeparateVs_Mojing_NoOverlayAndCAC, svrEyeBufferSeparateFs_Mojing_NoOverlayAndCAC, "svrEyeBufferSeparateVs_Mojing_NoOverlayAndCAC", "svrEyeBufferSeparateFs_Mojing_NoOverlayAndCAC"))
//    {
//        LOGE("Failed to initialize warpShaderSeparate_Mojing_NoOverlayAndCAC for merged mesh");
//        return false;
//    }

    InitializeBlitMesh(pWarpData->blitMeshGeom);

    if (!pWarpData->blitShader.Initialize(svrBlitQuadVs, svrBlitQuadFs, "svrBlitQuadVs", "svrBlitQuadFs"))
    {
        LOGE("Failed to initialize blitShader");
        return false;
    }

    if (!pWarpData->blitImageShader.Initialize(svrBlitQuadVs, svrBlitQuadFs_Image, "svrBlitQuadVs_Image", "svrBlitQuadFs_Image"))
    {
        LOGE("Failed to initialize blitShader_Image");
        return false;
    }

    return true;
}

namespace Svr{ bool g_bMeshDirty = true; }

bool UpdateAsyncWarpData(SvrAsyncWarpResources* pWarpData)
{
    bool bRet = false;
    if (g_bMeshDirty && gVertexTable != NULL)
    {
        gVertexTable->m_bUsing = true;
        bRet = InitializeAsyncWarpData(pWarpData);
        g_bMeshDirty = false;
        gVertexTable->m_bUsing = false;
    }
    return bRet;
}

//-----------------------------------------------------------------------------
void DestroyAsyncWarpData(SvrAsyncWarpResources* pWarpData)
//-----------------------------------------------------------------------------
{
    for (int whichMesh = 0; whichMesh < kNumWarpMeshAreas; whichMesh++)
    {
        pWarpData->warpMeshes[whichMesh].Destroy();
    }

    for (int whichShader = 0; whichShader < kNumWarpShaders; whichShader++)
    {
        pWarpData->warpShaders[whichShader].Destroy();
    }

    pWarpData->blitMeshGeom.Destroy();
    pWarpData->blitShader.Destroy();
    pWarpData->blitImageShader.Destroy();

    pWarpData->stencilMeshGeomLeft.Destroy();
    pWarpData->stencilMeshGeomRight.Destroy();
    pWarpData->stencilShader.Destroy();

    pWarpData->overlayTarget.Destroy();
}

//-----------------------------------------------------------------------------
void WarpSleep(uint64_t sleepTimeNano)
//-----------------------------------------------------------------------------
{
    uint64_t preSleep = GetTimeNano();

    if (!gBusyWait)
    {
        timespec t, rem;
        t.tv_sec = 0;
        t.tv_nsec = sleepTimeNano;
        nanosleep(&t, &rem);
    }
    else
    {
        while (true)
        {
            uint64_t timeNow = GetTimeNano();
            if ((timeNow - preSleep) > sleepTimeNano)
            {
                break;
            }
        }
    }

    uint64_t postSleep = GetTimeNano();

    if (gLogEyeOverSleep)
    {
        uint64_t sleepDelta = postSleep - preSleep;
        float waitDelta = (float)((int64_t)sleepDelta - (int64_t)sleepTimeNano);

        if ((waitDelta * NANOSECONDS_TO_MILLISECONDS) > 1.0f)
        {
            LOGE("WarpSleep: Overslept by %2.3f ms", waitDelta * NANOSECONDS_TO_MILLISECONDS);
        }
    }
}

#ifdef MOTION_TO_PHOTON_TEST
bool bLeftFlash = false;
bool bRightFlash = false;

float GetMtpAmount(glm::fquat latestQuat)
{
    float factor = 0.0f;

    // Compute the difference of quaternions between the current sample and the last
    glm::fquat inverseLast = glm::inverse(gMotionToPhotonLast);
    glm::fquat diffValue = latestQuat * inverseLast;
    diffValue.w = 0.0f;
    gMotionToPhotonW = glm::length(diffValue);

    //LOGI("%f (%f)", gMotionToPhotonW, oneMinusW );

    if (gMotionToPhotonAccThreshold > 0.0f)
    {
        factor = 0.0f;

        if (bRightFlash)
        {
            //During the right eye render motion was picked up so automatically
            //flash for this eye then clear the flags
            factor = 1.0f;
            bRightFlash = false;
            bLeftFlash = false;
        }
        else if (gMotionToPhotonW > gMotionToPhotonAccThreshold)
        {
            uint64_t motoPhoTimeStamp = Svr::GetTimeNano();
            if (((motoPhoTimeStamp - gMotionToPhotonTimeStamp) * NANOSECONDS_TO_MILLISECONDS) > 500.0f)
            {
                //It's been more than 1/2s since the last time we flashed
                factor = 1.0f;
                gMotionToPhotonTimeStamp = motoPhoTimeStamp;
                bLeftFlash = true;
            }
        }

    }
    else
    {
        factor = gMotionToPhotonC * gMotionToPhotonW;
        if (factor > 1.0f)
            factor = 1.0f;
    }

    gMotionToPhotonLast = latestQuat;

    return factor;
}
#endif // MOTION_TO_PHOTON_TEST

//-----------------------------------------------------------------------------
void UpdateOverlay(SvrAsyncWarpResources *pWarpData, svrFrameParamsInternal *pWarpFrame)
//-----------------------------------------------------------------------------
{
    pWarpData->overlayTarget.Bind();
    glViewport(0, 0, pWarpData->overlayTarget.GetWidth(), pWarpData->overlayTarget.GetHeight());
    glScissor(0, 0, pWarpData->overlayTarget.GetWidth(), pWarpData->overlayTarget.GetHeight());

    glClearColor(0.0f, 0.0f, 0.0f, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Doesn't currently have a depth buffer

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    float quadPos[8];
    float quadUVs[8];

    unsigned int quadIndices[6] = { 0, 2, 1, 1, 2, 3 };
    int indexCount = sizeof(quadIndices) / sizeof(unsigned int);

    glm::vec4 scaleoffset = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);

    for (int whichOverlay = 0; whichOverlay < SVR_NUM_OVERLAYS; whichOverlay++)
    {
        if (pWarpFrame->frameParams.overlayBuffer[whichOverlay] == 0)
            continue;

        // Update the attribute stream for this quad
        quadPos[0] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerLeft[0];
        quadPos[1] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerLeft[1];
        quadUVs[0] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerLeft[2];
        quadUVs[1] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerLeft[3];

        quadPos[2] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperLeft[0];
        quadPos[3] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperLeft[1];
        quadUVs[2] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperLeft[2];
        quadUVs[3] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperLeft[3];

        quadPos[4] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerRight[0];
        quadPos[5] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerRight[1];
        quadUVs[4] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerRight[2];
        quadUVs[5] = pWarpFrame->frameParams.overlayLayout[whichOverlay].LowerRight[3];

        quadPos[6] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperRight[0];
        quadPos[7] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperRight[1];
        quadUVs[6] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperRight[2];
        quadUVs[7] = pWarpFrame->frameParams.overlayLayout[whichOverlay].UpperRight[3];

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


        // Bind the blit shader...
        if (pWarpFrame->frameParams.overlayFormat == kOverlayImage)
        {
            pWarpData->blitImageShader.Bind();
        }
        else
        {
            pWarpData->blitShader.Bind();
        }

        // ... set attributes ...
        glEnableVertexAttribArray(kPosition);
        glVertexAttribPointer(kPosition, 2, GL_FLOAT, 0, 0, quadPos);

        glEnableVertexAttribArray(kTexcoord0);
        glVertexAttribPointer(kTexcoord0, 2, GL_FLOAT, 0, 0, quadUVs);

        // ... set uniforms ...
        pWarpData->blitShader.SetUniformVec4(1, scaleoffset);

        // ... set textures ...
        if (pWarpFrame->frameParams.overlayFormat == kOverlayImage)
        {
            pWarpData->blitShader.SetUniformSampler("srcTex", pWarpFrame->frameParams.overlayBuffer[whichOverlay], GL_TEXTURE_EXTERNAL_OES);
        }
        else
        {
            pWarpData->blitShader.SetUniformSampler("srcTex", pWarpFrame->frameParams.overlayBuffer[whichOverlay], GL_TEXTURE_2D);
        }

        // ... render the quad ...
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, quadIndices);

        // ... and disable attributes
        glDisableVertexAttribArray(kPosition);
        glDisableVertexAttribArray(kTexcoord0);

    }

    pWarpData->overlayTarget.Unbind();
}

enum svrSurfaceSubset
{
    kIgnore,
    kEntire,
    kLeftHalf,
    KRightHalf,
    kTopHalf,
    KBottomHalf,
    kLowerLeft,
    kUpperLeft,
    kLowerRight,
    kUpperRight
};

//-----------------------------------------------------------------------------
void L_SetSurfaceScissor(svrSurfaceSubset whichPart)
//-----------------------------------------------------------------------------
{
    switch (whichPart)
    {
    case kIgnore:
        // No Operations
        break;
    case kEntire:
        glScissor(0, 0, gAppContext->modeContext->warpRenderSurfaceWidth, gAppContext->modeContext->warpRenderSurfaceHeight);
        break;
    case kLeftHalf:
        glScissor(0, 0, gAppContext->modeContext->warpRenderSurfaceWidth / 2, gAppContext->modeContext->warpRenderSurfaceHeight);
        break;
    case KRightHalf:
        glScissor(gAppContext->modeContext->warpRenderSurfaceWidth / 2, 0, gAppContext->modeContext->warpRenderSurfaceWidth / 2, gAppContext->modeContext->warpRenderSurfaceHeight);
        break;
    case kTopHalf:
        glScissor(0, gAppContext->modeContext->warpRenderSurfaceHeight / 2, gAppContext->modeContext->warpRenderSurfaceWidth, gAppContext->modeContext->warpRenderSurfaceHeight / 2);
        break;
    case KBottomHalf:
        glScissor(0, 0, gAppContext->modeContext->warpRenderSurfaceWidth, gAppContext->modeContext->warpRenderSurfaceHeight / 2);
        break;
    case kLowerLeft:
        break;
    case kUpperLeft:
        break;
    case kLowerRight:
        break;
    case kUpperRight:
        break;
    }
}

//-----------------------------------------------------------------------------
void L_SetEyeObjects(unsigned int &leftEye, unsigned int &rightEye, unsigned int &samplerType, svrFrameParamsInternal* pWarpFrame )
//-----------------------------------------------------------------------------
{
    // ********************************
    // Set the eye buffers...
    // ********************************
    leftEye = pWarpFrame->frameParams.eyeBufferArray[0];
    rightEye = pWarpFrame->frameParams.eyeBufferArray[1];

    if (gMeshOrderEnum == kMeshOrderRightToLeft)
    {
        // Reverse the order since rendering the opposite way
        // Later: Don't want to reverse since swithing buffers is handled later
        // leftEye = pWarpFrame->frameParams.eyeBufferArray[1];
        // rightEye = pWarpFrame->frameParams.eyeBufferArray[0];
    }

    if (pWarpFrame->frameParams.eyeBufferType == kEyeBufferMono ||
        pWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSingle ||
        pWarpFrame->frameParams.eyeBufferType == kEyeBufferArray)
    {
        // Mono will duplicate the eye buffer for both eyes
        // Both of the other cases only have a single texture
        leftEye = pWarpFrame->frameParams.eyeBufferArray[0];
        rightEye = pWarpFrame->frameParams.eyeBufferArray[0];
    }

    // ********************************
    //... then the sampler type
    // ********************************
    if (pWarpFrame->frameParams.eyeBufferType == kEyeBufferMono ||
        pWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSeparate ||
        pWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSingle)
    {
        samplerType = GL_TEXTURE_2D;
    }
    else if (pWarpFrame->frameParams.eyeBufferType == kEyeBufferArray)
    {
        samplerType = GL_TEXTURE_2D_ARRAY;
    }

}

//-----------------------------------------------------------------------------
bool L_SetOverlayObjects(unsigned int &leftImage, unsigned int &rightImage, unsigned int &samplerType, svrFrameParamsInternal* pWarpFrame, SvrAsyncWarpResources &warpData)
//-----------------------------------------------------------------------------
{
    // TODO: madi, do we need to support single-eye overlay
    if (pWarpFrame->frameParams.overlayBuffer[0] == 0 && pWarpFrame->frameParams.overlayBuffer[0] == 0)
    {
        // Overlay is not enabled
        return false;
    }

    switch (pWarpFrame->frameParams.overlayType)
    {
    case kOverlayMono:
        leftImage = pWarpFrame->frameParams.overlayBuffer[0];
        rightImage = pWarpFrame->frameParams.overlayBuffer[0];
        break;

    case kOverlayStereo:
        leftImage = pWarpFrame->frameParams.overlayBuffer[0];
        rightImage = pWarpFrame->frameParams.overlayBuffer[1];
        break;

    case kOverlayLayers:
        // Render layers into our render target and then treat as "Mono"
        UpdateOverlay(&warpData, pWarpFrame);
        leftImage = warpData.overlayTarget.GetColorAttachment();
        rightImage = warpData.overlayTarget.GetColorAttachment();

        break;

    default:
        LOGE("Unknown overlay type: %d. Overlay will not be rendered!", (int)pWarpFrame->frameParams.overlayType);
        break;
    }

    switch (pWarpFrame->frameParams.overlayFormat)
    {
    case kOverlayTexture:
        samplerType = GL_TEXTURE_2D;
        break;

    case kOverlayImage:
        if (pWarpFrame->frameParams.overlayType == kOverlayLayers)
        {
            // Image textures are rendered into a normal texture
            samplerType = GL_TEXTURE_2D;
        }
        else
        {
            samplerType = GL_TEXTURE_EXTERNAL_OES;
        }
        break;

    default:
        LOGE("Unknown overlay format: %d. Overlay will not be rendered!", (int)pWarpFrame->frameParams.overlayFormat);
        return false;
        break;
    }

    // Overlay is supported and set up
    return true;
}



//-----------------------------------------------------------------------------
Svr::SvrShader * L_GetCurrentShader(svrFrameParamsInternal* pWarpFrame, SvrAsyncWarpResources &warpData, bool overlayEnabled, bool chromaEnabled)
//-----------------------------------------------------------------------------
{
    //LOGI("--madi-- Mono = %d, Separate = %d, meshType = %d, overlayEnabled = %s, chromaEnabled = %s", (int)kEyeBufferMono, (int)kEyeBufferStereoSeparate, (int)pWarpFrame->frameParams.eyeBufferType, overlayEnabled ? "true" : "false", chromaEnabled ? "true" : "false");

    Svr::SvrShader *pCurrentShader = NULL;

    // ********************************
    // Separate Eye Buffers
    // ********************************
    if (pWarpFrame->frameParams.eyeBufferType == kEyeBufferMono || pWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSeparate)
    {
        if (!overlayEnabled)
        {
            if( gUseCAC == 1)
            {
                pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing_NoOverlay];
            }
            else if( gUseCAC == 2 )
            {
                pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing_NoOverlayAndCAC];
            }
            else
            {
                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSeparate : kShaderSeparate_NoChroma];
            }
//#if (USE_MOJING_MERGED_MESH == 1)
//            //LOGI("--madi-- use drawcall-merged-WITHOUT-overlay shader");
//            pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing_NoOverlay];
//#elif (USE_MOJING_MERGED_MESH == 2)
//            pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing_NoOverlayAndCAC];
//#else
//            pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSeparate : kShaderSeparate_NoChroma];
//#endif
        }
        else if (overlayEnabled)
        {
#if (USE_MOJING_MERGED_MESH == 1)
            //LOGI("--madi-- use drawcall-merged-WITH-overlay shader");
            pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing];
#elif (USE_MOJING_MERGED_MESH == 2)
            pCurrentShader = &warpData.warpShaders[kShaderSeparate_Mojing_NoCAC];
#else
            if (pWarpFrame->frameParams.overlayFormat == kOverlayImage && pWarpFrame->frameParams.overlayType != kOverlayLayers)
            {
                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSeparate_ImageOverlay : kShaderSeparate_ImageOverlay_NoChroma];
            }
            else
            {
                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSeparate_Overlay : kShaderSeparate_Overlay_NoChroma];
            }
#endif


        }
    }

    // ********************************
    // Single Eye Buffer
    // ********************************
    else if (pWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSingle)
    {
        if (!overlayEnabled)
        {
            pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSingle : kShaderSingle_NoChroma];
        }

        else if (overlayEnabled)
        {
            if (pWarpFrame->frameParams.overlayFormat == kOverlayImage && pWarpFrame->frameParams.overlayType != kOverlayLayers)
            {
                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSingle_ImageOverlay : kShaderSingle_ImageOverlay_NoChroma];
            }
            else
            {
                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderSingle_Overlay : kShaderSingle_Overlay_NoChroma];
            }
        }
    }

    // ********************************
    // Eye Buffer Array
    // ********************************
    else if (pWarpFrame->frameParams.eyeBufferType == kEyeBufferArray)
    {
        if (!overlayEnabled)
        {
            pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderArray : kShaderArray_NoChroma];
        }


        else if (overlayEnabled)
        {
            if (pWarpFrame->frameParams.overlayFormat == kOverlayImage && pWarpFrame->frameParams.overlayType != kOverlayLayers)
            {
                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderArray_ImageOverlay : kShaderArray_ImageOverlay_NoChroma];
            }
            else
            {
                pCurrentShader = &warpData.warpShaders[chromaEnabled ? kShaderArray_Overlay : kShaderArray_Overlay_NoChroma];
            }
        }
    }

    // Did we find a shader?
    if (pCurrentShader == NULL)
    {
        LOGE("Invalid State!  No shader determined.  Defaulting to separate eye buffers");
        pCurrentShader = &warpData.warpShaders[kShaderSeparate];
    }

    return pCurrentShader;
}

struct ShaderParamStruct
{
    Svr::SvrShader *    pShader;

    glm::vec4           scaleoffset;
    glm::mat4           texWarpMatrix;
    glm::vec2           eyeBorder;
    glm::vec4           ipdoffset;
    glm::mat4           overlayWarpMatrix;
#if USE_MOJING_MERGED_MESH
    glm::mat4           overlayWarpMatrix2;
#endif
};

enum UniformOptions
{
    kEnableEyeBorder = (1 << 0),
    kEnableIpdOffset = (1 << 1)
};

//-----------------------------------------------------------------------------
void L_SetShaderUniforms(ShaderParamStruct *pParams, unsigned int flags, unsigned int leftEyeBuffer, unsigned int rightEyeBuffer, unsigned int eyeSampler, unsigned int leftOverlayBuffer, unsigned int rightOverlayBuffer, unsigned int overlaySampler)
//-----------------------------------------------------------------------------
{
    pParams->pShader->SetUniformVec4(1, pParams->scaleoffset);
    pParams->pShader->SetUniformMat4(2, pParams->texWarpMatrix);

    if (flags & kEnableEyeBorder)
    {
        pParams->pShader->SetUniformVec2(3, pParams->eyeBorder);
    }

    if (flags & kEnableIpdOffset)
    {
        pParams->pShader->SetUniformVec4(4, pParams->ipdoffset);
    }
    // LOGI("--madi-- set uniform leftEyeBuffer: %d rightEyeBuffer: %d leftOverlayBuffer: %d rightOverlayBuffer: %d",
    //     leftEyeBuffer, rightEyeBuffer, leftOverlayBuffer, rightOverlayBuffer);
    if (leftEyeBuffer != 0 && rightEyeBuffer != 0)
    {
        pParams->pShader->SetUniformSampler("srcTex", leftEyeBuffer, eyeSampler);
        pParams->pShader->SetUniformSampler("srcTex2", rightEyeBuffer, eyeSampler);
    }
    else
    {
        assert(leftEyeBuffer != 0 && rightEyeBuffer == 0 && "eyebuffer: when merged mesh not used, use the left eye buffer and leave right empty");
        pParams->pShader->SetUniformSampler("srcTex", leftEyeBuffer, eyeSampler);
    }

    if (leftOverlayBuffer != 0 && rightOverlayBuffer !=0 )
    {
        // LOGI("--madi-- set uniform overlay left: %d,right: %d", leftOverlayBuffer, rightOverlayBuffer);
        pParams->pShader->SetUniformMat4(4, pParams->overlayWarpMatrix);
        pParams->pShader->SetUniformSampler("overlayTex", leftOverlayBuffer, overlaySampler);
#if USE_MOJING_MERGED_MESH
        pParams->pShader->SetUniformMat4(5, pParams->overlayWarpMatrix2);
        pParams->pShader->SetUniformSampler("overlayTex2", rightOverlayBuffer, overlaySampler);
#endif
    }
    else
    {
//        assert(leftOverlayBuffer != 0 && rightOverlayBuffer == 0 && "overlaybuffer: when merged mesh not used, use the left overlay buffer and leave right empty");
#if USE_MOJING_MERGED_MESH
#else
        pParams->pShader->SetUniformMat4(5, pParams->overlayWarpMatrix);
        pParams->pShader->SetUniformSampler("overlayTex", leftOverlayBuffer, overlaySampler);
#endif

    }
}

//-----------------------------------------------------------------------------
void* WarpThreadMain(void* arg)
//-----------------------------------------------------------------------------
{
    unsigned int prevWarpFrameCount = 0;

    LOGI("WarpThreadMain Entered");

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    PROFILE_THREAD_NAME(gTelemetryData.context, 0, "TimeWarp");
    PROFILE_ENABLE_GROUP(GROUP_TIMEWARP, 1);

    if (gEnableWarpThreadFifo)
    {
        L_SetThreadPriority("TimeWarp Thread", SCHED_FIFO, gFifoPriorityWarp);
    }

    if (gTimeWarpThreadCore >= 0)
    {
        LOGI("Setting TimeWarp Affinity to %d", gTimeWarpThreadCore);
        svrSetThreadAffinity(gTimeWarpThreadCore);
    }

    // Want the mesh type in an enumeration
    if (gWarpMeshType == 0)
        gMeshOrderEnum = kMeshOrderLeftToRight;
    else if (gWarpMeshType == 1)
        gMeshOrderEnum = kMeshOrderRightToLeft;
    else if (gWarpMeshType == 2)
        gMeshOrderEnum = kMeshOrderTopToBottom;
    else if (gWarpMeshType == 3)
        gMeshOrderEnum = kMeshOrderBottomToTop;

#if TEST_MESHTYPE_0
    gMeshOrderEnum = kMeshOrderLeftToRight;
#endif

    // Correct to RightToLeft if orientation is LandscapeRight and set to render LeftToRight.
    // This is because the orientation sets the left eye when the right side is rastering out.
    // Later: Remove this because the Meta1 device actually is LandcapeRight but for some reason
    // the raster is LeftToRight!  Makes no sense, but there is tearing if RightToLeft!
    // if (gAppContext->deviceInfo.displayOrientation == 270 && gMeshOrderEnum == kMeshOrderLeftToRight)
    // {
    //     LOGI("Because application is orientated as Landscape-Left, setting mesh type to Right-To-Left");
    //     gMeshOrderEnum = kMeshOrderRightToLeft;
    // }

    svrCreateWarpContext();

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_SCISSOR_TEST);

    if (gDirectModeWarp)
    {
        LOGI("Rendering in direct mode");
        glEnable(GL_BINNING_CONTROL_HINT_QCOM);
        glHint(GL_BINNING_CONTROL_HINT_QCOM, GL_RENDER_DIRECT_TO_FRAMEBUFFER_QCOM);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


/*
	float MJ_CLEARCOLOR[8][4] = {
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }
		};
	int MJ_CLEAR_COLOR_INDEX = 0;

	*/	
		
    bool bHasWarpData = false;
    while (true)
    {
        LOGE("begin while, F:%s threadid=%d", __FUNCTION__, gettid());
        PROFILE_SCOPE_DEFAULT(GROUP_TIMEWARP);
		/*
		// CLEAR COLOR
		glViewport(0, 0, gAppContext->modeContext->warpRenderSurfaceWidth, gAppContext->modeContext->warpRenderSurfaceHeight);
		glScissor(0, 0, gAppContext->modeContext->warpRenderSurfaceWidth, gAppContext->modeContext->warpRenderSurfaceHeight);
		
		glClearColor(MJ_CLEARCOLOR[MJ_CLEAR_COLOR_INDEX][0], MJ_CLEARCOLOR[MJ_CLEAR_COLOR_INDEX][1], MJ_CLEARCOLOR[MJ_CLEAR_COLOR_INDEX][2], MJ_CLEARCOLOR[MJ_CLEAR_COLOR_INDEX][3]);
		glClear(GL_COLOR_BUFFER_BIT);	
		
		MJ_CLEAR_COLOR_INDEX = (MJ_CLEAR_COLOR_INDEX + 1) % 8;
		// CLEAR COLOR END
		*/

        //Check if the main thread has requested an exit
        if (gAppContext->modeContext->warpThreadExit)
        {
            LOGI("WarpThreadMain exit flag set");
            break;
        }

        //Wait until the raster is at the halfway point, find a set of eye buffers and render the left eye
        double framePeriodNano = 1e9 / gAppContext->deviceInfo.displayRefreshRateHz;
        uint64_t waitTime;

        pthread_mutex_lock(&gAppContext->modeContext->vsyncMutex);
        uint64_t timestamp = Svr::GetTimeNano();
        double framePct = (double)gAppContext->modeContext->vsyncCount + ((double)timestamp - gAppContext->modeContext->vsyncTimeNano) / (framePeriodNano);
        pthread_mutex_unlock(&gAppContext->modeContext->vsyncMutex);


        uint64_t warpVsyncCount = ceil(framePct);

        double fractFrame = framePct - ((long)framePct);
        if (fractFrame < 0.5)
        {
            //We are currently in the first half of the display so wait until we hit the halfway point
            waitTime = (uint64_t)((0.5 - fractFrame) * framePeriodNano);
        }
        else
        {
            //We are currently past the halfway point in the display so wait until halfway through the next vsync cycle
            waitTime = (uint64_t)((1.5 - fractFrame) * framePeriodNano);
        }

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Left EyeBuffer Wait");
        WarpSleep(waitTime);

        PROFILE_EXIT(GROUP_TIMEWARP); // Left EyeBuffer Wait

        uint64_t postLeftWaitTimeStamp = Svr::GetTimeNano();

        unsigned int curSubmitFrameCount = gAppContext->modeContext->submitFrameCount;

        //Get the frame parameters for the frame we will be warping
        for (int i = 0; i < NUM_SWAP_FRAMES - 1; i++)
        {
            int checkFrameCount = curSubmitFrameCount - i;

            if (checkFrameCount <= 0)
            {
                //This is the first time through and no frames have been submitted yet so bail out
                gpWarpFrame = NULL;
                break;
            }

            svrFrameParamsInternal* pCheckFrame = &gAppContext->modeContext->frameParams[checkFrameCount % NUM_SWAP_FRAMES];

            if (pCheckFrame->minVSyncCount > warpVsyncCount)
            {
            	//LOGI("minVSyncCount : %d, warpVsyncCount : %d", pCheckFrame->minVSyncCount, warpVsyncCount);
                continue;
            }

            //Check to see if the frame has already finished on the GPU
            GLenum syncResult = glClientWaitSync(pCheckFrame->frameSync, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
            if (syncResult == GL_TIMEOUT_EXPIRED)
            {
                //The current frame hasn't finished on the GPU so keep looking back for a frame that has
                //LOGI("GPU not finished with frame %d", checkFrameCount);
                continue;
            }

            gpWarpFrame = pCheckFrame;
            // madi: ChangeMojingWorld
            bHasWarpData |= UpdateAsyncWarpData(&gWarpData);
            //LOGI("set warpFrameCount = %d,  checkFrameCount: %d", gAppContext->modeContext->warpFrameCount, checkFrameCount);    
            gAppContext->modeContext->warpFrameCount = checkFrameCount;

            break;
        }


        LOGV("Warping %d [vc : %llu]", gAppContext->modeContext->warpFrameCount, gAppContext->modeContext->vsyncCount);

        //Signal the eye render thread that it can continue on
        if (!pthread_mutex_trylock(&gAppContext->modeContext->warpBufferConsumedMutex))
        {
            pthread_cond_signal(&gAppContext->modeContext->warpBufferConsumedCv);
            pthread_mutex_unlock(&gAppContext->modeContext->warpBufferConsumedMutex);
        }

        if (gpWarpFrame == NULL)
        {
            //No frame to warp (just started rendering??) so start over...
            LOGI("No valid frame to warp...");
            continue;
        }

		if (gpWarpFrame->warpSync != 0)
    	{
    		glDeleteSync(gpWarpFrame->warpSync);
    		gpWarpFrame->warpSync = 0;
		}
		/*LOGV("-- HX -- curSubmitFrameCount = %d , submitFrameCount = %d , warpFrameCount = %d , TID = %d    [SWAP - 1]" ,
			curSubmitFrameCount,
			gAppContext->modeContext->submitFrameCount,
			gAppContext->modeContext->warpFrameCount,
			gpWarpFrame->frameParams.eyeBufferArray[0]
		);*/
		
        int frameDoubled = 0;
        if (gLogFrameDoubles && gpWarpFrame->frameParams.minVsyncs == 1 && gAppContext->modeContext->warpFrameCount == prevWarpFrameCount)
        {
            LOGI("Frame doubled on %d [%llu,%llu]", gAppContext->modeContext->warpFrameCount, warpVsyncCount, gAppContext->modeContext->vsyncCount);
            frameDoubled = 1;
        }
        prevWarpFrameCount = gAppContext->modeContext->warpFrameCount;

        if (gpWarpFrame->warpFrameLeftTimeStamp == 0)
        {
            gpWarpFrame->warpFrameLeftTimeStamp = postLeftWaitTimeStamp;
        }

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Warp Submission : VSync %d Frame %d Doubled %d",
            (int)(gAppContext->modeContext->vsyncCount) % 8 + 1,
            (int)(gAppContext->modeContext->warpFrameCount) % 8 + 1,
            frameDoubled
        );

        // Set up the shader parameters
        ShaderParamStruct shaderParams;
        memset(&shaderParams, 0, sizeof(shaderParams));

        shaderParams.scaleoffset = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
        shaderParams.texWarpMatrix = glm::mat4(1.0f);
        shaderParams.eyeBorder = glm::vec2(0.0f, 1.0f);
        shaderParams.ipdoffset = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

#if USE_MOJING_MERGED_MESH
        svrRect rcOverlayRectL = gpWarpFrame->frameParams.overlayRect[0];
        svrRect rcOverlayRectR = gpWarpFrame->frameParams.overlayRect[1];
        // LOGI("--madi-- overlay rect left:%f,%f,%f,%f right:%f, %f, %f, %f",
        //     rcOverlayRectL.fX, rcOverlayRectL.fY, rcOverlayRectL.fW, rcOverlayRectL.fH,
        //     rcOverlayRectR.fX, rcOverlayRectR.fY, rcOverlayRectR.fW, rcOverlayRectR.fH
        // );
        shaderParams.overlayWarpMatrix = CalculateOverlayMatrix(rcOverlayRectL);
        shaderParams.overlayWarpMatrix2 = CalculateOverlayMatrix(rcOverlayRectR);
#else
        shaderParams.overlayWarpMatrix = CalculateProjectionMatrix(gAppContext->deviceInfo.targetFovXRad);
#endif

        bool bDistortionEnabled = !((gpWarpFrame->frameParams.frameOptions & kDisableDistortionCorrection) > 0);
        gbChromaEnabled = !((gpWarpFrame->frameParams.frameOptions & kDisableChromaticCorrection) > 0);
        //bool bTimeWarpEnabled = !((gpWarpFrame->frameParams.frameOptions & kDisableReprojection) > 0);
        bool bTimeWarpEnabled = gTimewarpEnabled;//gAppContext->m_TimewarpEnabled;
        //LOGI("--madi-- timewarp enabled: %s", bTimeWarpEnabled ? "true" : "false");

        // If distortion is disabled then there is no reason to do chromatic aberration
        if (!bDistortionEnabled)
        {
            gbChromaEnabled = false;
        }

        if (gRecenterTransition < gRecenterFrames)
        {
            // Need to disable reprojection for a few frames after recentering
            gRecenterTransition++;
			//LOGE("Reprojection Disabled for this frame (Left): %d", gRecenterTransition);
            bTimeWarpEnabled = false;
        }

        // We have a predicted time that was used to render each eye.  Look at what we predicted and
        // where we are now and adjust the warp matrix to correct.
        // This is done by CalculateWarpMatrix() so no special code is needed.
        // Check and see if the predicted time is close to where we are now.
        uint64_t timeNowNano = Svr::GetTimeNano();
        uint64_t diffTimeNano = timeNowNano - gpWarpFrame->frameParams.headPoseState.poseTimeStampNs;
        float diffTimeMs = diffTimeNano * NANOSECONDS_TO_MILLISECONDS;

        float adjustedTimeMs = 0.0f;
        if (gpWarpFrame->frameParams.headPoseState.predictedTimeMs > 0.0f && !(gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
        {
            //Predict ahead
            adjustedTimeMs = (0.5 * framePeriodNano) * 1e-6;
        }

        // If mesh type is not "Left to Right" then TimeWarp is disabled, but we need to use the latest
        // predicted time for both eyes.  The current adjusted time is only for the first half so we double
        // it to get where the second half should be
        if (gMeshOrderEnum == kMeshOrderTopToBottom || gMeshOrderEnum == kMeshOrderBottomToTop)
        {
            adjustedTimeMs *= 2.0f;
        }


        //LOGI("Left Eye Predicted %0.2f ms but it took %0.2f ms [%0.2f]", gpWarpFrame->frameParams.headPoseState.predictedTimeMs, diffTimeMs, adjustedTimeMs);
        glm::fquat origQuat, latestQuat;

        int64_t timeToLeftEye = 0L;
        int64_t timeToRightEye = 0L;

        // If this is the first submitted frame with an overlay of type kOverlayLayers then create the render target
        if (gpWarpFrame->frameParams.overlayType == kOverlayLayers && gWarpData.overlayTarget.GetFrameBufferId() == 0)
        {
            int localWidth = gAppContext->modeContext->warpRenderSurfaceWidth / 2;
            int localHeight = gAppContext->modeContext->warpRenderSurfaceHeight;

            LOGI("Creating overlay render target ( %dx%d )", localWidth, localHeight);
            gWarpData.overlayTarget.Initialize(localWidth, localHeight, 1, GL_RGBA8, false);
        }

        // TODO: Determine which mesh (Distorted or square)

        // Determine the "Left/Right" eye buffers.  "Left/Right" are really "First/Second" because
        // sometimes we are rendering left-to-right and sometimes right-to-left
        unsigned int leftEyeBuffer = gpWarpFrame->frameParams.eyeBufferArray[0];
        unsigned int rightEyeBuffer = gpWarpFrame->frameParams.eyeBufferArray[1];
        unsigned int eyeSamplerType = GL_TEXTURE_2D;
        L_SetEyeObjects(leftEyeBuffer, rightEyeBuffer, eyeSamplerType, gpWarpFrame);


        // Determine the overlay buffer
        unsigned int leftOverlayBuffer = 0;
        unsigned int rightOverlayBuffer = 0;
        unsigned int overlaySamplerType = GL_TEXTURE_2D;
        gOverlayEnabled = L_SetOverlayObjects(leftOverlayBuffer, rightOverlayBuffer, overlaySamplerType, gpWarpFrame, gWarpData);

        // Up to 4 meshes are rendered each pass.  If Left/Right order then 2 are rendered (Each half of screen)
        // If Top/Bottom order then 4 are rendered (Each corner of the screen).
        // The logic for which mesh is rendered when gets complicated so define them here.
        unsigned int mesh1A = 0;    // First part of "Left" eye
        unsigned int mesh1B = 0;    // Second part of "Left" eye
        unsigned int mesh2A = 0;    // First part of "Right" eye
        unsigned int mesh2B = 0;    // Second part of "Right" eye

        switch (gMeshOrderEnum)
        {
        case kMeshOrderLeftToRight:
            mesh1A = bDistortionEnabled ? kMeshLeft : kMeshLeft_Low;
            mesh1B = 0;
            mesh2A = bDistortionEnabled ? kMeshRight : kMeshRight_Low;
            mesh2B = 0;
            break;

        case kMeshOrderRightToLeft:
            mesh1A = bDistortionEnabled ? kMeshRight : kMeshRight_Low;
            mesh1B = 0;
            mesh2A = bDistortionEnabled ? kMeshLeft : kMeshLeft_Low;
            mesh2B = 0;
            break;

        case kMeshOrderTopToBottom:
#if USE_MOJING_MERGED_MESH
            mesh1A = kMeshU;
            mesh2A = kMeshL;
#else
            mesh1A = bDistortionEnabled ? kMeshUL : kMeshUL_Low;
            mesh1B = bDistortionEnabled ? kMeshUR : kMeshUR_Low;
            mesh2A = bDistortionEnabled ? kMeshLL : kMeshLL_Low;
            mesh2B = bDistortionEnabled ? kMeshLR : kMeshLR_Low;
#endif

            break;

        case kMeshOrderBottomToTop:
            mesh1A = bDistortionEnabled ? kMeshLL : kMeshLL_Low;
            mesh1B = bDistortionEnabled ? kMeshLR : kMeshLR_Low;
            mesh2A = bDistortionEnabled ? kMeshUL : kMeshUL_Low;
            mesh2B = bDistortionEnabled ? kMeshUR : kMeshUR_Low;
            break;
        }

        // Now if the orientation of the device does not match the orientation of the application we need
        // to use the inverted versions
        unsigned int tempSwapper = 0;
        if (gSensorHomePosition == kSensorLandscapeLeft && gAppContext->deviceInfo.displayOrientation == 270)
        {
            // This is a "Landscape Right" application but device is "Landscape Left"
            //LOGE("Landscape Right application on Landscape Left device: Inverting Eye Buffers!");
            if (gMeshOrderEnum == kMeshOrderLeftToRight)
            {
                mesh1A = (bDistortionEnabled ? kMeshLeft_Inv : kMeshLeft_Low_Inv);
                mesh1B = 0;
                mesh2A = (bDistortionEnabled ? kMeshRight_Inv : kMeshRight_Low_Inv);
                mesh2B = 0;
            }
            else
            {
                mesh1A = (bDistortionEnabled ? kMeshRight_Inv : kMeshRight_Low_Inv);
                mesh1B = 0;
                mesh2A = (bDistortionEnabled ? kMeshLeft_Inv : kMeshLeft_Low_Inv);
                mesh2B = 0;
            }

            tempSwapper = rightEyeBuffer;
            rightEyeBuffer = leftEyeBuffer;
            leftEyeBuffer = tempSwapper;

            tempSwapper = rightOverlayBuffer;
            rightOverlayBuffer = leftOverlayBuffer;
            leftOverlayBuffer = tempSwapper;
        }
        else if (gSensorHomePosition == kSensorLandscapeRight && gAppContext->deviceInfo.displayOrientation == 90)
        {
            // This is a "Landscape Left" application but device is "Landscape Right"
            // LOGE("Landscape Left application on Landscape Right device: Inverting Eye Buffers!");
            if (gMeshOrderEnum == kMeshOrderLeftToRight)
            {
                mesh1A = (bDistortionEnabled ? kMeshLeft_Inv : kMeshLeft_Low_Inv);
                mesh1B = 0;
                mesh2A = (bDistortionEnabled ? kMeshRight_Inv : kMeshRight_Low_Inv);
                mesh2B = 0;
            }
            else
            {
                mesh1A = (bDistortionEnabled ? kMeshRight_Inv : kMeshRight_Low_Inv);
                mesh1B = 0;
                mesh2A = (bDistortionEnabled ? kMeshLeft_Inv : kMeshLeft_Low_Inv);
                mesh2B = 0;
            }

            tempSwapper = rightEyeBuffer;
            rightEyeBuffer = leftEyeBuffer;
            leftEyeBuffer = tempSwapper;

            tempSwapper = rightOverlayBuffer;
            rightOverlayBuffer = leftOverlayBuffer;
            leftOverlayBuffer = tempSwapper;
        }

        // LOGI("Warping Eye Buffers: Left = %d; Right = %d", leftEyeBuffer, rightEyeBuffer);

        // TODO: Does "gAppContext->deviceInfo.displayOrientation == 0|180" and Top/Bottom mesh order require special handling?

        // Determine the current shader...
        shaderParams.pShader = L_GetCurrentShader(gpWarpFrame, gWarpData, gOverlayEnabled, gbChromaEnabled);

        // ... and then bind it
        shaderParams.pShader->Bind();

        glViewport(0, 0, gAppContext->modeContext->warpRenderSurfaceWidth, gAppContext->modeContext->warpRenderSurfaceHeight);
#if USE_MOJING_MERGED_MESH
        // madi: we set the scissor util drawing
#else
        L_SetSurfaceScissor(kEntire);
#endif


        //*******************
        //Left Eye
        //*******************
        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Left Eye - Pose Update");
        glm::mat4 leftWarpMatrix = glm::mat4(1.0);
        if (bTimeWarpEnabled)
        {
            //Get the updated sensor state
            //LOGI("TimeWarp left eye predicted time: %0.2f ms", adjustedTimeMs);
            svrHeadPoseState latestPoseState = svrGetPredictedHeadPose(adjustedTimeMs);
            origQuat = glmQuatFromSvrQuat(gpWarpFrame->frameParams.headPoseState.pose.rotation);
            latestQuat = glmQuatFromSvrQuat(latestPoseState.pose.rotation);
            leftWarpMatrix = CalculateWarpMatrix(origQuat, latestQuat);

            timeToLeftEye = latestPoseState.poseTimeStampNs - gpWarpFrame->frameParams.headPoseState.poseTimeStampNs;
        }
        shaderParams.texWarpMatrix = leftWarpMatrix * (CalculateProjectionMatrix(gAppContext->deviceInfo.targetFovXRad));

        PROFILE_EXIT(GROUP_TIMEWARP); //Left Eye - Pose Update

#ifdef MOTION_TO_PHOTON_TEST
        if (gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton)
        {
            // Compute the difference of quaternions between the current sample and the last
            glm::fquat inverseLast = glm::inverse(gMotionToPhotonLast);
            glm::fquat diffValue = latestQuat * inverseLast;
            diffValue.w = 0.0f;
            gMotionToPhotonW = glm::length(diffValue);

            //LOGI("%f (%f)", gMotionToPhotonW, oneMinusW );

            if (gMotionToPhotonAccThreshold > 0.0f)
            {
                clearColor[0] = clearColor[1] = clearColor[2] = 0.0f;

                if (bRightFlash)
                {
                    //During the right eye render motion was picked up so automatically
                    //flash for this eye then clear the flags
                    clearColor[0] = clearColor[1] = clearColor[2] = 1.0f;
                    bRightFlash = false;
                    bLeftFlash = false;
                }
                else if (gMotionToPhotonW > gMotionToPhotonAccThreshold)
                {
                    uint64_t motoPhoTimeStamp = Svr::GetTimeNano();
                    if (((motoPhoTimeStamp - gMotionToPhotonTimeStamp) * NANOSECONDS_TO_MILLISECONDS) > 500.0f)
                    {
                        //It's been more than 1/2s since the last time we flashed
                        clearColor[0] = clearColor[1] = clearColor[2] = 1.0f;
                        gMotionToPhotonTimeStamp = motoPhoTimeStamp;
                        bLeftFlash = true;
                    }
                }

            }
            else
            {
                float factor = gMotionToPhotonC * gMotionToPhotonW;
                if (factor > 1.0f)
                    factor = 1.0f;

                clearColor[0] = factor;
                clearColor[1] = factor;
                clearColor[2] = factor;
            }

            gMotionToPhotonLast = latestQuat;
        }
#endif // MOTION_TO_PHOTON_TEST

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Left Eye Submit");

        if (gTimeWarpClearBuffer)
        {
            glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);

            switch (gMeshOrderEnum)
            {
            case kMeshOrderLeftToRight:
                L_SetSurfaceScissor(kLeftHalf);
                break;

            case kMeshOrderRightToLeft:
                L_SetSurfaceScissor(KRightHalf);
                break;

            case kMeshOrderTopToBottom:
                L_SetSurfaceScissor(kTopHalf);
                break;

            case kMeshOrderBottomToTop:
                L_SetSurfaceScissor(KBottomHalf);
                break;
            }


            glClear(GL_COLOR_BUFFER_BIT);
        }

#ifdef MOTION_TO_PHOTON_TEST
        if ((gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton) && !gTimeWarpClearBuffer)
        {
            // Not already cleared above so clear now
            glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

            switch (gMeshOrderEnum)
            {
            case kMeshOrderLeftToRight:
                L_SetSurfaceScissor(kLeftHalf);
                break;

            case kMeshOrderRightToLeft:
                L_SetSurfaceScissor(KRightHalf);
                break;

            case kMeshOrderTopToBottom:
                L_SetSurfaceScissor(kTopHalf);
                break;

            case kMeshOrderBottomToTop:
                L_SetSurfaceScissor(KBottomHalf);
                break;
            }

            glClear(GL_COLOR_BUFFER_BIT);
        }
#endif // MOTION_TO_PHOTON_TEST
        // Left eye
        if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferMono ||
            gpWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSeparate)
        {
#ifdef MOTION_TO_PHOTON_TEST
            if (!(gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
            {
#endif // MOTION_TO_PHOTON_TEST

                switch (gMeshOrderEnum)
                {
                case kMeshOrderLeftToRight:
                    shaderParams.scaleoffset.z = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();
                    break;

                case kMeshOrderRightToLeft:
                    shaderParams.scaleoffset.z = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();
                    break;

                case kMeshOrderTopToBottom:
#if USE_MOJING_MERGED_MESH
                    shaderParams.scaleoffset.z = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, rightEyeBuffer, eyeSamplerType, leftOverlayBuffer, rightOverlayBuffer, overlaySamplerType);
                    L_SetSurfaceScissor(kTopHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();
#else
                    shaderParams.scaleoffset.z = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();

                    shaderParams.scaleoffset.z = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1B].Submit();
#endif
                    break;

                case kMeshOrderBottomToTop:
                    shaderParams.scaleoffset.z = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();

                    shaderParams.scaleoffset.z = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1B].Submit();
                    break;
                }

#ifdef MOTION_TO_PHOTON_TEST
            }
#endif // MOTION_TO_PHOTON_TEST

        }
        else if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSingle)
        {
            //Each eye is using half the width of the source texture
#ifdef MOTION_TO_PHOTON_TEST
            if (!(gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
            {
#endif // MOTION_TO_PHOTON_TEST

                switch (gMeshOrderEnum)
                {
                case kMeshOrderLeftToRight:
                    shaderParams.eyeBorder = glm::vec2(0.0f, 0.5f);
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();
                    break;

                case kMeshOrderRightToLeft:
                    shaderParams.eyeBorder = glm::vec2(0.5f, 1.0f);
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();
                    break;

                case kMeshOrderTopToBottom:
                    shaderParams.eyeBorder = glm::vec2(0.0f, 0.5f);
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();

                    shaderParams.eyeBorder = glm::vec2(0.5f, 1.0f);
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1B].Submit();
                    break;

                case kMeshOrderBottomToTop:
                    shaderParams.eyeBorder = glm::vec2(0.0f, 0.5f);
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();

                    shaderParams.eyeBorder = glm::vec2(0.5f, 1.0f);
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1B].Submit();
                    break;
                }

#ifdef MOTION_TO_PHOTON_TEST
            }
#endif // MOTION_TO_PHOTON_TEST

        }
        else if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferArray)
        {
            // This is the case for why following code can't be common.  We have a new offset: gMeshIpdOffset
            // This has not been tested.
#ifdef MOTION_TO_PHOTON_TEST
            if (!(gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
            {
#endif // MOTION_TO_PHOTON_TEST

                switch (gMeshOrderEnum)
                {
                case kMeshOrderLeftToRight:
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();
                    break;

                case kMeshOrderRightToLeft:
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();
                    break;

                case kMeshOrderTopToBottom:
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();

                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1B].Submit();
                    break;

                case kMeshOrderBottomToTop:
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh1A].Submit();

                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh1B].Submit();
                    break;
                }

#ifdef MOTION_TO_PHOTON_TEST
            }
#endif // MOTION_TO_PHOTON_TEST

        }

        PROFILE_EXIT(GROUP_TIMEWARP); //Left Eye Submit

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Left Eye Finalize");
        if (gSingleBufferWindow)
        {
            glFlush();

        }
        PROFILE_EXIT(GROUP_TIMEWARP); // Left Eye Finalize

        //Wait for the time point when the raster should be at the left edge of the display to render the right eye
        if (gSingleBufferWindow)
        {
            uint64_t rightTimestamp = Svr::GetTimeNano();

            PROFILE_ENTER(GROUP_TIMEWARP, 0, "Right EyeBuffer Wait");
            //We started the left eye half way through vsync, figure out how long
            //we have left in the half frame until the raster starts over so we can render the right eye
            uint64_t delta = rightTimestamp - postLeftWaitTimeStamp;
            uint64_t waitTime = 0;
            if (delta < (uint64_t)(framePeriodNano / 2.0))
            {
                waitTime = ((uint64_t)(framePeriodNano / 2.0)) - delta;
                WarpSleep(waitTime);
            }
            else
            {
                //The left eye took longer than 1/2 the refresh so the raster has already wrapped around and is
                //in the left half of the screen.  Skip the wait and get right on rendering the right eye.
                LOGE("Left Eye took too long!!! ( %2.3f ms )", delta * NANOSECONDS_TO_MILLISECONDS);
                PROFILE_MESSAGE(GROUP_TIMEWARP, 0, "Left Eye took too long!!! ( %2.3f ms )", delta * NANOSECONDS_TO_MILLISECONDS);
            }
            PROFILE_EXIT(GROUP_TIMEWARP); // Right EyeBuffer Wait
        }

        if (gpWarpFrame->warpFrameRightTimeStamp == 0)
        {
            gpWarpFrame->warpFrameRightTimeStamp = GetTimeNano();
        }

        //*******************
        //Right Eye
        //*******************
        // As the meshes are moved using gMeshIpdOffset they can stomp on each other.
        if (gMeshIpdOffset != 0.0f)
            L_SetSurfaceScissor(kEntire);

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Right Eye Pose Update");

        // Update the time warp adjustment
        timeNowNano = Svr::GetTimeNano();
        diffTimeNano = timeNowNano - gpWarpFrame->frameParams.headPoseState.poseTimeStampNs;
        diffTimeMs = (float)diffTimeNano / 1000000.0f;

        adjustedTimeMs = 0.0f;
        if (gpWarpFrame->frameParams.headPoseState.predictedTimeMs > 0.0f)
        {
            adjustedTimeMs = (0.5 * framePeriodNano) * 1e-6;
        }

#ifdef MOTION_TO_PHOTON_TEST
        //If we are performing MtP tests we don't want to use predicted sensor values
        adjustedTimeMs = 0.0f;
#endif

        //LOGI("Right Eye Predicted %0.2f ms but it took %0.2f ms [%0.2f]", gpWarpFrame->frameParams.headPoseState.predictedTimeMs, diffTimeMs, adjustedTimeMs);

        // It is possible that the recenter event came in between eyes so whe need to check again
        if (bTimeWarpEnabled && gRecenterTransition < gRecenterFrames)
        {
            // Need to disable reprojection for a few frames after recentering
            gRecenterTransition++;
            //LOGE("Reprojection Disabled for this frame (Right): %d", gRecenterTransition);
            bTimeWarpEnabled = false;
        }

        glm::mat4 rightWarpMatrix = glm::mat4(1.0);
        if (bTimeWarpEnabled)
        {
            svrHeadPoseState latestPoseState;

            if (gMeshOrderEnum == kMeshOrderTopToBottom || gMeshOrderEnum == kMeshOrderBottomToTop)
            {
                rightWarpMatrix = leftWarpMatrix;
            }
            else
            {
                //Get the updated sensor state
                // LOGI("TimeWarp right eye predicted time: %0.2f ms", adjustedTimeMs);
                latestPoseState = svrGetPredictedHeadPose(adjustedTimeMs);
                origQuat = glmQuatFromSvrQuat(gpWarpFrame->frameParams.headPoseState.pose.rotation);
                latestQuat = glmQuatFromSvrQuat(latestPoseState.pose.rotation);
                rightWarpMatrix = CalculateWarpMatrix(origQuat, latestQuat);
            }

            timeToRightEye = latestPoseState.poseTimeStampNs - gpWarpFrame->frameParams.headPoseState.poseTimeStampNs;
        }

        shaderParams.texWarpMatrix = rightWarpMatrix * (CalculateProjectionMatrix(gAppContext->deviceInfo.targetFovXRad));

        PROFILE_EXIT(GROUP_TIMEWARP); // Right Eye Pose Update

#ifdef MOTION_TO_PHOTON_TEST
        if (gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton)
        {
            // Compute the difference of quaternions between the current sample and the last
            glm::fquat inverseLast = glm::inverse(gMotionToPhotonLast);
            glm::fquat diffValue = latestQuat * inverseLast;
            diffValue.w = 0.0f;
            gMotionToPhotonW = glm::length(diffValue);

            //LOGI("%f (%f)", gMotionToPhotonW, oneMinusW );

            if (gMotionToPhotonAccThreshold > 0.0f)
            {
                clearColor[0] = clearColor[1] = clearColor[2] = 0.0f;

                if (bLeftFlash)
                {
                    //During the left eye render motion was picked up so automatically
                    //flash for this eye then clear the flags
                    clearColor[0] = clearColor[1] = clearColor[2] = 1.0f;
                    bRightFlash = false;
                    bLeftFlash = false;
                }
                else if (gMotionToPhotonW > gMotionToPhotonAccThreshold)
                {
                    uint64_t motoPhoTimeStamp = Svr::GetTimeNano();
                    if (((motoPhoTimeStamp - gMotionToPhotonTimeStamp) * NANOSECONDS_TO_MILLISECONDS) > 500.0f)
                    {
                        //It's been more than 1/2s since the last time we flashed
                        clearColor[0] = clearColor[1] = clearColor[2] = 1.0f;
                        gMotionToPhotonTimeStamp = motoPhoTimeStamp;
                        bRightFlash = true;
                    }
                }

            }
            else
            {
                float factor = gMotionToPhotonC * gMotionToPhotonW;
                if (factor > 1.0f)
                    factor = 1.0f;

                clearColor[0] = factor;
                clearColor[1] = factor;
                clearColor[2] = factor;
            }

            gMotionToPhotonLast = latestQuat;
        }
#endif // MOTION_TO_PHOTON_TEST

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Right Eye Submit");

        if (gTimeWarpClearBuffer)
        {
            glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

            switch (gMeshOrderEnum)
            {
            case kMeshOrderLeftToRight:
                L_SetSurfaceScissor(KRightHalf);
                break;

            case kMeshOrderRightToLeft:
                L_SetSurfaceScissor(kLeftHalf);
                break;

            case kMeshOrderTopToBottom:
                L_SetSurfaceScissor(KBottomHalf);
                break;

            case kMeshOrderBottomToTop:
                L_SetSurfaceScissor(kTopHalf);
                break;
            }

            glClear(GL_COLOR_BUFFER_BIT);
        }

#ifdef MOTION_TO_PHOTON_TEST
        if ((gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton) && !gTimeWarpClearBuffer)
        {
            // Not already cleared above so clear now
            glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
            L_SetSurfaceScissor(KRightHalf);

            switch (gMeshOrderEnum)
            {
            case kMeshOrderLeftToRight:
                L_SetSurfaceScissor(KRightHalf);
                break;

            case kMeshOrderRightToLeft:
                L_SetSurfaceScissor(kLeftHalf);
                break;

            case kMeshOrderTopToBottom:
                L_SetSurfaceScissor(KBottomHalf);
                break;

            case kMeshOrderBottomToTop:
                L_SetSurfaceScissor(kTopHalf);
                break;
            }

            glClear(GL_COLOR_BUFFER_BIT);
        }
#endif // MOTION_TO_PHOTON_TEST
        // Right eye
        shaderParams.scaleoffset.z = 0.0f;
        shaderParams.scaleoffset.w = 0.0f;

        if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferMono ||
            gpWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSeparate)
        {

#ifdef MOTION_TO_PHOTON_TEST
            if (!(gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
            {
#endif // MOTION_TO_PHOTON_TEST

                switch (gMeshOrderEnum)
                {
                case kMeshOrderLeftToRight:
                    shaderParams.scaleoffset.z = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();
                    break;

                case kMeshOrderRightToLeft:
                    shaderParams.scaleoffset.z = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();
                    break;

                case kMeshOrderTopToBottom:
#if USE_MOJING_MERGED_MESH
                    shaderParams.scaleoffset.z = -gMeshIpdOffset;
                    L_SetSurfaceScissor(KBottomHalf);
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, rightEyeBuffer, eyeSamplerType, leftOverlayBuffer, rightOverlayBuffer, overlaySamplerType);
                    gWarpData.warpMeshes[mesh2A].Submit(); //
#else
                    shaderParams.scaleoffset.z = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();

                    shaderParams.scaleoffset.z = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2B].Submit(); //
#endif

                    break;

                case kMeshOrderBottomToTop:
                    shaderParams.scaleoffset.z = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();

                    shaderParams.scaleoffset.z = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2B].Submit();
                    break;
                }

#ifdef MOTION_TO_PHOTON_TEST
            }
#endif // MOTION_TO_PHOTON_TEST
        }
        else if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferStereoSingle)
        {
#ifdef MOTION_TO_PHOTON_TEST
            if (!(gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
            {
#endif // MOTION_TO_PHOTON_TEST

                switch (gMeshOrderEnum)
                {
                case kMeshOrderLeftToRight:
                    shaderParams.eyeBorder = glm::vec2(0.5f, 1.0f);
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();
                    break;

                case kMeshOrderRightToLeft:
                    shaderParams.eyeBorder = glm::vec2(0.0f, 0.5f);
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();
                    break;

                case kMeshOrderTopToBottom:
                    shaderParams.eyeBorder = glm::vec2(0.0f, 0.5f);
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();

                    shaderParams.eyeBorder = glm::vec2(0.5f, 1.0f);
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2B].Submit();
                    break;

                case kMeshOrderBottomToTop:
                    shaderParams.eyeBorder = glm::vec2(0.0f, 0.5f);
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();

                    shaderParams.eyeBorder = glm::vec2(0.5f, 1.0f);
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableEyeBorder | kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2B].Submit();
                    break;
                }

#ifdef MOTION_TO_PHOTON_TEST
            }
#endif // MOTION_TO_PHOTON_TEST
        }
        else if (gpWarpFrame->frameParams.eyeBufferType == kEyeBufferArray)
        {

            // This is the case for why following code can't be common.  We have a new offset: gMeshIpdOffset
            // This has not been tested.
#ifdef MOTION_TO_PHOTON_TEST
            if (!(gpWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
            {
#endif // MOTION_TO_PHOTON_TEST

                switch (gMeshOrderEnum)
                {
                case kMeshOrderLeftToRight:
                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();
                    break;

                case kMeshOrderRightToLeft:
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();
                    break;

                case kMeshOrderTopToBottom:
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();

                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2B].Submit();
                    break;

                case kMeshOrderBottomToTop:
                    shaderParams.scaleoffset.z = 0.f;
                    shaderParams.ipdoffset.x = gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, leftEyeBuffer, 0, eyeSamplerType, leftOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(kLeftHalf);
                    gWarpData.warpMeshes[mesh2A].Submit();

                    shaderParams.scaleoffset.z = 1.f;
                    shaderParams.ipdoffset.x = -gMeshIpdOffset;
                    L_SetShaderUniforms(&shaderParams, kEnableIpdOffset, rightEyeBuffer, 0, eyeSamplerType, rightOverlayBuffer, 0, overlaySamplerType);
                    if (gMeshIpdOffset != 0.0f) L_SetSurfaceScissor(KRightHalf);
                    gWarpData.warpMeshes[mesh2B].Submit();
                    break;
                }

#ifdef MOTION_TO_PHOTON_TEST
            }
#endif // MOTION_TO_PHOTON_TEST
        }

        // As the meshes are moved using gMeshIpdOffset they can stomp on each other.
        if (gMeshIpdOffset != 0.0f)
            L_SetSurfaceScissor(kEntire);


        //LOGE("TimeToLeftEye = %lld ns; TimeToRightEye = %lld ns", timeToLeftEye, timeToRightEye);
        //LOGE("TimeToLeftEye = %0.2f ms; TimeToRightEye = %0.2f ms", (float)timeToLeftEye / 1000000.0f, (float)timeToRightEye / 1000000.0f);

        PROFILE_EXIT(GROUP_TIMEWARP); //Right Eye Submit

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "Right Eye Finalize");
        // if (gSingleBufferWindow)
        {
        	gpWarpFrame->warpSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glFlush();
            
        }
        PROFILE_EXIT(GROUP_TIMEWARP); //Right Eye Finalize

        PROFILE_EXIT(GROUP_TIMEWARP); // Warp Submission

        PROFILE_ENTER(GROUP_TIMEWARP, 0, "eglSwapBuffers");
        eglSwapBuffers(display, gAppContext->modeContext->warpRenderSurface);
        PROFILE_EXIT(GROUP_TIMEWARP); //eglSwapBuffers

        PROFILE_EXIT(GROUP_TIMEWARP); //PROFILE_SCOPE_DEFAULT(GROUP_TIMEWARP);
        
       	/*
        LOGV("-- HX -- curSubmitFrameCount = %d , submitFrameCount = %d , warpFrameCount = %d , TID = %d    [SWAP - 2]" ,
			curSubmitFrameCount,
			gAppContext->modeContext->submitFrameCount,
			gAppContext->modeContext->warpFrameCount,
			gpWarpFrame->frameParams.eyeBufferArray[0]
		);*/
        LOGE("end while");
    }   // while(true)

    LOGI("WarpThreadMain while loop exited");

    if (gTimeWarpThreadCore >= 0)
    {
        LOGI("Clearing TimeWarp Affinity");
        svrClearThreadAffinity();
    }

    if (gEnableWarpThreadFifo)
    {
        L_SetThreadPriority("TimeWarp Thread", SCHED_NORMAL, gNormalPriorityWarp);
    }

    DestroyAsyncWarpData(&gWarpData);
    g_bMeshDirty = true;

    svrDestroyWarpContext();

    LOGI("WarpThreadMain cleaned up.  Exiting");

	return 0;
}

//-----------------------------------------------------------------------------
bool svrBeginTimeWarp()
//-----------------------------------------------------------------------------
{
    LOGI("svrBeginTimeWarp");

    if (!svrUpdateEyeContextSurface())
    {
        return false;
    }

    int status = 0;

    // In case we need to change some attributes of the thread
    pthread_attr_t  threadAttribs;
    status = pthread_attr_init(&threadAttribs);
    if (status != 0)
    {
        LOGE("svrBeginTimeWarp: Failed to initialize warp thread attributes");
    }

    if (gEnableWarpThreadFifo)
    {
        status = pthread_attr_setschedpolicy(&threadAttribs, SCHED_FIFO);
        if (status != 0)
        {
            LOGE("svrBeginTimeWarp: Failed to set warp thread attribute: SCHED_FIFO");
        }

        sched_param schedParam;
        memset(&schedParam, 0, sizeof(schedParam));
        schedParam.__sched_priority = gFifoPriorityWarp;
        status = pthread_attr_setschedparam(&threadAttribs, &schedParam);
        if (status != 0)
        {
            LOGE("svrBeginTimeWarp: Failed to set warp thread attribute: Priority");
        }

    }

    status = pthread_create(&gAppContext->modeContext->warpThread, &threadAttribs, &WarpThreadMain, NULL);
    if (status != 0)
    {
        LOGE("svrBeginTimeWarp: Failed to create warp thread");
    }
    pthread_setname_np(gAppContext->modeContext->warpThread, "svrWarp");

    // No longer need the thread attributes
    status = pthread_attr_destroy(&threadAttribs);
    if (status != 0)
    {
        LOGE("svrBeginTimeWarp: Failed to destroy warp thread attributes");
    }


    //Wait until the warp thread is created and the context has been created before continuing
    LOGI("Waiting for timewarp context creation...");
    pthread_mutex_lock(&gAppContext->modeContext->warpThreadContextMutex);
    while (!gAppContext->modeContext->warpContextCreated)
    {
        pthread_cond_wait(&gAppContext->modeContext->warpThreadContextCv, &gAppContext->modeContext->warpThreadContextMutex);
    }
    pthread_mutex_unlock(&gAppContext->modeContext->warpThreadContextMutex);

    return true;
}

//-----------------------------------------------------------------------------
bool svrEndTimeWarp()
//-----------------------------------------------------------------------------
{
    LOGI("svrEndTimeWarp");

    // Check global contexts
    if (gAppContext == NULL)
    {
        LOGE("Unable to end TimeWarp! Application context has been released!");
        return false;
    }

    if (gAppContext->modeContext == NULL)
    {
        LOGE("Unable to end TimeWarp: Called when not in VR mode!");
        return false;
    }

    // Clean up any stencil assets (This is the thread to do it)
    SvrAsyncWarpResources* pWarpData = &gWarpData;
    pWarpData->stencilMeshGeomLeft.Destroy();
    pWarpData->stencilMeshGeomRight.Destroy();
    pWarpData->stencilShader.Destroy();

    //Stop the warp thread
    gAppContext->modeContext->warpThreadExit = true;
    pthread_join(gAppContext->modeContext->warpThread, NULL);

    return true;
}

