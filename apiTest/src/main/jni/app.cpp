//=============================================================================
// FILE: app.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include <unistd.h>

#include "svrApi.h"

#include "svrProfile.h"
#include "svrUtil.h"
#include "app.h"

#include "svrConfig.h"

#define PROFILE_EYE_RENDERING   0

#define NUM_MULTIVIEW_SLICES    2


using namespace Svr;

// Functions that have to be queried from EGL
typedef void(*PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR)(GLenum, GLenum, GLuint, GLint, GLint, GLsizei);
PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR glFramebufferTextureMultiviewOVR = NULL;

#ifndef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY        0x9102
#endif // GL_TEXTURE_2D_MULTISAMPLE_ARRAY

#ifndef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE       0x8D56
#endif // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE

// Functions that have to be queried from EGL
typedef void(*PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR)(GLenum, GLenum, GLuint, GLint, GLsizei, GLint, GLsizei);
PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR glFramebufferTextureMultisampleMultiviewOVR = NULL;

typedef void(*PFNGLTEXSTORAGE3DMULTISAMPLEOES)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
PFNGLTEXSTORAGE3DMULTISAMPLEOES glTexStorage3DMultisampleOES = NULL;

// These are from eglextQCOM.h
#ifndef EGL_NEW_IMAGE_QCOM
#define EGL_NEW_IMAGE_QCOM              0x3120
#endif // EGL_NEW_IMAGE_QCOM

#ifndef EGL_IMAGE_FORMAT_QCOM
#define EGL_IMAGE_FORMAT_QCOM           0x3121
#endif // EGL_IMAGE_FORMAT_QCOM

#ifndef EGL_FORMAT_RGBA_8888_QCOM
#define EGL_FORMAT_RGBA_8888_QCOM       0x3122
#endif // EGL_FORMAT_RGBA_8888_QCOM

enum eRenderMode
{
    kRenderModeSingle = 0,
    kRenderModeDouble,
    kRenderModeMultiview
};
eRenderMode gRenderMode = kRenderModeSingle;

// Configuration Variables
VAR(int, gRenderTargetType, 0, kVariableNonpersistent);

// MSAA samples: 1 = Off; 2/4 = Number of MSAA samples
VAR(int, gMsaaSamples, 1, kVariableNonpersistent);

VAR(bool, gEnableLensDistortion, true, kVariableNonpersistent);
VAR(bool, gEnableChromaticAberration, true, kVariableNonpersistent);

VAR(bool, gTouchTogglesSubmitFrame, false, kVariableNonpersistent);
VAR(bool, gForceGridTexture, false, kVariableNonpersistent);
VAR(bool, gForceOrientationTexture, false, kVariableNonpersistent);

VAR(int, gOverlayType, -1, kVariableNonpersistent);
VAR(int, gOverlayFormat, 0, kVariableNonpersistent);

VAR(int, gForcedVSyncTime, 0, kVariableNonpersistent);

VAR(bool, gForceEyeColor, false, kVariableNonpersistent);

VAR(bool, gLogSensorData, false, kVariableNonpersistent);

// For testing warping we need to sometimes turn off frame submit. These work with gTouchTogglesSubmitFrame
bool gSubmitFrame = true;
unsigned int gLastToggleTime = 0;

// Sometimes we need to test application running slower than TimeWarp.
// To force a desired FPS, set gForcedVSyncTime to desired frame time (milliseconds)
unsigned int gLastRenderTime = 0;

//TODO: Add this to config file and check in the arrow object
bool gUseArrows = false;

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

SvrApiTest::SvrApiTest()
    : mMdlRotation(0.0f)
    , mFrameTimer(64)
{
}

//-----------------------------------------------------------------------------
void * SvrApiTest::GetFileBuffer(const char *pFileName, int *pBufferSize)
//-----------------------------------------------------------------------------
{
    char filePath[512];

    sprintf(filePath, "%s/%s", mAppContext.externalPath, pFileName);

    LOGI("Opening File: %s", filePath);
    FILE* fp = fopen(filePath, "rb");
    if (!fp)
    {
        LOGE("Unable to open file: %s", filePath);
        return NULL;
    }

    // Need the file size
    fseek(fp, 0, SEEK_END);
    *pBufferSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Now read the file into the new buffer
    char* pRetBuff = new char[*pBufferSize + 1];
    fread(pRetBuff, 1, *pBufferSize, fp);
    pRetBuff[*pBufferSize] = 0;

    fclose(fp);

    return pRetBuff;
}

bool SvrApiTest::LoadTextureCommon(GLuint *pTexture, const char *pFileName)
{
    GLenum TexTarget;
    KtxTexture TexHelper;

    LOGI("Loading %s...", pFileName);

    // Open File
    int TexBuffSize;
    char *pTexBuffer = (char *)GetFileBuffer(pFileName, &TexBuffSize);
    if (pTexBuffer == NULL)
    {
        LOGE("Unable to open file: %s", pFileName);
        return false;
    }

    // Use the helper to create the texture
    TKTXHeader* pOutHeader = NULL;
    TKTXErrorCode ResultCode = TexHelper.LoadKtxFromBuffer(pTexBuffer, TexBuffSize, pTexture, &TexTarget, pOutHeader);
    if (ResultCode != KTX_SUCCESS)
    {
        LOGE("Unable to load texture: %s", pFileName);
    }


    // No longer need buffers
    free(pTexBuffer);

    return true;
}

