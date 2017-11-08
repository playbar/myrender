//
//  MojingRenderMetal.cpp
//  MojingSDK
//
//  Created by Tracy Ma on 16/5/12.
//  Copyright © 2016年 baofeng. All rights reserved.
//
#include <TargetConditionals.h>
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include "Vsync.h"
#include "GlGeometryMetal.h"
#include "RenderFrameMetal.h"
#include "MojingRenderMetal.h"
#include "../../Base/MojingTypes.h"
#include "../VerifyFailedInfo.h"
#include "../VersionLow.h"
#include "../../MojingManager.h"
#include "../../Distortion/MojingDistortion.h"
#include "../../Parameters/MojingDisplayParameters.h"
#include "../../Parameters/MojingParameters.h"
#include "../../MojingSDKStatus.h"

#import "../../Interface/Unity/Unity/IUnityGraphicsMetal.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#import "SharedTypes.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif


#define USE_UNITY_COMMANDBUFFER 1

namespace Baofeng
{
    namespace Mojing
    {
        MojingRenderMetal* MojingRenderMetal::_instance = nullptr;
        
        // 获取单例
        MojingRenderMetal* MojingRenderMetal::CreateCurrentRenderMetal(
                                                                       bool bEnableMultiThread,
                                                                       bool bEnableTimeWarp,
                                                                       IUnityGraphicsMetal* metalGraphics,
                                                                       MTLDeviceRef device,
                                                                       MTLCommandQueueRef cmdQueue,
                                                                       MTLCommandBufferRef cmdBuffer)
        {
            if(_instance != nullptr)
            {
                return _instance;
            }
            else
            {
                _instance = new MojingRenderMetal(bEnableMultiThread, bEnableTimeWarp,metalGraphics, device, cmdQueue, cmdBuffer);
                
                
                return _instance;
            }
        }
        
        // TODO: need to check return value is nullptr
        MojingRenderMetal* MojingRenderMetal::GetInstance()
        {
//            NSCAssert(_instance != nullptr, @"Metal renderer has NOT been created, \
//                        call MojingSDK_API_EnterMojingWorld_Metal to create one");
            // if (_instance == nullptr)
            // {
            //     NSLog(@"Metal renderer has NOT been created, \
            //             call MojingSDK_API_EnterMojingWorld_Metal to create one");
            // }
            return _instance;
        }
        
        void MojingRenderMetal::Release()
        {
            delete _instance;
            _instance = nullptr;
        }
        
        void MojingRenderMetal::OnModifyed()
        {
            MOJING_FUNC_TRACE(g_APIlogger);
            MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
            m_bLandscape = pDisplayParameters->GetScreenWidth() > pDisplayParameters->GetScreenHeight();
            m_iScreenWidth = pDisplayParameters->GetScreenWidth();
            m_iScreenHeight = pDisplayParameters->GetScreenHeight();
            
            m_iScreenWidth_Half = m_iScreenWidth / 2;
            m_iScreenHeight_Half = m_iScreenHeight / 2;
            
            // create warpmesh
            GenDistortionMesh();

            g_bIsModifyed = false;
        }
        
        MojingRenderMetal::MojingRenderMetal(bool bEnableMultithread,
                                             bool bEnableTimewarp,
                                             IUnityGraphicsMetal* metalGraphics,
                                             MTLDeviceRef mtlDevice,
                                             MTLCommandQueueRef cmdQueue,
                                             MTLCommandBufferRef cmdBuffer)
        :
        _enableMultithread(bEnableMultithread),
        _enableTimewarp(bEnableTimewarp),
        _leftOverlayRect(vector_float4{0,0,0,0}),
        _rightOverlayRect(vector_float4{0,0,0,0})
        
