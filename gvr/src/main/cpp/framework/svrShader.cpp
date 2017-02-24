//=============================================================================
// FILE: svrShader.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include "svrShader.h"
#include "svrUtil.h"

namespace Svr
{

static SvrAttribute gSvrDefaultAttributes[] =
{
    { kPosition, "position"},
    { kNormal, "normal"},
    { kColor, "color"},
    { kTexcoord0, "texcoord0"},
    { kTexcoord1, "texcoord1"}
};

bool IsSamplerType(unsigned int type)
{
    if( type == GL_SAMPLER_2D ||
        type == GL_SAMPLER_2D_ARRAY ||
        type == GL_SAMPLER_EXTERNAL_OES ||
        type == GL_SAMPLER_3D ||
        type == GL_SAMPLER_CUBE )
    {
        return true;
    }
    return false;
}

SvrShader::SvrShader()
    : mShaderId(0)
    , mVsId(0)
    , mFsId(0)
{
    mUniformMap.Init(32);
}

bool SvrShader::Initialize(const char* pVertSrc, const char* pFragSrc, const char* pVertDbgName, const char* pFragDbgName)
{
    static char errMsg[4096];
    int result;

    mVsId = GL(glCreateShader( GL_VERTEX_SHADER ));
    GL(glShaderSource( mVsId, 1, &pVertSrc, 0));
    GL(glCompileShader( mVsId ));
    GL(glGetShaderiv( mVsId, GL_COMPILE_STATUS, &result ));
    if( result == GL_FALSE )
    {
        errMsg[0] = 0;
        GL(glGetShaderInfoLog( mVsId, sizeof(errMsg), 0, errMsg));
        LOGE("%s : %s\n", pVertDbgName, errMsg);
        return false;
    }

    mFsId = GL(glCreateShader( GL_FRAGMENT_SHADER ));
    GL(glShaderSource( mFsId, 1, &pFragSrc, 0));
    GL(glCompileShader( mFsId ));
    GL(glGetShaderiv( mFsId, GL_COMPILE_STATUS, &result ));
    if( result == GL_FALSE )
    {
        errMsg[0] = 0;
        GL(glGetShaderInfoLog( mFsId, sizeof(errMsg), 0, errMsg));
        LOGE("%s : %s\n", pFragDbgName, errMsg);
        return false;
    }

    mShaderId = GL(glCreateProgram());
    GL(glAttachShader( mShaderId, mVsId ));
    GL(glAttachShader( mShaderId, mFsId ));

    for ( unsigned int i = 0; i < sizeof( gSvrDefaultAttributes ) / sizeof( gSvrDefaultAttributes[0] ); i++ )
	{
        // LOGI("Shader %d: %s => %d", mShaderId, gSvrDefaultAttributes[i].name, gSvrDefaultAttributes[i].location);
        GL(glBindAttribLocation(mShaderId, gSvrDefaultAttributes[i].location, gSvrDefaultAttributes[i].name));
	}

    GL(glLinkProgram( mShaderId ));
    GL(glGetProgramiv( mShaderId, GL_LINK_STATUS, &result));
    if( result == GL_FALSE )
    {
        errMsg[0] = 0;
        GL(glGetProgramInfoLog( mShaderId, sizeof(errMsg), 0, errMsg));
        LOGE("Link (%s,%s) : %s\n", pVertDbgName, pFragDbgName, errMsg);
        return false;
    }

    // LOGI("Shader Linked: %s <=> %s", pVertDbgName, pFragDbgName);

    int curTextureUnit = 0;
    int nActiveUniforms;
    GL(glGetProgramiv( mShaderId, GL_ACTIVE_UNIFORMS, &nActiveUniforms ));
    LOGI("--madi-- shader: %s, id: %d, active uniforms: %d", pVertDbgName, mShaderId, nActiveUniforms);
    GL( glUseProgram( mShaderId ) );

    char nameBuffer[MAX_UNIFORM_NAME_LENGTH];
    for( int i=0; i < nActiveUniforms; i++ )
    {
        int uniformNameLength, uniformSize;
        GLenum uniformType;
        GL(glGetActiveUniform( mShaderId, i, MAX_UNIFORM_NAME_LENGTH - 1, &uniformNameLength, &uniformSize, &uniformType, &nameBuffer[0]));
        nameBuffer[uniformNameLength] = 0;
        unsigned int location = GL(glGetUniformLocation( mShaderId, nameBuffer ));

        SvrUniform uniform;
        uniform.location = location;
        uniform.type = uniformType;
        strncpy( uniform.name, nameBuffer, MAX_UNIFORM_NAME_LENGTH );

        LOGI("    Uniform (%s) is a type 0x%x => Bound to location %d", nameBuffer, uniformType, location);

        if (IsSamplerType(uniformType))
        {
            GL(glUniform1i( location, curTextureUnit));
            uniform.textureUnit = curTextureUnit++;
            // LOGI("        Uniform (%s) is a texture in unit %d", nameBuffer, uniform.textureUnit);
        }
        else
        {
            uniform.textureUnit = 0;
            // LOGI("        Uniform (%s) is NOT a texture in unit %d", nameBuffer, uniform.textureUnit);
        }

        mUniformMap.Insert( &nameBuffer[0], uniform );
    }

    GL( glUseProgram( 0 ) );

    return true;
}

void SvrShader::Destroy()
{
    if (mShaderId != 0)
    {
        GL(glDeleteProgram(mShaderId));
    }

    if (mVsId != 0)
    {
        GL(glDeleteShader(mVsId));
    }

    if (mFsId != 0)
    {
        GL(glDeleteShader(mFsId));
    }
}

void SvrShader::Bind()
{
    GL( glUseProgram( mShaderId ) );
}

void SvrShader::Unbind()
{
    GL(glUseProgram(0));
}

void SvrShader::SetUniformMat4(const char* name, glm::mat4& matrix)
{
    SvrUniform  uniform;
    if( mUniformMap.Find( name, &uniform ) )
    {
        SetUniformMat4(uniform.location, matrix);
    }
    //else
    //{
    //    LOGE("Failed to set uniform %s on shader %d", name, mShaderId);
    //}
}

void SvrShader::SetUniformMat4(int location, glm::mat4& matrix)
{
    GL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void SvrShader::SetUniformMat4fv(const char* name, unsigned int count, float *pData)
{
    SvrUniform  uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformMat4fv(uniform.location, count, pData);
    }
    //else
    //{
    //    LOGE("Failed to set uniform %s on shader %d", name, mShaderId);
    //}
}

void SvrShader::SetUniformMat4fv(int location, unsigned int count, float *pData)
{
    GL(glUniformMatrix4fv(location, 2, GL_FALSE, pData));
}

void SvrShader::SetUniformVec4(const char* name, glm::vec4& vector)
{
    SvrUniform uniform;
    if(mUniformMap.Find( name, &uniform))
    {
        SetUniformVec4(uniform.location, vector);
    }
    //else
    //{
    //    LOGE("Failed to set uniform %s on shader %d", name, mShaderId);
    //}
}

void SvrShader::SetUniformVec4(int location, glm::vec4& vector)
{
    GL(glUniform4fv(location, 1, glm::value_ptr(vector)));
}

void SvrShader::SetUniformVec3(const char* name, glm::vec3& vector)
{
    SvrUniform uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformVec3(uniform.location, vector);
    }
}