bool SvrApiTest::CreateEglImage()
{
    // This overlay can any size.  Could make same size as eye buffer or
    // size of physical surface (for one eye).  Video textures will be larger
    // so final render will be sharper.
    mImageWidth = mAppContext.physicalWidth / 2;
    mImageHeight = mAppContext.physicalHeight;

    LOGI("**************************************************");
    LOGI("Creating %dx%d image buffer for overlay...", mImageWidth, mImageHeight);
    LOGI("**************************************************");

    // Create in RGBA format
    int totalBytes = mImageWidth * mImageHeight * 4;
    unsigned char *pData = (unsigned char *) new unsigned char[totalBytes];
    if (pData == NULL)
    {
        LOGE("Unable to allocate %d bytes for image buffer!", totalBytes);
        return false;
    }

    // Fill in default data for the texture
    // Routine taken from page 373 of OpenGL Redbook
    unsigned char onColor[4] = { 255, 255, 255, 255 };
    unsigned char offColor[4] = { 25, 25, 25, 255 };
    for (int row = 0; row < mImageHeight; row++)
    {
        for (int col = 0; col < mImageWidth; col++)
        {
            int whichPixel = (col * mImageHeight + row) * 4;

            // 0x08 parameters tells how many "checkers".
            // The lower the value, the more "checkers"
            // The number of "checkers" is uiWidth / 0x08
            unsigned int onTest = (row & 0x08) ^ (col & 0x08);

            if (onTest)
            {
                pData[whichPixel + 0] = onColor[0];
                pData[whichPixel + 1] = onColor[1];
                pData[whichPixel + 2] = onColor[2];
                pData[whichPixel + 3] = onColor[3];
            }
            else
            {
                pData[whichPixel + 0] = offColor[0];
                pData[whichPixel + 1] = offColor[1];
                pData[whichPixel + 2] = offColor[2];
                pData[whichPixel + 3] = offColor[3];
            }
        }   // Which Column
    }   // Which Row

    // Create the image object
    EGLint EglAttribs[] =
    {
        EGL_WIDTH, mImageWidth,
        EGL_HEIGHT, mImageHeight,
        EGL_IMAGE_FORMAT_QCOM, EGL_FORMAT_RGBA_8888_QCOM,
        EGL_NONE
    };

    // Can't get access to gralloc so have to let the driver create a new one for us.
    // mEGLImage = eglCreateImageKHR(mAppContext.display, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, (EGLClientBuffer)pData, EglAttribs);
    mEGLImage = eglCreateImageKHR(mAppContext.display, EGL_NO_CONTEXT, EGL_NEW_IMAGE_QCOM, (EGLClientBuffer)0, EglAttribs);
    if (mEGLImage == 0)
    {
        LOGE("Unable to create image buffer!");
        return false;
    }

    delete[] pData;

    // Create the GL texture object that goes with the image
    glGenTextures(1, &mImageTexture);

    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mImageTexture);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Have the EGL image provide backing for the GL texture
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mEGLImage);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);

    // Create the GL framebuffer object and attach the texture to it
    glGenFramebuffers(1, &mImageFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mImageFrameBuffer);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_EXTERNAL_OES, mImageTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void SvrApiTest::UpdateEglImage()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mImageFrameBuffer);
    GLenum nDrawBuffer[] = { GL_COLOR_ATTACHMENT0 };
    glViewport(0, 0, mImageWidth, mImageHeight);
    glScissor(0, 0, mImageWidth, mImageHeight);
    glDrawBuffers(1, nDrawBuffer);

    glClearColor(0.45f, 0.2f, 0.45f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind the blit shader...
    mBlitShader.Bind();

    // ... set uniforms ...
    glm::vec4 scaleoffset = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    mBlitShader.SetUniformVec4(1, scaleoffset);

    // ... set textures ...
    mBlitShader.SetUniformSampler("srcTex", mOverlayTextures[kQuadOverlay], GL_TEXTURE_2D);

    // ... render the quad
    mBlitMesh.Submit();

    // Unbind the frame buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void SvrApiTest::CreateBlitAssets()
{
    // **********************
    // Geometry
    // **********************
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

    mBlitMesh.Initialize(   &attribs[0], 2, 
                            &quadIndices[0], 6,
                            &quadVertices[0], sizeof(quadVert) * 4, 4);

    // **********************
    // Shader
    // **********************
    InitializeShader(mBlitShader, "shaders/blit_v.glsl", "shaders/blit_f.glsl", "blitVs", "blitFs");
}

void SvrApiTest::InitializeModel(const char* path)
{
    char filePath[512];

    SvrGeometry*    pObjGeom;
    int             nObjGeom;
    sprintf(filePath, "%s/%s", mAppContext.externalPath, path);
    SvrGeometry::CreateFromObjFile(filePath, &pObjGeom, nObjGeom);
    mpModel = &pObjGeom[0];

    InitializeShader(mModelShader, "shaders/model_v.glsl", "shaders/model_f.glsl", "modelVs", "modelFs");
    InitializeShader(mMultiViewShader, "shaders/multiview_v.glsl", "shaders/multiview_f.glsl", "multiViewVs", "multiViewFs");
}

void SvrApiTest::InitializeShader(Svr::SvrShader &whichShader, const char* vertexPath, const char* fragmentPath, const char* vertexName, const char* fragmentName)
{
    int fileBuffSize;

    char *pVsBuffer = (char *)GetFileBuffer(vertexPath, &fileBuffSize);
    if (pVsBuffer == NULL)
    {
        return;
    }

    char *pFsBuffer = (char *)GetFileBuffer(fragmentPath, &fileBuffSize);
    if (pFsBuffer == NULL)
    {
        free(pVsBuffer);
        return;
    }

    whichShader.Initialize(pVsBuffer, pFsBuffer, vertexName, fragmentName);

    // No longer need buffers
    free(pVsBuffer);
    free(pFsBuffer);
}

void SvrApiTest::InitializeFunctions()
{
    LOGI("**************************************************");
    LOGI("Initializing Function: glFramebufferTextureMultiviewOVR");
    LOGI("**************************************************");
    glFramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR)eglGetProcAddress("glFramebufferTextureMultiviewOVR");
    if (!glFramebufferTextureMultiviewOVR)
    {
        LOGE("Failed to  get proc address for glFramebufferTextureMultiviewOVR!!");
        exit(EXIT_FAILURE);
    }

    LOGI("**************************************************");
    LOGI("Initializing Function: glFramebufferTextureMultisampleMultiviewOVR");
    LOGI("**************************************************");
    glFramebufferTextureMultisampleMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR)eglGetProcAddress("glFramebufferTextureMultisampleMultiviewOVR");
    if (!glFramebufferTextureMultisampleMultiviewOVR)
    {
        LOGE("Failed to  get proc address for glFramebufferTextureMultisampleMultiviewOVR!!");
        exit(EXIT_FAILURE);
    }

    LOGI("**************************************************");
    LOGI("Initializing Function: glTexStorage3DMultisampleOES");
    LOGI("**************************************************");
    glTexStorage3DMultisampleOES = (PFNGLTEXSTORAGE3DMULTISAMPLEOES)eglGetProcAddress("glTexStorage3DMultisampleOES");
    if (!glTexStorage3DMultisampleOES)
    {
        LOGE("Failed to  get proc address for glTexStorage3DMultisampleOES!!");
        exit(EXIT_FAILURE);
    }
}