        {
            _unityMetalInterface = metalGraphics;

            {
                if (mtlDevice)
                {
                    _mtlDevice = mtlDevice;
                }
                else
                {
                    if (_unityMetalInterface)
                    {
                        _mtlDevice = _unityMetalInterface->MetalDevice();
                    }
                    else
                    {
                        NSLog(@"Error: metal device nust not be null");
                    }
                }
                
                /// 优先使用调用者的 cmdQueue, 没有的话创建一个
                if (cmdQueue)
                {
                    _mtlCommandQueue = cmdQueue;
                }
                else
                {
                    _mtlCommandQueue = [_mtlDevice newCommandQueue]; // create new cmd queue
                    NSLog(@"MetalRender: create cmdQueue %p use mtlDevice %p", _mtlCommandQueue, _mtlDevice);
                }
                
                /// 优先使用调用者的 cmdBuffer, 没有的话创建一个
                if (cmdBuffer)
                {
                    _mtlCommandBuffer = cmdBuffer;
                }
                else
                {
                    // We create cmdBuffer in render loop
                }
                
                
            }
            
            _mtlCommandEncoder = nullptr;

            NSString* distortionShader = @
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "\n"
            "struct Uniforms\n"
            "{\n"
            "    float4x4 modelViewProjectionMatrix;\n"
            "    float4x4 Texm;\n"
            "    float4x4 Texm2;\n"
            "};\n"
            "\n"
            "struct Vertex\n"
            "{\n"
            "    float4 position [[attribute(0)]];\n"
            "    float2 dR [[attribute(1)]];\n"
            "    float2 dG [[attribute(2)]];\n"
            "    float2 dB [[attribute(3)]];\n"
            "    float2 texcoord [[attribute(4)]];\n"
            "};\n"
            "\n"
            "struct ProjectedVertex\n"
            "{\n"
            "    float4 position [[position]];\n"
            "    float2 oTexCoord1r;\n"
            "    float2 oTexCoord1g;\n"
            "    float2 oTexCoord1b;\n"
            "    float intensity;\n"
            "};\n"
            "\n"
            "vertex ProjectedVertex vertex_main_MojingDistortion(Vertex vert [[stage_in]],\n"
            "                                   constant Uniforms &uniforms [[buffer(1)]])\n"
            "{\n"
            "    ProjectedVertex outVert;\n"
            "    outVert.position = uniforms.modelViewProjectionMatrix * vert.position;\n"
            "    float3 proj;\n"
            "    float projIZ;\n"
            "    proj = float3( uniforms.Texm * float4(vert.dR,-1,1) );\n"
            "    projIZ = 1.0 / max( proj.z, 0.00001 );\n"
            "    outVert.oTexCoord1r = float2( proj.x * projIZ, proj.y * projIZ );\n"
            "    proj = float3( uniforms.Texm * float4(vert.dG,-1,1) );\n"
            "    projIZ = 1.0 / max( proj.z, 0.00001 );\n"
            "    outVert.oTexCoord1g = float2( proj.x * projIZ, proj.y * projIZ );\n"
            "    proj = float3( uniforms.Texm * float4(vert.dB,-1,1) );\n"
            "    projIZ = 1.0 / max( proj.z, 0.00001 );\n"
            "    outVert.oTexCoord1b = float2( proj.x * projIZ, proj.y * projIZ );\n"
            "    outVert.intensity = vert.texcoord.y;\n"
            "    return outVert;\n"
            "}\n"
            "\n"
            "fragment float4 fragment_main_MojingDistortion(ProjectedVertex vert [[stage_in]],\n"
            "                              texture2d<float> diffuseTexture [[texture(0)]],\n"
            "                              sampler samplr [[sampler(0)]])\n"
            "{\n"
            "    float4 color1r = diffuseTexture.sample(samplr, vert.oTexCoord1r);\n"
            "    float4 color1g = diffuseTexture.sample(samplr, vert.oTexCoord1g);\n"
            "    float4 color1b = diffuseTexture.sample(samplr, vert.oTexCoord1b);\n"
            "    return vert.intensity * float4( color1r.x, color1g.y, color1b.z, 1.0);\n"
            "}\n"
            ;
            
            NSString* overlayShader = @
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "\n"
            "struct Uniforms_overlay\n"
            "{\n"
            "    float4x4 mvp;\n"
            "    float4x4 Texm;\n"
            "};\n"
            "\n"
            "struct Vertex\n"
            "{\n"
            "    float4 position [[attribute(0)]];\n"
            "    float2 dR [[attribute(1)]];\n"
            "    float2 dG [[attribute(2)]];\n"
            "    float2 dB [[attribute(3)]];\n"
            "    float2 texcoord [[attribute(4)]];\n"
            "};\n"
            "\n"
            "struct ProjectedVertex\n"
            "{\n"
            "    float4 position [[position]];\n"
            "    float2 oTexCoord1r;\n"
            "    float2 oTexCoord1g;\n"
            "    float2 oTexCoord1b;\n"
            "    float intensity;\n"
            "};\n"
            "\n"
            "vertex ProjectedVertex vertex_main_MojingDistortionOverlay(Vertex vert [[stage_in]],\n"
            "                                   constant Uniforms_overlay &uniforms [[buffer(1)]])\n"
            "{\n"
            "    ProjectedVertex outVert;\n"
            "    outVert.position = uniforms.mvp * vert.position;\n"
            "    float3 proj;\n"
            "    float projIZ;\n"
            "    proj = float3( uniforms.Texm * float4(vert.dR,1,1) );\n"
            "    projIZ = 1.0 / max( proj.z, 0.00001 );\n"
            "    outVert.oTexCoord1r = float2( proj.x * projIZ, proj.y * projIZ );\n"
            "    proj = float3( uniforms.Texm * float4(vert.dG,1,1) );\n"
            "    projIZ = 1.0 / max( proj.z, 0.00001 );\n"
            "    outVert.oTexCoord1g = float2( proj.x * projIZ, proj.y * projIZ );\n"
            "    proj = float3( uniforms.Texm * float4(vert.dB,1,1) );\n"
            "    projIZ = 1.0 / max( proj.z, 0.00001 );\n"
            "    outVert.oTexCoord1b = float2( proj.x * projIZ, proj.y * projIZ );\n"
            "    outVert.intensity = vert.texcoord.y;\n"
            "    return outVert;\n"
            "}\n"
            "\n"
            "fragment float4 fragment_main_MojingDistortionOverlay(ProjectedVertex vert [[stage_in]],\n"
            "                              texture2d<float> diffuseTexture [[texture(0)]],\n"
            "                              sampler samplr [[sampler(0)]])\n"
            "{\n"
            "    float4 color1r = diffuseTexture.sample(samplr, vert.oTexCoord1r);\n"
            "    float4 color1g = diffuseTexture.sample(samplr, vert.oTexCoord1g);\n"
            "    float4 color1b = diffuseTexture.sample(samplr, vert.oTexCoord1b);\n"
            "    return vert.intensity * float4( color1r.x, color1g.y, color1b.z, color1r.w);\n"
            "}\n"
            ;
            
            NSString* lineShader = @
            "#include <metal_stdlib>\n"
            "using namespace metal;\n"
            "\n"
            "struct Uniforms\n"
            "{\n"
            "    float4x4 modelViewProjectionMatrix;\n"
            "};\n"
            "\n"
            "struct Vertex\n"
            "{\n"
            "    float4 position [[attribute(0)]];\n"
            "};\n"
            "\n"
            "struct ProjectedVertex\n"
            "{\n"
            "    float4 position [[position]];\n"
            "};\n"
            "\n"
            "vertex ProjectedVertex vertex_main_Linestrip(Vertex vert [[stage_in]],\n"
            "                                   constant Uniforms &uniforms [[buffer(1)]])\n"
            "{\n"
            "    ProjectedVertex outVert;\n"
            "    outVert.position = uniforms.modelViewProjectionMatrix * vert.position;\n"
            "    return outVert;\n"
            "}\n"
            "\n"
            "fragment float4 fragment_main_Linestrip(ProjectedVertex vert [[stage_in]])\n"
            "{\n"
            "    return float4{1,1,1,1};\n"
            "}\n";
            
            NSError* errors = nil;
            
            id<MTLLibrary> mtlLibrary = [_mtlDevice newLibraryWithSource:distortionShader options:nil error:&errors];
            if (errors != nil)
            {
                NSLog(@"get error when compile shader: %@", errors.description);
                assert(0);
            }
            
            MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
            samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
            samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToZero;
            samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToZero;
            
            _distortionSamplerState = [_mtlDevice newSamplerStateWithDescriptor:samplerDescriptor];
            
            _distortionVertexFunction = [mtlLibrary newFunctionWithName:@"vertex_main_MojingDistortion"];
            if (!_distortionVertexFunction)
            {
                NSLog(@"Could not load vertex function named: vertex_main_MojingDistortion ");
            }
            
            _distortionFragmentFunction = [mtlLibrary newFunctionWithName:@"fragment_main_MojingDistortion"];
            if (!_distortionFragmentFunction)
            {
                NSLog(@"Could not load fragment function named: fragment_main_MojingDistortion ");
            }
            
            /*
             Overlay
             */
            mtlLibrary = [_mtlDevice newLibraryWithSource:overlayShader options:nil error:&errors];
            
            if (errors != nil)
            {
                NSLog(@"get error when compile shader: %@", errors.description);
                assert(0);
            }
            
            _overlayVertexFunction = [mtlLibrary newFunctionWithName:@"vertex_main_MojingDistortionOverlay"];
            if (!_overlayVertexFunction)
            {
                NSLog(@"Error: build Metal gui vertex shader");
            }
            _overlayFragmentFunction = [mtlLibrary newFunctionWithName:@"fragment_main_MojingDistortionOverlay"];
            if (!_overlayFragmentFunction)
            {
                NSLog(@"Error: build Metal gui fragment shader");
            }
            
            /*
             Center line
             */
            _line = new GeometryLineMetal(_mtlDevice);

            mtlLibrary = [_mtlDevice newLibraryWithSource:lineShader options:nil error:&errors];
            
            if (errors != nil)
            {
                NSLog(@"get error when compile shader: %@", errors.description);
                assert(0);
            }
            
            _line->vertexShader = [mtlLibrary newFunctionWithName:@"vertex_main_Linestrip"];
            if (!_overlayVertexFunction)
            {
                NSLog(@"Error: build Metal linestrip vertex shader");
            }
            _line->fragmentShader = [mtlLibrary newFunctionWithName:@"fragment_main_Linestrip"];
            if (!_overlayFragmentFunction)
            {
                NSLog(@"Error: build Metal linestrip fragment shader");
            }
            
            ///
            SetupDistortionPipeline();
            SetupOverlayPipeline();
            SetupLinestripPipeline();

            SetupUniforms();
            SetupLinestripUniforms();

            
        }
        
