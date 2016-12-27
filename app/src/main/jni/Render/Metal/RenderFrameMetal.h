//
//  RenderFrameMetal.h
//  MojingSDK
//
//  Created by Tracy Ma on 16/6/6.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#ifndef RenderFrameMetal_h
#define RenderFrameMetal_h

#include <vector>

#import <Metal/Metal.h>
#import <simd/simd.h>
#import "SharedTypes.h"

namespace Baofeng
{
    namespace Mojing
    {
        class RenderFrameMetal
        {
        public:
            RenderFrameMetal(id<MTLDevice> device, int capacity);
            void UpdateEncode(id<MTLRenderCommandEncoder> encoder);
            bool Upload();
            void Update();
            
        private:
            Uniforms    m_Uniform; // tri-buffer
            matrix_float4x4 m_Oreitation;
            
            std::vector<id<MTLBuffer>> mUniformBuffers; // timewarp buffers
            
            uint32_t  mnBufferCount;
            uint32_t  mnMemBarrierIndex;
            uint32_t  mnEncodeIndex;
        };
    }
}
#endif /* RenderFrameMetal_h */
