//
//  RenderFrameMetal.cpp
//  MojingSDK
//
//  Created by Tracy Ma on 16/6/6.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#import <Metal/Metal.h>
#include "RenderFrameMetal.h"
#import "SharedTypes.h"

#include "../../MojingAPI.h"

namespace Baofeng
{
    namespace Mojing
    {
        
        RenderFrameMetal::RenderFrameMetal(id<MTLDevice> device, int bufferCount)
        {
            if (device)
            {
                mnBufferCount = bufferCount;
                mUniformBuffers.reserve(mnBufferCount);
                
                id<MTLBuffer> buffer = nullptr;
                for(int i = 0; i < mnBufferCount; i++)
                {
                    buffer = [device newBufferWithLength:sizeof(Uniforms) options:0];
                    
                    buffer.label = [NSString stringWithFormat:@"DistortionUniformBuffer%i", i];
                    
                    mUniformBuffers.push_back(buffer);
                }
                
                if (mUniformBuffers.size() != mnBufferCount)
                {
                    NSLog(@"create timewarp buffers failed!");
                }
                
                // TODO
                std::memset(&m_Uniform, 0, sizeof(Uniforms));
                
                mnMemBarrierIndex = 0;
                mnEncodeIndex     = 0;
            }
            
        }
        /*
         fill the uniform buffer
         */
        bool RenderFrameMetal::Upload()
        {
            id<MTLBuffer> buffer = mUniformBuffers[mnMemBarrierIndex];
            
            uint8_t* pBufferContents = (uint8_t*)[buffer contents];
            
            std::memcpy(pBufferContents, &m_Uniform, sizeof(Uniforms));
            
            return true;
            
        }
        
        /*
         buffer[0],buffer[1],buffer[2], ...
         */
        void RenderFrameMetal::UpdateEncode(id<MTLRenderCommandEncoder> encoder)
        {
            [encoder setVertexBuffer:mUniformBuffers[mnMemBarrierIndex] offset:0 atIndex:0];
            mnMemBarrierIndex = (mnMemBarrierIndex + 1) % mnBufferCount;
        }
        
        /*
         get sensor data, update uniform
         */
        void RenderFrameMetal::Update()
        {
//            simd::float4x4 projectionMatrix = PerspectiveProjection(1, 90 * (M_PI / 180), 0.1, 100);
//            //
//            float sensorVeiw[16];
//            MojingSDK_getLastHeadView(sensorVeiw);
//            matrix_float4x4 sensorMatrix = {
//                vector_float4{sensorVeiw[0], sensorVeiw[1],sensorVeiw[2], sensorVeiw[3]},
//                vector_float4{sensorVeiw[4], sensorVeiw[5], sensorVeiw[6], sensorVeiw[7]},
//                vector_float4{sensorVeiw[8], sensorVeiw[9], sensorVeiw[10], sensorVeiw[11]},
//                vector_float4{sensorVeiw[12], sensorVeiw[13], sensorVeiw[14], sensorVeiw[15]}
//            };
//            
//            sensorMatrix.columns[3].z = -1.5;
//            
//            m_Uniform. = sensorMatrix;
//            self.renderer.modelViewProjectionMatrix = projectionMatrix * sensorMatrix;
        }
    }
}