        // TODO: if there is no overlay texture, we do NOT need to setup this pipeline
        void MojingRenderMetal::SetupDistortionPipeline()
        {
            MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
            NSError *error = nil;
            
            vertexDescriptor.attributes[0].format = MTLVertexFormatFloat4;
            vertexDescriptor.attributes[0].bufferIndex = 0;
            vertexDescriptor.attributes[0].offset = offsetof(Vertex_Mojing, position);
            
            vertexDescriptor.attributes[1].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[1].bufferIndex = 0;
            vertexDescriptor.attributes[1].offset = offsetof(Vertex_Mojing, dR);
            
            vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[2].bufferIndex = 0;
            vertexDescriptor.attributes[2].offset = offsetof(Vertex_Mojing, dG);
            
            vertexDescriptor.attributes[3].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[3].bufferIndex = 0;
            vertexDescriptor.attributes[3].offset = offsetof(Vertex_Mojing, dB);
            
            vertexDescriptor.attributes[4].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[4].bufferIndex = 0;
            vertexDescriptor.attributes[4].offset = offsetof(Vertex_Mojing, texcoord);
            
            vertexDescriptor.layouts[0].stride = sizeof(Vertex_Mojing);
            vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
            
            
            MTLRenderPipelineDescriptor *pipelineDescriptor = [MTLRenderPipelineDescriptor new];
            pipelineDescriptor.vertexFunction = _distortionVertexFunction;
            pipelineDescriptor.fragmentFunction = _distortionFragmentFunction;
            pipelineDescriptor.vertexDescriptor = vertexDescriptor;
            
            pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

#ifdef USE_UNITY_COMMANDBUFFER
//            pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
//            pipelineDescriptor.stencilAttachmentPixelFormat = MTLPixelFormatStencil8;
#else
            pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
#endif
            


            
            
            
            _distortionPipelineState = [_mtlDevice newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                                               error:&error];
            
            if (!_distortionPipelineState)
            {
                NSLog(@"Error occurred when creating render pipeline state: %@", error);
            }
            else
            {
                NSLog(@"distortion pipeline setup ok!");
            }
        }
        