void SvrApiTest::InitializeMultiView(int whichBuffer, int width, int height, int samples)
{
    if (whichBuffer >= SVR_NUM_EYE_BUFFERS)
    {
        LOGE("InitializeMultiView(): Buffer index out of range!! Passed %d when max allowed is %d", whichBuffer, SVR_NUM_EYE_BUFFERS - 1);
        return;
    }

    GLsizei NumLevels = 1;
    GLenum ColorFormat = GL_RGBA8;

    // Create the color attachment array
    GL(glGenTextures(1, &mMultiViewBuffers[whichBuffer].mColorId));
    GL(glBindTexture(GL_TEXTURE_2D_ARRAY, mMultiViewBuffers[whichBuffer].mColorId));
    GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, NumLevels, ColorFormat, mMultiViewWidth, mMultiViewHeight, mMultiViewSlices));
    GL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

    // Create the depth attachment array
    GL(glGenTextures(1, &mMultiViewBuffers[whichBuffer].mDepthId));
    GL(glBindTexture(GL_TEXTURE_2D_ARRAY, mMultiViewBuffers[whichBuffer].mDepthId));
    GL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, NumLevels, GL_DEPTH_COMPONENT24, mMultiViewWidth, mMultiViewHeight, mMultiViewSlices));
    GL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

    // Create the frame buffer object
    GL(glGenFramebuffers(1, &mMultiViewBuffers[whichBuffer].mFboId));

    // Attach the color and depth arrays
    GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mMultiViewBuffers[whichBuffer].mFboId));

    if (samples > 1)
    {
        LOGI("Attaching color and depth to framebuffer as multi-sample");
        GL(glFramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mMultiViewBuffers[whichBuffer].mColorId, 0, samples, 0, mMultiViewSlices));
        GL(glFramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mMultiViewBuffers[whichBuffer].mDepthId, 0, samples, 0, mMultiViewSlices));
    }
    else
    {
        GL(glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mMultiViewBuffers[whichBuffer].mColorId, 0, 0, mMultiViewSlices));
        GL(glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mMultiViewBuffers[whichBuffer].mDepthId, 0, 0, mMultiViewSlices));
    }


    // Verify the frame buffer was created
    GLenum eResult = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (eResult != GL_FRAMEBUFFER_COMPLETE)
    {
        const char *pPrefix = "MultiView Framebuffer";
        switch (eResult)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGE("%s => Error (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) setting up FBO", pPrefix);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGE("%s => Error (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) setting up FBO", pPrefix);
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGE("%s => Error (GL_FRAMEBUFFER_UNSUPPORTED) setting up FBO", pPrefix);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            LOGE("%s => Error (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) setting up FBO", pPrefix);
            break;
        default:
            LOGE("%s => Error (0x%X) setting up FBO", pPrefix, eResult);
            break;
        }
    }

    // No longer need to be bound
    GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void SvrApiTest::Initialize()
{
    mCurrentEyeBuffer = 0;

    // Load the config file
    static const char* gAppConfigFilePath = "app_config.txt";

    char filePath[512];
    sprintf(filePath, "%s/%s", mAppContext.externalPath, gAppConfigFilePath);
    LoadVariableFile(filePath);

    // Convert config file variable to enumeration
    if (gRenderTargetType == 0)
    {
        LOGI("Rendering to separate target for each eye");
        gRenderMode = kRenderModeSingle;
    }
    else if (gRenderTargetType == 1)
    {
        LOGI("Rendering to single target for both eyes");
        gRenderMode = kRenderModeDouble;
    }
    else if (gRenderTargetType == 2)
    {
        LOGI("Rendering to multiview target for both eyes");
        gRenderMode = kRenderModeMultiview;
    }
    else
    {
        LOGI("Unknown render target type: %d.  Rendering to separate target for each eye", gRenderTargetType);
        gRenderMode = kRenderModeSingle;
    }

    // Since Multi-View, check the MSAA setting
    if (gMsaaSamples == 1)
    {
        LOGI("Rendering to non-MSAA target");
    }
    else if (gMsaaSamples == 2 || gMsaaSamples == 4)
    {
        LOGI("Rendering to %dX-MSAA target", gMsaaSamples);
    }
    else
    {
        LOGI("Unsupported MSAA samples: %d.  Rendering to non-MSAA target for both eyes", gMsaaSamples);
        gMsaaSamples = 1;
    }

    switch (gRenderMode)
    {
    case kRenderModeSingle:
        break;

    case kRenderModeDouble:
        break;

    case kRenderModeMultiview:
        // Some functions need their pointers initialized
        InitializeFunctions();
        break;
    }

    // This MUST be after InitalizeFunctions() when using MultiView because it allocates eye buffers
    // using functions that must be set up before using.
    SvrApplication::Initialize();

    mAppContext.cpuPerfLevel = kPerfMedium;
    mAppContext.gpuPerfLevel = kPerfMedium;

    CreateBlitAssets();

    if (gUseArrows)
    {
        InitializeModel("models/arrow.obj");
    }
    else
    {
        InitializeModel("models/torus.obj");
    }

    mGridTexture = 0;
    if (gForceGridTexture)
    {
        const char *pGridTexFile = "textures/grid.ktx";
        LoadTextureCommon(&mGridTexture, pGridTexFile);
    }

    mOrientationTexture = 0;
    if (gForceOrientationTexture)
    {
        const char *pOrientationTexFile = "textures/bbb_title.ktx";
        LoadTextureCommon(&mOrientationTexture, pOrientationTexFile);
    }

    memset(mOverlayTextures, 0, sizeof(mOverlayTextures));
    mImageFrameBuffer = 0;
    mImageTexture = 0;
    mEGLImage = reinterpret_cast<EGLImageKHR>(0);

    if (gOverlayType >= 0)
    {
        // const char *pCenterOverlayTexFile = "textures/overlay_items_center.ktx";
        // const char *pLeftOverlayTexFile = "textures/overlay_items_left.ktx";
        // const char *pRightOverlayTexFile = "textures/overlay_items_right.ktx";

        const char *pCenterOverlayTexFile = "textures/overlay_text_center.ktx";
        const char *pLeftOverlayTexFile = "textures/overlay_text_left.ktx";
        const char *pRightOverlayTexFile = "textures/overlay_text_right.ktx";

        const char *pQuadOverlayTexFile = "textures/bbb_frame.ktx";

        switch (gOverlayFormat)
        {
        case kOverlayTexture:
            LoadTextureCommon(&mOverlayTextures[kMonoOverlay], pCenterOverlayTexFile);
            LoadTextureCommon(&mOverlayTextures[kLeftOverlay], pLeftOverlayTexFile);
            LoadTextureCommon(&mOverlayTextures[kRightOverlay], pRightOverlayTexFile);
            LoadTextureCommon(&mOverlayTextures[kQuadOverlay], pQuadOverlayTexFile);
            break;

        case kOverlayImage:
            CreateEglImage();

            // Both Texture and Image use the Quad Overlay texture
            LoadTextureCommon(&mOverlayTextures[kQuadOverlay], pQuadOverlayTexFile);
            break;
        }

    }

    float aspect = (float)mAppContext.targetEyeWidth / (float)mAppContext.targetEyeHeight;
   
    mProjMtx = glm::perspective(DEG_TO_RAD * mAppContext.targetEyeFovYDeg, aspect, 0.1f, 300.0f);

    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;

    float objectPositionRadius = 4.0f;

    // +X
    mMdlMtx[0] = glm::translate(glm::mat4(1.0f), glm::vec3(objectPositionRadius, 0.0f, 0.0f));
    mMdlColor[0] = glm::vec3(1.0f, 0.0f, 0.0f);     // gForceEyeColor: Do NOT change this!

    // +Y
    mMdlMtx[1] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, objectPositionRadius, 0.0f));
    mMdlColor[1] = glm::vec3(0.0f, 1.0f, 0.0f);     // gForceEyeColor: Do NOT change this!

    // +Z
    mMdlMtx[2] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, objectPositionRadius));
    mMdlColor[2] = glm::vec3(0.0f, 0.0f, 1.0f);

    // -X
    mMdlMtx[3] = glm::translate(glm::mat4(1.0f), glm::vec3(-objectPositionRadius, 0.0f, 0.0f));
    mMdlColor[3] = glm::vec3(0.0f, 1.0f, 1.0f);

    // -Y
    mMdlMtx[4] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -objectPositionRadius, 0.0f));
    mMdlColor[4] = glm::vec3(1.0f, 0.0f, 1.0f);

    // -Z
    mMdlMtx[5] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -objectPositionRadius));
    mMdlColor[5] = glm::vec3(1.0f, 1.0f, 0.0f);

    mHeadPosition = glm::vec3(0.0f);

    mLastFPSTime = GetTimeMS();
    mCurrentFPS = 0.0f;
}

