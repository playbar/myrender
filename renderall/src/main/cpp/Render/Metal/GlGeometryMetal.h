//
//  GlGeometryMetal.h
//  MojingSDK
//
//  Created by Tracy Ma on 16/5/12.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#ifndef GlGeometryMetal_h
#define GlGeometryMetal_h

#import <Metal/Metal.h>

namespace Baofeng
{
    namespace Mojing
    {
        class GeometryTrianglesMetal
        {
        public:
            GeometryTrianglesMetal(id<MTLDevice> device);
            bool BuildGeometry();
            bool IsAvailable();
        private:
            id<MTLDevice> metalDevice;
        public:
            id<MTLBuffer> vertexBuffer;
            id<MTLBuffer> indexBuffer;
            id<MTLFunction> vertexShader;
            id<MTLFunction> fragmentShader;
        };
        
        class GeometryLineMetal
        {
        public:
            GeometryLineMetal(id<MTLDevice> device);
            
            id<MTLBuffer> vertexBuffer;
            id<MTLFunction> vertexShader;
            id<MTLFunction> fragmentShader;
        };
    }
}


#endif /* GlGeometryMetal_hpp */