        void MojingRenderMetal::SetupLinestripPipeline()
        {
            MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
            
            vertexDescriptor.attributes[0].format = MTLVertexFormatFloat4;
            vertexDescriptor.attributes[0].bufferIndex = 0;
            vertexDescriptor.attributes[0].offset = offsetof(Vertex_line, point);
            
            
            vertexDescriptor.layouts[0].stride = sizeof(Vertex_line);
            vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
            
            MTLRenderPipelineDescriptor *pipelineDescriptor = [MTLRenderPipelineDescriptor new];
            pipelineDescriptor.vertexFunction = _line->vertexShader;
            pipelineDescriptor.fragmentFunction = _line->fragmentShader;
            pipelineDescriptor.vertexDescriptor = vertexDescriptor;
            
            pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

#ifdef USE_UNITY_COMMANDBUFFER
//            pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
//            pipelineDescriptor.stencilAttachmentPixelFormat = MTLPixelFormatStencil8;
#else
            pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
#endif
            
            NSError *error = nil;
            _linestripPipelineState = [_mtlDevice newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                                               error:&error];
            
            if (!_linestripPipelineState)
            {
                NSLog(@"Error occurred when creating render pipeline state: %@", error);
            }
            else
            {
                NSLog(@"linestrip pipiline setup ok!");
            }

        }
        
        void MojingRenderMetal::SetupOverlayPipeline()
        {
            MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
            
            vertexDescriptor.attributes[0].format = MTLVertexFormatFloat4;
            vertexDescriptor.attributes[0].bufferIndex = 0;
            vertexDescriptor.attributes[0].offset = offsetof(Vertex_Mojing, position);
            
            vertexDescriptor.attributes[1].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[1].bufferIndex = 0;
            vertexDescriptor.attributes[1].offset = offsetof(Vertex_Mojing, dR);
            
            vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[2].bufferIndex = 0;
            vertexDescriptor.attributes[2].offset = offsetof(Vertex_Mojing, dG);
            
            vertexDescriptor.attributes[3].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[3].bufferIndex = 0;
            vertexDescriptor.attributes[3].offset = offsetof(Vertex_Mojing, dB);
            
            vertexDescriptor.attributes[4].format = MTLVertexFormatFloat2;
            vertexDescriptor.attributes[4].bufferIndex = 0;
            vertexDescriptor.attributes[4].offset = offsetof(Vertex_Mojing, texcoord);
            
            vertexDescriptor.layouts[0].stride = sizeof(Vertex_Mojing);
            vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
            
            MTLRenderPipelineDescriptor *pipelineDescriptor = [MTLRenderPipelineDescriptor new];
            pipelineDescriptor.vertexFunction = _overlayVertexFunction;
            pipelineDescriptor.fragmentFunction = _overlayFragmentFunction;
            pipelineDescriptor.vertexDescriptor = vertexDescriptor;
            
            pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
            pipelineDescriptor.colorAttachments[0].blendingEnabled = true;
            pipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
            pipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
            pipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            pipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

#ifdef USE_UNITY_COMMANDBUFFER
//            pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
//            pipelineDescriptor.stencilAttachmentPixelFormat = MTLPixelFormatStencil8;
#else
            pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
#endif
            
            NSError *error = nil;
            _overlayPipelineState = [_mtlDevice newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                                        error:&error];
            
            if (!_overlayPipelineState)
            {
                NSLog(@"Error occurred when creating render pipeline state: %@", error);
            }
            else
            {
                NSLog(@"overlay pipiline setup ok!");
            }
        }
        
        void MojingRenderMetal::GenDistortionMesh()
        {
            _mesh = new GeometryTrianglesMetal(_mtlDevice);
            NSLog(@"mesh data:%lu, %lu", (unsigned long)[_mesh->vertexBuffer length], (unsigned long)[_mesh->indexBuffer length]);
        }
        
        void MojingRenderMetal::SetupUniforms()
        {
            if (!_distortionUniformBuffer)
            {
                _distortionUniformBuffer = [_mtlDevice newBufferWithLength:sizeof(Uniforms)
                                                      options:MTLResourceOptionCPUCacheModeDefault];
            }
            
            Uniforms uniforms;
            uniforms.Texm = simd::float4x4{
                simd::float4{0.5, 0, 0, 0},
                simd::float4{0, 0.5, 0, 0},
                simd::float4{-0.5, -0.5, -1, -1},
                simd::float4{0, 0, 0, 0}
            };
            uniforms.Texm2 = simd::float4x4{
                simd::float4{0.5, 0, 0, 0},
                simd::float4{0, 0.5, 0, 0},
                simd::float4{-0.5, -0.5, -1, -1},
                simd::float4{0, 0, 0, 0}
            };
            uniforms.modelViewProjectionMatrix = simd::float4x4{
                simd::float4{1.0f, 0.0f, 0.0f, 0.0f},
                simd::float4{0.0f, 1.0f, 0.0f, 0.0f},
                simd::float4{0.0f, 0.0f, 0.0f, 0.0f},
                simd::float4{0.0f, 0.0f, 0.0f, 1.0f}
            };
            
            memcpy([_distortionUniformBuffer contents], &uniforms, sizeof(Uniforms));
        }
        