void SvrApiTest::Shutdown()
{
    DestroyBlitAssets();

    if (mImageTexture != 0)
        glDeleteTextures(1, &mImageTexture);
    mImageTexture = 0;

    if (mImageFrameBuffer != 0)
        glDeleteFramebuffers(1, &mImageFrameBuffer);
    mImageFrameBuffer = 0;

    if (mEGLImage != 0)
        eglDestroyImageKHR(eglGetCurrentDisplay(), mEGLImage);
    mEGLImage = 0;

    if (mGridTexture != 0)
        glDeleteTextures(1, &mGridTexture);
    mGridTexture = 0;

    if (mOrientationTexture != 0)
        glDeleteTextures(1, &mOrientationTexture);
    mOrientationTexture = 0;

}

void SvrApiTest::DestroyBlitAssets()
{
    mBlitMesh.Destroy();
    mBlitShader.Destroy();
}

void SvrApiTest::AllocateEyeBuffers()
{
    switch (gRenderMode)
    {
    case kRenderModeSingle:
        LOGI("SvrApiTest - Allocating Separate Single Eye Buffers");

        LOGI("**************************************************");
        LOGI("Creating %dx%d framebuffer...", mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
        LOGI("**************************************************");

        for (int whichBuffer = 0; whichBuffer < SVR_NUM_EYE_BUFFERS; whichBuffer++)
        {
            mAppContext.eyeBuffers[whichBuffer].eyeTarget[kLeft].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, gMsaaSamples, GL_RGBA8, true);
            mAppContext.eyeBuffers[whichBuffer].eyeTarget[kRight].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, gMsaaSamples, GL_RGBA8, true);
        }
        break;

    case kRenderModeDouble:
        LOGI("SvrApiTest - Allocating Wide Eye Buffers");

        LOGI("**************************************************");
        LOGI("Creating %dx%d framebuffer...", mAppContext.targetEyeWidth * 2, mAppContext.targetEyeHeight);
        LOGI("**************************************************");

        for (int whichBuffer = 0; whichBuffer < SVR_NUM_EYE_BUFFERS; whichBuffer++)
        {
            mAppContext.eyeBuffers[whichBuffer].eyeTarget[0].Initialize(mAppContext.targetEyeWidth * 2, mAppContext.targetEyeHeight, gMsaaSamples, GL_RGBA8, true);
        }
        break;

    case kRenderModeMultiview:
        mMultiViewSlices = NUM_MULTIVIEW_SLICES;
        mMultiViewWidth = mAppContext.targetEyeWidth;
        mMultiViewHeight = mAppContext.targetEyeHeight;
        mMultiViewSamples = gMsaaSamples;

        LOGI("**************************************************");
        LOGI("Creating %dx%d mulitview framebuffer (%d slices; %d samples)...", mMultiViewWidth, mMultiViewHeight, mMultiViewSlices, mMultiViewSamples);
        LOGI("**************************************************");

        for (int whichBuffer = 0; whichBuffer < SVR_NUM_EYE_BUFFERS; whichBuffer++)
        {
            InitializeMultiView(whichBuffer, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, gMsaaSamples);
        }
        break;
    }

}