void SvrShader::SetUniformVec3(int location, glm::vec3& vector)
{
    GL(glUniform3fv(location, 1, glm::value_ptr(vector)));
}

void SvrShader::SetUniformVec2(const char* name, glm::vec2& vector)
{
    SvrUniform uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformVec2(uniform.location, vector);
    }
}

void SvrShader::SetUniformVec2(int location, glm::vec2& vector)
{
    GL(glUniform2fv(location, 1, glm::value_ptr(vector)));
}

void SvrShader::SetUniform1ui(const char* name, unsigned int value)
{
    SvrUniform uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniform1ui(uniform.location, value);
    }    
}

void SvrShader::SetUniform1ui(int location, unsigned int value)
{
    GL(glUniform1ui(location, value));
}

void SvrShader::SetUniformSampler(const char* name, unsigned int samplerId, unsigned int samplerType, GLint edgeMode)
{
    SvrUniform uniform;
    if(mUniformMap.Find( name, &uniform))
    {
        // LOGI("    %s: Texture Unit = %d; Sampler = %d; Type = %d", name, uniform.textureUnit, samplerId, samplerType);
        GL(glActiveTexture(GL_TEXTURE0 + uniform.textureUnit));
        GL(glBindTexture(samplerType, samplerId));

        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeMode));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeMode));
    }
    else
    {
        LOGE("Failed to set uniform %s on shader %d", name, mShaderId);
    }
}


}