        void MojingRenderMetal::SetupLinestripUniforms()
        {
            if (!_linestripUniformBuffer)
            {
                _linestripUniformBuffer = [_mtlDevice newBufferWithLength:sizeof(Uniforms_line) options:MTLResourceOptionCPUCacheModeDefault];
            }
            Uniforms_line uniforms{
                simd::float4x4{
                    simd::float4{1.0f, 0.0f, 0.0f, 0.0f},
                    simd::float4{0.0f, 1.0f, 0.0f, 0.0f},
                    simd::float4{0.0f, 0.0f, 0.0f, 0.0f},
                    simd::float4{0.0f, 0.0f, 0.0f, 1.0f}
                }
            };
            memcpy([_linestripUniformBuffer contents], &uniforms, sizeof(Uniforms_line));
        }
        
        void MojingRenderMetal::SetLeftOverlayRect(vector_float4 rect)
        {
            _leftOverlayRect = rect;
        }
        
        void MojingRenderMetal::SetRightOverlayRect(vector_float4 rect)
        {
            _rightOverlayRect = rect;
        }
        
        // TODO: optimise
        matrix_float4x4 GUI_Matrix4f(vector_float4 v4fGUIRect)
        {
            MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
            enum X_Axis_Order
            {
                XO_RIGHT_BIGGER,
                XO_LEFT_BIGGER
            }XOrder = XO_RIGHT_BIGGER;
            enum X_Axis
            {
                XA_REVERSAL,
                //XO_CENTER,
                XA_NORMAL
            }XOrigin = XA_REVERSAL;
            enum Y_Axis_Order
            {
                YO_UP_BIGGER,
                YO_DOWN_BIGGER
            }YOrder = YO_DOWN_BIGGER;
            enum Y_Axis
            {
                YA_REVERSAL,
                //YO_CENTER,
                YA_NORMAL
            }YOrigin = YA_REVERSAL;
            vector_float3 OverlayPosition = {v4fGUIRect.x ,v4fGUIRect.y , 0};
            vector_float3 OverlayScale = {v4fGUIRect.z ,v4fGUIRect.w , 0};

            

#if 1
            // HOME 在右的时候
            XOrder = XO_LEFT_BIGGER;
            XOrigin = XA_NORMAL;
            YOrder = YO_DOWN_BIGGER;
            YOrigin = YA_REVERSAL;
            
//            if (pStatus->GetEngineStatus() == ENGINE_UNITY)
//            {
//                XOrder = XO_RIGHT_BIGGER;
//                XOrigin = XA_REVERSAL;
//                YOrder = YO_UP_BIGGER;
//                YOrigin = YA_REVERSAL;
//            }
            if (pStatus->GetEngineStatus() == ENGINE_UNITY)
            {
                YOrigin = YA_NORMAL;
            }
#else
            // HOME 在左的时候
            XOrder = XO_RIGHT_BIGGER;
            XOrigin = XA_REVERSAL;
            YOrder = YO_UP_BIGGER;
            YOrigin = YA_REVERSAL;
            
            if (pStatus->GetEngineStatus() == ENGINE_UNITY)
            {
                XOrder = XO_LEFT_BIGGER;
                XOrigin = XA_NORMAL;
                YOrder = YO_DOWN_BIGGER;
                YOrigin = YA_NORMAL;
            }
#endif
            /*
             注意：	以下M44矩阵将在Shader中与表示[-1,1]区间的色散畸变颜色分量系数V4(x,y,-1,1)相乘
             即M44 * V4 ，得到原始图的颜色采样坐标
             */
            // 基准矩阵，将图像缩放到正好满屏
            matrix_float4x4 SingleEye = {
                vector_float4{0.5, 0.0, 0.0, 0.0},
                vector_float4{0.0, 0.5, 0.0, 0.0},
                vector_float4{0.5, 0.5, 1.0, 0.0},
                vector_float4{0.0, 0.0, 0.0, 0.0}
            };
            
            matrix_float4x4 SingleEye_Scale = matrix_identity_float4x4;// 对上面的矩阵做缩放和轴旋转
            matrix_float4x4 ScaleToTarget = matrix_identity_float4x4;// 用来将缩放后的矩阵移动到左上角，再移动到目标位置上
            // 防止除零错
            OverlayScale.x = fmax(0.000001, OverlayScale.x);
            OverlayScale.y = fmax(0.000001, OverlayScale.y);
            
            SingleEye_Scale.columns[0][0] = 1 / (OverlayScale.x);
            SingleEye_Scale.columns[1][1] = 1 / (OverlayScale.y);
            
            if (XOrigin == XA_REVERSAL)
            {
                SingleEye_Scale.columns[0][0] = -SingleEye_Scale.columns[0][0];
            }
            
            if (YOrigin == YA_REVERSAL)
            {
                SingleEye_Scale.columns[1][1] = -SingleEye_Scale.columns[1][1];
            }
            if (XOrder == XO_LEFT_BIGGER)
            {
                ScaleToTarget.columns[3][0] = 1 - OverlayScale.x - OverlayPosition.x * 2;
            }
            else
            {
                ScaleToTarget.columns[3][0] = OverlayScale.x - 1 + OverlayPosition.x * 2;
            }
            if (YOrder == YO_DOWN_BIGGER)
            {
                ScaleToTarget.columns[3][1] = OverlayScale.y - 1 + OverlayPosition.y * 2;
            }
            else
            {
                ScaleToTarget.columns[3][1] = 1 - OverlayScale.y - OverlayPosition.y * 2;
            }

            // 1 缩放
            SingleEye = matrix_multiply(SingleEye, SingleEye_Scale);
            // 2 移动到移动到目标位置
            SingleEye = matrix_multiply(SingleEye, ScaleToTarget);
            
            return SingleEye;
        }
        