void L_CreateLayout(float centerX, float centerY, float radiusX, float radiusY, svrOverlayLayout *pLayout)
{
    float lowerLeft[4]  = { centerX - radiusX, centerY - radiusY, 0.0f, 0.0f };
    float lowerRight[4] = { centerX + radiusX, centerY - radiusY, 1.0f, 0.0f };
    float upperLeft[4]  = { centerX - radiusX, centerY + radiusY, 0.0f, 1.0f };
    float upperRight[4] = { centerX + radiusX, centerY + radiusY, 1.0f, 1.0f };

    memcpy(pLayout->LowerLeft, lowerLeft, sizeof(lowerLeft));
    memcpy(pLayout->LowerRight, lowerRight, sizeof(lowerRight));
    memcpy(pLayout->UpperLeft, upperLeft, sizeof(upperLeft));
    memcpy(pLayout->UpperRight, upperRight, sizeof(upperRight));
}


void SvrApiTest::Update()
{
    PROFILE_ENTER(GROUP_WORLDRENDER, 0, "Update");

    mFrameTimer.Start();

    SvrApplication::Update();


    // Update the time
    unsigned int TimeNow = GetTimeMS();
    
    unsigned int ReportTimeFPS = 1000;
    mFrameCount++;
    if (TimeNow - mLastFPSTime > ReportTimeFPS)
    {
        float ElapsedSeconds = (float)(TimeNow - mLastFPSTime) / 1000.0f;
        mCurrentFPS = (float)mFrameCount / ElapsedSeconds;
        //LOGI("FPS: %0.2f", mCurrentFPS);

        mFrameCount = 0;
        mLastFPSTime = GetTimeMS();;
    }

    if (mInput.IsPointerDown(0))
    {
        if (gTouchTogglesSubmitFrame)
        {
            if (TimeNow - gLastToggleTime > 1000)
            {
                gLastToggleTime = TimeNow;
                gSubmitFrame = !gSubmitFrame;
            }
        }
        else
        {
            // Recenter the pose
            svrRecenterPose();
        }
    }

    PROFILE_EXIT(GROUP_WORLDRENDER);
}

