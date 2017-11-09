//
//  MojingRenderMetal.h
//  MojingSDK
//
//  Created by Tracy Ma on 16/5/12.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#ifndef MojingRenderMetal_hpp
#define MojingRenderMetal_hpp

#include <TargetConditionals.h>
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)

#import <simd/simd.h>

#import "MojingRenderBase.h"
//#import "GlGeometryMetal.h"
//#import "RenderFrameMetal.h"

#import "MetalForwardDecls.h"

struct IUnityGraphicsMetal; // unity interface

// TODO: 参数冗余
namespace Baofeng
{
    namespace Mojing
    {
        class GeometryTrianglesMetal;
        class GeometryLineMetal;
        
        class RenderFrameMetal;
        
        class MojingRenderMetal : public MojingRenderBase
        {
        public:
            MojingRenderMetal(bool bEnableMultiThread,
                              bool bEnableTimeWarp,
                              IUnityGraphicsMetal* metalGraphics,
                              MTLDeviceRef mtlDevice,
                              MTLCommandQueueRef mtlCommandQueue,
                              MTLCommandBufferRef mtlCommandBuffer);
            
            void OnModifyed() override;
            void GenDistortionMesh();
            
            
            static MojingRenderMetal* CreateCurrentRenderMetal(bool bEnableMultiThread,
                                                               bool bEnableTimeWarp,
                                                               IUnityGraphicsMetal* metalGraphics,
                                                               MTLDeviceRef device,
                                                               MTLCommandQueueRef cmdQueue,
                                                               MTLCommandBufferRef cmdBuffer);
            static MojingRenderMetal* GetInstance();

            void SetEyeTextureMetal(MTLTextureRef leftEyeTexture, MTLTextureRef rightEyeTexture);
            bool WarpToScreen(CAMetalDrawableRef drawable,
                              MTLTextureRef eyeTextureLeft,
                              MTLTextureRef eyeTextureRight,
                              MTLTextureRef overlayTextureLeft,
                              MTLTextureRef overlayTextureRight);
            
            void SetLeftOverlayRect(vector_float4 rect);
            void SetRightOverlayRect(vector_float4 rect);
            
            static void Release();

        private:
            bool DrawDistortion(CAMetalDrawableRef drawable,
                                MTLTextureRef eyeTextureLeft,
                                MTLTextureRef eyeTextureRight,
                                MTLTextureRef layoutTextureLeft,
                                MTLTextureRef layoutTextureRight);
            bool DrawDistortionGUI();
            
        private:
            static MojingRenderMetal*         _instance;
            
            IUnityGraphicsMetal*            _unityMetalInterface;
            MTLDeviceRef                    _mtlDevice;
            MTLCommandQueueRef              _mtlCommandQueue; // TODO
            MTLCommandBufferRef             _mtlCommandBuffer;
            MTLCommandEncoderRef            _mtlCommandEncoder;
            /*
             Distortion
             */
            MTLFunctionRef                  _distortionVertexFunction;
            MTLFunctionRef                  _distortionFragmentFunction;
            MTLDepthStencilStateRef         _distortionDepthState;
            MTLRenderPassDescriptor          *_distortionRenderPass; // distortion, overlay, center line in one pass
            MTLRenderPipelineStateRef      _distortionPipelineState;
            MTLBufferRef                   _distortionUniformBuffer;
            MTLTextureRef                  _distortionDepthTexture;
            MTLSamplerStateRef             _distortionSamplerState;
            MTLTextureRef                  _cellTexture; // when there is no texture provided, use this
            /*
             Overlay
             */
            MTLFunctionRef                 _overlayVertexFunction;
            MTLFunctionRef                 _overlayFragmentFunction;
            MTLRenderPipelineStateRef      _overlayPipelineState;
            MTLBufferRef                   _overlayUniformBufferLeft;
            MTLBufferRef                   _overlayUniformBufferRight;
            
            GeometryTrianglesMetal* _mesh = nullptr;
            
            vector_float4                   _leftOverlayRect;
            vector_float4                   _rightOverlayRect;
            
            /*
             Center line
             */
            GeometryLineMetal* _line;
            MTLRenderPipelineStateRef      _linestripPipelineState;
            MTLBufferRef                   _linestripUniformBuffer;
            void SetupLinestripPipeline();
            void SetupLinestripUniforms();
            /*
             timewarp
             */
            RenderFrameMetal*                _renderFrame;
            bool                            _enableTimewarp;
            bool                            _enableMultithread;
            
//            dispatch_semaphore_t  _semaphore;
            
            void CreateDepthBuffer();
            void SetupUniforms();
            void UpdateRenderPass(CAMetalDrawableRef drawable);
            void SetupDistortionPipeline();

            void SetupOverlayUniforms(bool left, vector_float4 leftOverlayRect);
            void SetupOverlayPipeline();
            
        };
    }
}


#endif // MJ_IOS_METAL
#endif /* MojingRenderMetal_hpp */