        void MojingRenderMetal::SetupOverlayUniforms(bool left, vector_float4 overlayRect)
        {
            if (!_overlayUniformBufferLeft)
            {
                _overlayUniformBufferLeft = [_mtlDevice newBufferWithLength:sizeof(Uniforms_overlay)
                                                                   options:MTLResourceOptionCPUCacheModeDefault];
            }
            if (!_overlayUniformBufferRight)
            {
                _overlayUniformBufferRight = [_mtlDevice newBufferWithLength:sizeof(Uniforms_overlay)
                                                                     options:MTLResourceOptionCPUCacheModeDefault];
            }
            
            Uniforms_overlay uniforms;
            
            /*
             Left
             */
            matrix_float4x4 gui_mat = GUI_Matrix4f(overlayRect);
            
            uniforms.Texm = simd::float4x4{
                simd::float4{
                    gui_mat.columns[0][0],
                    gui_mat.columns[0][1],
                    gui_mat.columns[0][2],
                    gui_mat.columns[0][3]
                },
                simd::float4{
                    gui_mat.columns[1][0],
                    gui_mat.columns[1][1],
                    gui_mat.columns[1][2],
                    gui_mat.columns[1][3]
                },
                simd::float4{
                    gui_mat.columns[2][0],
                    gui_mat.columns[2][1],
                    gui_mat.columns[2][2],
                    gui_mat.columns[2][3]
                },
                simd::float4{
                    gui_mat.columns[3][0],
                    gui_mat.columns[3][1],
                    gui_mat.columns[3][2],
                    gui_mat.columns[3][3]
                }
            };
            
//            if (_unityMetalInterface)
//            {
//                uniforms.mvp = simd::float4x4{
//                    simd::float4{1.0f, 0.0f, 0.0f, 0.0f},
//                    simd::float4{0.0f, 1.0f, 0.0f, 0.0f},
//                    simd::float4{0.0f, 0.0f, 0.0f, 0.0f},
//                    simd::float4{0.0f, 0.0f, 0.0f, 1.0f}
//                };
//            }
//            else
            {
                uniforms.mvp = simd::float4x4{
                    simd::float4{1.0f, 0.0f, 0.0f, 0.0f},
                    simd::float4{0.0f, 1.0f, 0.0f, 0.0f},
                    simd::float4{0.0f, 0.0f, 0.0f, 0.0f},
                    simd::float4{0.0f, 0.0f, 0.0f, 1.0f}
                };
            }
            
            if (left)
            {
                memcpy([_overlayUniformBufferLeft contents], &uniforms, sizeof(Uniforms_overlay));
            }
            else
            {
                memcpy([_overlayUniformBufferRight contents], &uniforms, sizeof(Uniforms_overlay));
            }
            
        }
        