void SvrApiTest::Render()
{
    if (!gSubmitFrame)
        return;

    unsigned int TimeNow = GetTimeMS();
    if (gLastRenderTime == 0)
        gLastRenderTime = TimeNow;

    if (gForcedVSyncTime != 0)
    {
        // Forcing VSync
        if (TimeNow - gLastRenderTime < gForcedVSyncTime)
        {
            return;
        }
    }

    float elapsedTime = (float)(TimeNow - gLastRenderTime) / 1000.0f;
    float rotateAmount = elapsedTime * (360.0f / 5.0f);   // Rotating at 5 sec/rotation
    for (int i = 0; i < 6; i++)
    {
        if (gUseArrows)
        {
            mMdlMtx[i] = glm::rotate(mMdlMtx[i], glm::radians(rotateAmount), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else
        {
            mMdlMtx[i] = glm::rotate(mMdlMtx[i], glm::radians(rotateAmount), glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }

    gLastRenderTime = TimeNow;

    PROFILE_ENTER(GROUP_WORLDRENDER, 0, "Render");

#if PROFILE_EYE_RENDERING
    unsigned int eyeGpuTimerSamples[2];
    mGpuTimer.StartFrame();
#endif //PROFILE_EYE_RENDERING

    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    float predictedTimeMs = svrGetPredictedDisplayTime();
    svrHeadPoseState poseState = svrGetPredictedHeadPose(predictedTimeMs);

    if (gLogSensorData)
    {
        LOGE("Position: (%0.2f, %0.2f, %0.2f); Orientation: (%0.2f, %0.2f, %0.2f, %0.2f)", poseState.pose.position.x,
                                                                                            poseState.pose.position.y, 
                                                                                            poseState.pose.position.z, 
                                                                                            poseState.pose.rotation.x,
                                                                                            poseState.pose.rotation.y, 
                                                                                            poseState.pose.rotation.z,
                                                                                            poseState.pose.rotation.w);
    }

    if (poseState.poseStatus & kTrackingPosition)
    {
        //Apply a scale to adjust for the scale of the content
        poseState.pose.position.x *= 2.0f;
        poseState.pose.position.y *= 2.0f;
        poseState.pose.position.z *= 2.0f;
    }
    else
    {
        poseState.pose.position.x = mHeadPosition.x;
        poseState.pose.position.y = mHeadPosition.y;
        poseState.pose.position.z = mHeadPosition.z;
    }

    float currentIpd = DEFAULT_IPD;
    float headHeight = DEFAULT_HEAD_HEIGHT;
    float headDepth = DEFAULT_HEAD_DEPTH;
    SvrGetEyeViewMatrices(poseState, false,
                            currentIpd, headHeight, headDepth,
                            mViewMtx[kLeft], mViewMtx[kRight]);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0);

    switch (gRenderMode)
    {
    case kRenderModeSingle:
        break;

    case kRenderModeDouble:
        //Bind the eye buffer
        mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[0].Bind();
        glDisable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        break;

    case kRenderModeMultiview:
        GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mMultiViewBuffers[mCurrentEyeBuffer].mFboId));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        break;
    }

    int numRenderCalls = SVR_NUM_EYES;
    switch (gRenderMode)
    {
    case kRenderModeSingle:
    case kRenderModeDouble:
        numRenderCalls = SVR_NUM_EYES;
        break;

    case kRenderModeMultiview:
        numRenderCalls = 1;
        break;
    }

    for (int whichEye = 0; whichEye < numRenderCalls; whichEye++)
    {
        // These get changed when rendering the GUI so put them back now
        glEnable(GL_SCISSOR_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        switch (gRenderMode)
        {
        case kRenderModeSingle:
            //Bind the eye buffer
            mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[whichEye].Bind();
            glViewport(0, 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
            glScissor(0, 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;

        case kRenderModeDouble:
            glViewport(0 + ((float)whichEye * mAppContext.targetEyeWidth), 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
            glScissor(0 + ((float)whichEye * mAppContext.targetEyeWidth), 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
            break;

        case kRenderModeMultiview:
            glViewport(0, 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
            glScissor(0, 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;
        }

        // Eye is ready to be rendered
        // TODO: Don't currently support MultiView!
#ifdef HIDE_STENCIL_WIP
        switch (gRenderMode)
        {
        case kRenderModeSingle:
            svrBeginEye(kEyeBufferStereoSeparate, (svrWhichEye)whichEye);
            break;

        case kRenderModeDouble:
            svrBeginEye(kEyeBufferStereoSingle, (svrWhichEye)whichEye);
            break;

        case kRenderModeMultiview:
            break;
        }
#endif // HIDE_STENCIL_WIP

        switch (gRenderMode)
        {
        case kRenderModeSingle:
            mModelShader.Bind();
            break;

        case kRenderModeDouble:
            mModelShader.Bind();
            break;

        case kRenderModeMultiview:
            mMultiViewShader.Bind();
            break;
        }


        for (int j = 0; j < 6; j++)
        {
            switch (gRenderMode)
            {
            case kRenderModeSingle:
            case kRenderModeDouble:
                mModelShader.SetUniformMat4("projMtx", mProjMtx);
                mModelShader.SetUniformMat4("viewMtx", mViewMtx[whichEye]);
                mModelShader.SetUniformMat4("mdlMtx", mMdlMtx[j]);

                if (gForceEyeColor)
                    mModelShader.SetUniformVec3("modelColor", mMdlColor[whichEye]);    // Red for left eye, green for right eye
                else
                    mModelShader.SetUniformVec3("modelColor", mMdlColor[j]);
                break;

            case kRenderModeMultiview:
                mMultiViewShader.SetUniformMat4("projMtx", mProjMtx);
                mMultiViewShader.SetUniformMat4("mdlMtx", mMdlMtx[j]);
                mMultiViewShader.SetUniformVec3("modelColor", mMdlColor[j]);

                // TODO: How do we implement gForceEyeColor?  Currently done in shader.

                // This is silly, but the uniform is in the list as "viewMtx[0]"
                mMultiViewShader.SetUniformMat4fv("viewMtx[0]", 2, glm::value_ptr(mViewMtx[0]));
                break;
            }

            mpModel->Submit();
        }

        // Eye is done being rendered
        // TODO: Don't currently support MultiView!
#ifdef HIDE_STENCIL_WIP
        switch (gRenderMode)
        {
        case kRenderModeSingle:
            svrEndEye(kEyeBufferStereoSeparate, (svrWhichEye)whichEye);
            break;

        case kRenderModeDouble:
            svrEndEye(kEyeBufferStereoSingle, (svrWhichEye)whichEye);
            break;

        case kRenderModeMultiview:
            break;
        }
#endif // HIDE_STENCIL_WIP

        switch (gRenderMode)
        {
        case kRenderModeSingle:
            //Unbind the eye buffer (call so attachments are invalidated correctly)
            mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[whichEye].Unbind();
            break;

        case kRenderModeDouble:
            break;

        case kRenderModeMultiview:
            break;
        }

#if PROFILE_EYE_RENDERING
        eyeGpuTimerSamples[i] = mGpuTimer.TakeSample();
#endif //PROFILE_EYE_RENDERING
    }


    switch (gRenderMode)
    {
    case kRenderModeSingle:
        break;

    case kRenderModeDouble:
        //Unbind the eye buffer (call so attachments are invalidated correctly)
        mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[0].Unbind();
        break;

    case kRenderModeMultiview:
        GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        break;
    }

#if PROFILE_EYE_RENDERING
    mGpuTimer.EndFrame();
#endif //PROFILE_EYE_RENDERING

    svrFrameParams frameParams;
    memset(&frameParams, 0, sizeof(frameParams));
    frameParams.frameIndex = mAppContext.frameCount;

    if (!gEnableLensDistortion)
    {
        frameParams.frameOptions |= kDisableDistortionCorrection;
    }

    if (!gEnableChromaticAberration)
    {
        frameParams.frameOptions |= kDisableChromaticCorrection;
    }
    
    switch (gRenderMode)
    {
    case kRenderModeSingle:
        frameParams.eyeBufferArray[0] = mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[Svr::kLeft].GetColorAttachment();
        frameParams.eyeBufferArray[1] = mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[Svr::kRight].GetColorAttachment();
        frameParams.eyeBufferType = kEyeBufferStereoSeparate;
        break;

    case kRenderModeDouble:
        frameParams.eyeBufferArray[0] = mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[0].GetColorAttachment();
        frameParams.eyeBufferType = kEyeBufferStereoSingle;
        break;

    case kRenderModeMultiview:
        frameParams.eyeBufferArray[0] = mMultiViewBuffers[mCurrentEyeBuffer].mColorId;
        frameParams.eyeBufferType = kEyeBufferArray;
        break;
    }

    frameParams.headPoseState = poseState;
    frameParams.minVsyncs = 1;
    PROFILE_EXIT(GROUP_WORLDRENDER);


#if PROFILE_EYE_RENDERING
    float leftEyeTime = (float)mGpuTimer.GetTimeTilNextSample(eyeGpuTimerSamples[0] - 1) * 1e-6;
    float rightEyeTime = (float)mGpuTimer.GetTimeTilNextSample(eyeGpuTimerSamples[1] - 1) * 1e-6;
    LOGI("Eye Time : %2.3f ms, %2.3f ms", leftEyeTime, rightEyeTime);
#endif //PROFILE_EYE_RENDERING



    if (gForceGridTexture)
    {
        frameParams.eyeBufferArray[0] = mGridTexture;
        frameParams.eyeBufferArray[1] = mGridTexture;
    }

    if (gForceOrientationTexture)
    {
        frameParams.eyeBufferArray[0] = mOrientationTexture;
        frameParams.eyeBufferArray[1] = mOrientationTexture;
    }

    // LOGI("Submitting Eye Buffers: Left = %d; Right = %d", frameParams.eyeBufferArray[0], frameParams.eyeBufferArray[1]);

    if (gOverlayType >= 0)
    {
        if (gOverlayFormat == kOverlayImage)
        {
            UpdateEglImage();
        }

        switch (gOverlayType)
        {
        case kOverlayMono:
            frameParams.overlayType = kOverlayMono;

            switch (gOverlayFormat)
            {
            case kOverlayTexture:
                frameParams.overlayFormat = kOverlayTexture;
                frameParams.overlayBuffer[0] = mOverlayTextures[kMonoOverlay];
                break;

            case kOverlayImage:
                frameParams.overlayFormat = kOverlayImage;
                frameParams.overlayBuffer[0] = mImageTexture;
                break;
            }
            break;

        case kOverlayStereo:
            frameParams.overlayType = kOverlayStereo;

            switch (gOverlayFormat)
            {
            case kOverlayTexture:
                frameParams.overlayFormat = kOverlayTexture;
                frameParams.overlayBuffer[0] = mOverlayTextures[kLeftOverlay];
                frameParams.overlayBuffer[1] = mOverlayTextures[kRightOverlay];
                break;

            case kOverlayImage:
                frameParams.overlayFormat = kOverlayImage;
                frameParams.overlayBuffer[0] = mImageTexture;
                frameParams.overlayBuffer[1] = mImageTexture;
                break;
            }
            break;

        case kOverlayLayers:
            frameParams.overlayType = kOverlayLayers;

            L_CreateLayout(-0.4f, 0.4f, 0.5f, 0.5f, &frameParams.overlayLayout[0]);
            L_CreateLayout(0.4f, -0.4f, 0.5f, 0.5f, &frameParams.overlayLayout[1]);

            switch (gOverlayFormat)
            {
            case kOverlayTexture:
                frameParams.overlayFormat = kOverlayTexture;
                frameParams.overlayBuffer[0] = mOverlayTextures[kQuadOverlay];
                frameParams.overlayBuffer[1] = mOverlayTextures[kQuadOverlay];
                break;

            case kOverlayImage:
                frameParams.overlayFormat = kOverlayImage;
                frameParams.overlayBuffer[0] = mImageTexture;
                frameParams.overlayBuffer[1] = mImageTexture;
                break;
            }
            break;
        }

    }

    svrSubmitFrame(&frameParams);
   
    mAppContext.eyeBufferIndex = (mAppContext.eyeBufferIndex + 1) % SVR_NUM_EYE_BUFFERS;

    
    mCurrentEyeBuffer = (mCurrentEyeBuffer + 1) % SVR_NUM_EYE_BUFFERS;


    mFrameTimer.Stop();
    
}

namespace Svr
{
    Svr::SvrApplication* CreateApplication()
    {
        return new SvrApiTest();
    }
}


