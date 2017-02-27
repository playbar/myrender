//=============================================================================
// FILE: svrShader.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#pragma once

#include <GLES3/gl3.h>
#include <GLES/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "svrHashTable.h"

#define MAX_UNIFORM_NAME_LENGTH 64

namespace Svr
{
    enum SvrAttributeLocation
    {
        kPosition = 0,
        kNormal = 1,
        kColor = 2,
        kTexcoord0 = 3,
        kTexcoord1 = 4
    } ;

    struct SvrAttribute
    {
        SvrAttributeLocation    location;
        const char*             name;
    };

    struct SvrUniform
    {
        unsigned int location;
        GLenum       type;
        char         name[MAX_UNIFORM_NAME_LENGTH];
        unsigned int textureUnit;
    };

    class SvrShader
    {
    public:
        SvrShader();

        bool Initialize(const char* pVertSrc, const char* pFragSrc, const char* pVertDbgName, const char* pFragDbgName);
        void Destroy();
        void Bind();
        void Unbind();

        void SetUniformMat4(const char* name, glm::mat4& matrix);
        void SetUniformMat4(int location, glm::mat4& matrix);
        void SetUniformMat4fv(const char* name, unsigned int count, float *pData);
        void SetUniformMat4fv(int location, unsigned int count, float *pData);
        void SetUniformVec4(const char* name, glm::vec4& vector);
        void SetUniformVec4(int location, glm::vec4& vector);
        void SetUniformVec3(const char* name, glm::vec3& vector);
        void SetUniformVec3(int location, glm::vec3& vector);
        void SetUniformVec2(const char* name, glm::vec2& vector);
        void SetUniformVec2(int location, glm::vec2& vector);
        void SetUniform1ui(const char* name, unsigned int value);
        void SetUniform1ui(int location, unsigned int value);
        void SetUniformSampler(const char* name, unsigned int samplerId, unsigned int samplerType, GLint edgeMode = GL_CLAMP_TO_EDGE);

        unsigned int GetShaderId() { return mShaderId;  }

    private:
        typedef HashTable<unsigned int, SvrUniform, DjB2Hash> UniformMap;
        typedef HashTable<unsigned int, unsigned int, DjB2Hash> SamplerMap;

        unsigned int    mShaderId;
        unsigned int    mVsId;
        unsigned int    mFsId;
        UniformMap      mUniformMap;
    };
}