        id<MTLTexture> CreateCellsMetal(MTLDeviceRef device, int iImageWidth, int iImageHeight, int iCellWidth, int iLineWidth, int iBKCol, int iLineCol, float fPPI, float fMetersPerTanAngleAtCenter)
        {
            char* pBuffer = new char[iImageWidth * iImageHeight * 4];
            // FILE  *file = fopen("/sdcard/tt.txt", "wt");
            /*计算R^2/10*/
            int iRt = iImageHeight / 2;
            iRt *= iRt;
            iRt /= 10;
            // fprintf(file, "Width = %d  Rt = %d \n ix , iy , iR , iTest\n", iImageHeight, iRt);
            SDK_VERIFY  status = MojingSDKStatus::GetSDKStatus()->GetVerfiyStatus();
            if (status != VERIFY_OK)
            {
                for (int iY = 1; iY < iImageHeight - 1; iY++)
                {
                    int* lpLenBuffer = (int*)pBuffer + iImageWidth * iY;
                    for (int iX = 1; iX < iImageWidth - 1; iX++)
                    {
                        lpLenBuffer[iX] = (iBKCol & 0x00FFFFFF);
                    }
                }
                int width = 0;
                int height = 0;
                const unsigned char *pInfo = NULL;
                if (status == VERIFY_INVALID)
                {
                    width = INFO_VERIFYFAILED_WIDTH;
                    height = INFO_VERIFYFAILED_HEIGHT;
                    pInfo = VerifyFailedInfo;
                }
                else if (status == VERIFY_VERSION_LOW)
                {
                    width = INFO_VERSIONLOW_WIDTH;
                    height = INFO_VERSIONLOW_HEIGHT;
                    pInfo = SDKVersionLowInfo;
                }
                
                if (pInfo != NULL)
                {
                    for (int iY = 0; iY < height; iY++)
                    {
                        int* lpLenBuffer = (int*)pBuffer + iImageWidth * (iY + (iImageHeight - height) / 2) + (iImageWidth - width) / 2;
                        for (int iX = 0; iX < width / 8; iX++)
                        {
                            int offset = iY*(width / 8) + iX;
                            unsigned char c = *(pInfo + offset);
                            offset = 8 * iX;
                            for (int i = 0; i < 8; i++)
                            {
                                if (((c >> (7 - i)) & 0x01) == 0)
                                    lpLenBuffer[offset + i] = (iBKCol | 0x00FFFFFF);
                            }
                        }
                    }
                }
            }
            else
            {
                
                // 最边上一圈需要黑色,所以循环边界要各缩放1 像素
                for (int iY = 1; iY < iImageHeight - 1; iY++)
                {
                    int iTestY = iY - iImageHeight / 2;
                    
                    int* lpLenBuffer = (int*)pBuffer + iImageWidth * iY;
                    for (int iX = 1; iX < iImageWidth - 1; iX++)
                    {
                        lpLenBuffer[iX] = (iBKCol & 0x00FFFFFF);
                        
                        int iTestX = iX - iImageHeight / 2;
                        
                        int iTestR = iTestX *iTestX + iTestY * iTestY;
                        int iTest = iTestR;
                        int iT = 1024;
                        if (iTest > iRt)
                            iTest %= iRt;
                        iTest = abs(iTest - iRt);
                        if ((iLineWidth > (abs((iY + iLineWidth / 2) - (iImageHeight / 2)) % iCellWidth)) || // 1 Y轴判定横线
                            (iLineWidth > (abs((iX + iLineWidth / 2) - (iImageWidth / 2)) % iCellWidth)) ||// 2 X轴判定纵线
                            iLineWidth / 2 > abs(abs(iX - iImageWidth / 2) - abs(iY - iImageHeight / 2)) // 3 对角线
                            || iTest < iT)
                        {
                            lpLenBuffer[iX] = iLineCol | 0xFF000000;
                        }
                    }
                }
            }
            // 			if (file)
            // 				fclose(file);
            
            //	memset(pBuffer, 128, iImageWidth * iImageHeight * 4);
            /////////////////////////////////////////////////////////////////////////

            //创建一个MTLTexture对象的各个属性
            const NSUInteger bytesPerPixel = 4;
            const NSUInteger bytesPerRow = bytesPerPixel * iImageWidth;
//            const NSUInteger bitsPerComponent = 8;
            
            MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                                                            width: iImageWidth                     height: iImageHeight         mipmapped: true];
            //创建一个新的纹理对象
            MTLTextureRef  texture=[device newTextureWithDescriptor: desc];
            MTLRegion region=MTLRegionMake2D(0, 0, int(iImageWidth), int(iImageHeight));
            [texture replaceRegion:region mipmapLevel:0 slice:0 withBytes:pBuffer bytesPerRow:bytesPerRow bytesPerImage:bytesPerRow * iImageHeight];
            
            delete[] pBuffer;
            return texture;
        }
        
        void MojingRenderMetal::UpdateRenderPass(CAMetalDrawableRef drawable)
        {
                /*
                 Distortion RenderPass
                 */
                _distortionRenderPass = [MTLRenderPassDescriptor renderPassDescriptor];
                
                _distortionRenderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
                _distortionRenderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
                _distortionRenderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);

