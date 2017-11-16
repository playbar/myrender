//
//  SharedTypes.h
//  MojingSDK
//
//  Created by Tracy Ma on 16/5/12.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#ifndef SharedTypes_h
#define SharedTypes_h

#include <simd/simd.h>

struct Vertex
{
    simd::float4 position;
    simd::float2 normal;
    simd::float2 uv0;
    simd::float2 tangent;
    simd::float2 uv1;
};

struct Vertex_Mojing
{
    simd::float4 position;
    simd::float2 dR;
    simd::float2 dG;
    simd::float2 dB;
    simd::float2 texcoord;
};

struct Uniforms
{
    simd::float4x4 modelViewProjectionMatrix;
    simd::float4x4 Texm;
    simd::float4x4 Texm2;
};

typedef uint16_t IndexType;


struct Vertex_line
{
    simd::float4 point;
};

struct Uniforms_line
{
    simd::float4x4 mvp;
};

struct Uniforms_overlay
{
    simd::float4x4 mvp;
    simd::float4x4 Texm;
};

#endif /* SharedTypes_h */
