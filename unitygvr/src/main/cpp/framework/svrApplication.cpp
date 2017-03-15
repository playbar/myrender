//=============================================================================
// FILE: svrApplication.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "svrApplication.h"
#include "svrUtil.h"

namespace Svr
{

bool SvrApplication::mImGuiInitialized = false;
int  SvrApplication::mImGuiShaderHandle = 0;
int  SvrApplication::mImGuiVertHandle = 0;
int  SvrApplication::mImGuiFragHandle = 0;
int  SvrApplication::mImGuiAttribLocTex = 0;
int  SvrApplication::mImGuiAttribLocProjMtx = 0;
int  SvrApplication::mImGuiAttribLocPos = 0;
int  SvrApplication::mImGuiAttribLocUv = 0;
int  SvrApplication::mImGuiAttribLocColor = 0;
unsigned int SvrApplication::mImGuiFontTextureHandle = 0;
unsigned int SvrApplication::mImGuiVboHandle = 0;
unsigned int SvrApplication::mImGuiVaoHandle = 0;
unsigned int SvrApplication::mImGuiElementsHandle = 0;

SvrApplication::SvrApplication()
{
    mAppContext.physicalWidth = 0;
    mAppContext.physicalHeight = 0;
    mAppContext.targetEyeWidth = 0;
    mAppContext.targetEyeHeight = 0;

    mAppContext.display = EGL_NO_DISPLAY;

    mAppContext.eyeRenderSurface = EGL_NO_SURFACE;
    mAppContext.eyeRenderContext = EGL_NO_CONTEXT;
   
    mAppContext.assetManager = NULL;
    mAppContext.nativeWindow = NULL;

    mAppContext.frameCount = 0;
}

SvrApplication::~SvrApplication()
{
}

void SvrApplication::Initialize()
{
    if (!mImGuiInitialized)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = mAppContext.targetEyeWidth;
        io.DisplaySize.y = mAppContext.targetEyeHeight;
        io.DeltaTime = 1.0f / 60.0f;
        io.IniFilename = NULL;
        io.RenderDrawListsFn = SvrApplication::RenderImGuiDrawLists;

        CreateImGuiDeviceObjects();

        mImGuiInitialized = true;
    }

    mAppContext.eyeBufferIndex = 0;
    mAppContext.cpuPerfLevel = kPerfMedium;
    mAppContext.gpuPerfLevel = kPerfMedium;
    mAppContext.trackingMode = (kTrackingRotation | kTrackingPosition);

    AllocateEyeBuffers();
}

void SvrApplication::AllocateEyeBuffers()
{
    //Default to separate render targets per eye 
    LOGI("SvrApplication - Allocating Separate Single Eye Buffers");
    for (int i = 0; i < SVR_NUM_EYE_BUFFERS; i++)
    {
        mAppContext.eyeBuffers[i].eyeTarget[kLeft].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, 1, GL_RGBA8, true);
        mAppContext.eyeBuffers[i].eyeTarget[kRight].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, 1, GL_RGBA8, true);
    }
}

void SvrApplication::Update()
{
    if (mImGuiInitialized)
    {
        ImGui::NewFrame();
    }
}

void SvrApplication::Shutdown()
{

}

SvrInput& SvrApplication::GetInput()
{
    return mInput;
}

SvrApplicationContext& SvrApplication::GetApplicationContext()
{
    return mAppContext;
}

void SvrApplication::CreateImGuiDeviceObjects()
{
    const GLchar *vertex_shader =
        "#version 300 es\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 300 es\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    mImGuiShaderHandle = GL(glCreateProgram());
    mImGuiVertHandle = GL(glCreateShader(GL_VERTEX_SHADER));
    mImGuiFragHandle = GL(glCreateShader(GL_FRAGMENT_SHADER));
    GL(glShaderSource(mImGuiVertHandle, 1, &vertex_shader, 0));
    GL(glShaderSource(mImGuiFragHandle, 1, &fragment_shader, 0));
    GL(glCompileShader(mImGuiVertHandle));
    GL(glCompileShader(mImGuiFragHandle));
    GL(glAttachShader(mImGuiShaderHandle, mImGuiVertHandle));
    GL(glAttachShader(mImGuiShaderHandle, mImGuiFragHandle));
    GL(glLinkProgram(mImGuiShaderHandle));

    mImGuiAttribLocTex = GL(glGetUniformLocation(mImGuiShaderHandle, "Texture"));
    mImGuiAttribLocProjMtx = GL(glGetUniformLocation(mImGuiShaderHandle, "ProjMtx"));
    mImGuiAttribLocPos = GL(glGetAttribLocation(mImGuiShaderHandle, "Position"));
    mImGuiAttribLocUv = GL(glGetAttribLocation(mImGuiShaderHandle, "UV"));
    mImGuiAttribLocColor = GL(glGetAttribLocation(mImGuiShaderHandle, "Color"));

    GL(glGenBuffers(1, &mImGuiVboHandle));
    GL(glGenBuffers(1, &mImGuiElementsHandle));

    GL(glGenVertexArrays(1, &mImGuiVaoHandle));
    GL(glBindVertexArray(mImGuiVaoHandle));
    GL(glBindBuffer(GL_ARRAY_BUFFER, mImGuiVboHandle));
    GL(glEnableVertexAttribArray(mImGuiAttribLocPos));
    GL(glEnableVertexAttribArray(mImGuiAttribLocUv));
    GL(glEnableVertexAttribArray(mImGuiAttribLocColor));

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    GL(glVertexAttribPointer(mImGuiAttribLocPos, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos)));
    GL(glVertexAttribPointer(mImGuiAttribLocUv, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv)));
    GL(glVertexAttribPointer(mImGuiAttribLocColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col)));
#undef OFFSETOF
    GL(glBindVertexArray(0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));


    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    GL(glGenTextures(1, &mImGuiFontTextureHandle));
    GL(glBindTexture(GL_TEXTURE_2D, mImGuiFontTextureHandle));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)mImGuiFontTextureHandle;

    // Cleanup (don't clear the input data if you want to append new fonts later)
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();
}

void SvrApplication::RenderImGuiDrawLists(ImDrawData* draw_data)
{
    GLint last_program, last_texture;
    GL(glGetIntegerv(GL_CURRENT_PROGRAM, &last_program));
    GL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture));
    GL(glEnable(GL_BLEND));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glDisable(GL_CULL_FACE));
    GL(glDisable(GL_DEPTH_TEST));
    GL(glEnable(GL_SCISSOR_TEST));
    GL(glActiveTexture(GL_TEXTURE0));

    // Setup orthographic projection matrix
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    const float ortho_projection[4][4] =
    {
        { 2.0f / width, 0.0f, 0.0f, 0.0f },
        { 0.0f, 2.0f / -height, 0.0f, 0.0f },
        { 0.0f, 0.0f, -1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f, 1.0f },
    };
    GL(glUseProgram(mImGuiShaderHandle));
    GL(glUniform1i(mImGuiAttribLocTex, 0));
    GL(glUniformMatrix4fv(mImGuiAttribLocProjMtx, 1, GL_FALSE, &ortho_projection[0][0]));
    GL(glBindVertexArray(mImGuiVaoHandle));

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        GL(glBindBuffer(GL_ARRAY_BUFFER, mImGuiVboHandle));
        GL(glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW));

        GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mImGuiElementsHandle));
        GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW));

        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                GL(glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId));
                GL(glScissor((int)pcmd->ClipRect.x, (int)(height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y)));
                GL(glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer_offset));
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }
}

}