            _distortionRenderPass.colorAttachments[0].texture = drawable.texture; // the metal layer to draw on
            
        }
        
        bool MojingRenderMetal::DrawDistortion(
                                               CAMetalDrawableRef drawable,
                                               MTLTextureRef eyeTextureLeft,
                                               MTLTextureRef eyeTextureRight,
                                               MTLTextureRef overlayTextureLeft,
                                               MTLTextureRef overlayTextureRight
                                            )
        {
            
            if (drawable == nullptr)
            {
                NSLog(@"Error: drawable is null");
                return false;
            }
            
            if ((_mesh == nullptr) || g_bIsModifyed)
            {
                OnModifyed();
            }
            
            // if there is no eye texture, create cell texture
            if (eyeTextureLeft == nullptr || eyeTextureRight == nullptr)
            {
                MojingDisplayParameters * pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
                int iWidth = pDisplayParameters->GetScreenWidth();
                iWidth = fmin(iWidth, pDisplayParameters->GetScreenHeight());
                
                if (iWidth > 1024)
                    iWidth = 1024;
                int iLineColor = -1;
                if (MojingSDKStatus::GetSDKStatus()->GetEngineStatus() == ENGINE_UNITY)
                {// Unity引擎的绘制文理不是我们管理的，我们没法预期它的内容什么时候被释放。所以为了防止出现不预期的网格图，这里直接返回掉不画了
                    iLineColor = 0;
                }
                _cellTexture = CreateCellsMetal(_mtlDevice, iWidth, iWidth, 40, 4, 0, iLineColor, pDisplayParameters->GetPPI(), Manager::GetMojingManager()->GetDistortion()->GetMetersPerTanAngleAtCenter());
                
            }
            
            UpdateRenderPass(drawable);
            
            /* 
             Distortion Mesh
             */
            /// when use custom encoder we have to end unity's encoder
            if (_unityMetalInterface != nullptr)
            {
                _unityMetalInterface->EndCurrentCommandEncoder();
                _mtlCommandBuffer = _unityMetalInterface->CurrentCommandBuffer();
            }
            else
            {
                _mtlCommandBuffer = [_mtlCommandQueue commandBuffer];
            }


            id<MTLRenderCommandEncoder> commandEncoder;
            
            commandEncoder = [_mtlCommandBuffer renderCommandEncoderWithDescriptor:_distortionRenderPass];
            

            [commandEncoder pushDebugGroup:@"Distortion pass"];
            commandEncoder.label = @"distortion";
            
            [commandEncoder setVertexBuffer:_mesh->vertexBuffer offset:0 atIndex:0];
            
            if (!_enableTimewarp)
            {
                [commandEncoder setVertexBuffer:_distortionUniformBuffer offset:0 atIndex:1];
            }
            else
            {
                _renderFrame->UpdateEncode(commandEncoder);
            }
            
            [commandEncoder setFragmentTexture:eyeTextureLeft == nullptr ? _cellTexture : eyeTextureLeft atIndex:0];
            [commandEncoder setFragmentSamplerState:_distortionSamplerState atIndex:0];
            
            [commandEncoder setRenderPipelineState:_distortionPipelineState];
            [commandEncoder setCullMode:MTLCullModeNone];
            [commandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
            
            // draw left eye
            [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                       indexCount:[_mesh->indexBuffer length] / (sizeof(UInt16) * 2)
                                        indexType:MTLIndexTypeUInt16
                                      indexBuffer:_mesh->indexBuffer
                                indexBufferOffset:0];
            
            // draw right eye
            [commandEncoder setFragmentTexture:eyeTextureRight == nullptr ? _cellTexture : eyeTextureRight atIndex:0];
            
            [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                       indexCount:[_mesh->indexBuffer length] / (sizeof(UInt16) * 2)
                                        indexType:MTLIndexTypeUInt16
                                      indexBuffer:_mesh->indexBuffer
                                indexBufferOffset:[_mesh->indexBuffer length] / (sizeof(UInt16))];
            
            /*
             Overlay
             */
            if ((overlayTextureLeft != nullptr || overlayTextureRight != nullptr) &&
                (_leftOverlayRect.z > 0 && _leftOverlayRect.w > 0 &&
                 _rightOverlayRect.z > 0 && _rightOverlayRect.w > 0)
               )
            {
                SetupOverlayUniforms(true, _leftOverlayRect); // TODO: remove from Update
                [commandEncoder setVertexBuffer:_mesh->vertexBuffer offset:0 atIndex:0];
                [commandEncoder setVertexBuffer:_overlayUniformBufferLeft offset:0 atIndex:1];
                
                [commandEncoder setFragmentTexture:(overlayTextureLeft ? overlayTextureLeft : overlayTextureRight) atIndex:0];
                [commandEncoder setFragmentSamplerState:_distortionSamplerState atIndex:0];
                
                [commandEncoder setRenderPipelineState:_overlayPipelineState]; // with overlay shader
                [commandEncoder setCullMode:MTLCullModeNone];
                [commandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
                
                [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                                  indexCount:[_mesh->indexBuffer length] / (sizeof(UInt16) * 2)
                                                   indexType:MTLIndexTypeUInt16
                                                 indexBuffer:_mesh->indexBuffer
                                           indexBufferOffset: overlayTextureLeft ? 0 : ([_mesh->indexBuffer length] / (sizeof(UInt16)))
                ];

                if (overlayTextureLeft != nullptr && overlayTextureRight != nullptr)
                {
                    SetupOverlayUniforms(false, _rightOverlayRect);
                    
                    [commandEncoder setVertexBuffer:_overlayUniformBufferRight offset:0 atIndex:1];
                    [commandEncoder setFragmentTexture:overlayTextureRight atIndex:0];
                    
                    [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                               indexCount:[_mesh->indexBuffer length] / (sizeof(UInt16) * 2)
                                                indexType:MTLIndexTypeUInt16
                                              indexBuffer:_mesh->indexBuffer
                                        indexBufferOffset:[_mesh->indexBuffer length] / (sizeof(UInt16))];
                }
            }

            [commandEncoder setVertexBuffer:_line->vertexBuffer offset:0 atIndex:0];
            
            [commandEncoder setVertexBuffer:_linestripUniformBuffer offset:0 atIndex:1];
            
            [commandEncoder setRenderPipelineState:_linestripPipelineState];
            [commandEncoder setCullMode:MTLCullModeNone];
            [commandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
            
            [commandEncoder drawPrimitives:MTLPrimitiveTypeLineStrip vertexStart:0 vertexCount:2];
            
            [commandEncoder popDebugGroup]; //
            

            [commandEncoder endEncoding];

            if (!_unityMetalInterface)
            {
                [_mtlCommandBuffer presentDrawable: drawable];
                [_mtlCommandBuffer commit];
            }
            else
            {
                // Unity manage the command buffer
            }
            
 
            
            return true;
        
        } // end of DrawDistortion

        bool MojingRenderMetal::WarpToScreen(
                                             CAMetalDrawableRef drawable,
                                             MTLTextureRef eyeTextureLeft,
                                             MTLTextureRef eyeTextureRight,
                                             MTLTextureRef overlayTextureLeft,
                                             MTLTextureRef overlayTextureRight
                                            )
        {
            DrawDistortion(drawable, eyeTextureLeft, eyeTextureRight, overlayTextureLeft, overlayTextureRight);
            return true;
        }
        
    }
}
#endif
