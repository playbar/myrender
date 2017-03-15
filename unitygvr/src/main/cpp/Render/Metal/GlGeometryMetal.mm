//
//  GlGeometryMetal.cpp
//  MojingSDK
//
//  Created by Tracy Ma on 16/5/12.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#import <Metal/Metal.h>
#include "GlGeometryMetal.h"
#include "../../MojingManager.h"
#include "../../Distortion/MojingDistortion.h"

#include "SharedTypes.h"

namespace Baofeng
{
    #define CELLS_COUNT 32
    namespace Mojing
    {
        // ------------- GeometryTrianglesMetal ---------------
        
        GeometryTrianglesMetal::GeometryTrianglesMetal(id<MTLDevice> device)
        {
            metalDevice = device;
            BuildGeometry();
        }
        
        bool GeometryTrianglesMetal::IsAvailable()
        {
            return (metalDevice != nullptr) && (indexBuffer != nullptr) && (vertexBuffer != nullptr);
        }
        
        bool GeometryTrianglesMetal::BuildGeometry()
        {
            
            const int NUM_SLICES_PER_EYE = 1;
            const float fovScale = 1.0f;
            
            void* pBuffer = Manager::GetMojingManager()->GetDistortion()->BuildDistortionBuffer(CELLS_COUNT, CELLS_COUNT);
            
            //const int magic = ((int *)pBuffer)[0];
            const int tesselationsX = ((int *)pBuffer)[1];
            const int tesselationsY = ((int *)pBuffer)[2];
            
            //const int vertexBytes = 12 + 2 * (tesselationsX + 1) * (tesselationsY + 1) * 6 * sizeof(float);
            const float * bufferVerts = &((float *)pBuffer)[3];
            
            const int attribCount = 12; // Metal: 12
            const int sliceTess = tesselationsX / NUM_SLICES_PER_EYE;
            
            const int vertexCount = 2 * NUM_SLICES_PER_EYE*(sliceTess + 1)*(tesselationsY + 1);
            const int floatCount = vertexCount * attribCount;
            float* pTessVertices = new float[floatCount];
            
            int	verts = 0;
            for (int eye = 0; eye < 2; eye++)
            {
                for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
                {
                    const int vertBase = verts;
                    for (int y = 0; y <= tesselationsY; y++)
                    {
                        const float	yf = (float)y / (float)tesselationsY;
                        for (int x = 0; x <= sliceTess; x++)
                        {
                            const int sx = slice * sliceTess + x;
                            const float	xf = (float)sx / (float)tesselationsX;
                            float * v = &pTessVertices[attribCount * (vertBase + y * (sliceTess + 1) + x)];
                            // Metal vertex data
                            // position(float, float, float, float)
                            // R(float, float),
                            // G(float, float),
                            // B(float, float)
                            // A(float, float)
                            v[0] = -1.0f + eye + xf;	// 调整 x 坐标范围，从 ［0，1］到 左眼 [-1, 0], 右眼 [0, 1]
                            v[1] = yf * 2.0f - 1.0f;	// 调整 y 坐标范围，从 ［0，1］到［－1，1］
                            v[2] = 0.0f;                // Metal position 必须得是 float4
                            v[3] = 1.0f;
                            
                            // R, G, B
                            for (int i = 0; i < 6; i++)
                            {
                                v[4 + i] = fovScale * bufferVerts
                                [(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + i];
                            }

                            v[10] = (float)x / sliceTess;
                            // Enable this to allow fading at the edges.
                            // Samsung recommends not doing this, because it could cause
                            // visible differences in pixel wear on the screen over long
                            // periods of time.
                            
                            if ((y == 0 || y == tesselationsY || sx == 0 || sx == tesselationsX))
                            {
                                v[11] = 0.0f;	// fade to black at edge
                            }
                            else
                            {
                                v[11] = bufferVerts[(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + 6];
                            }
                            v[11] = 1.0f;
                        }
                    }
                    verts += (tesselationsY + 1)*(sliceTess + 1);
                }
            }
            free(pBuffer);
            
            const int indexCount = 2 * tesselationsX * tesselationsY * 6;
            unsigned short* pTessIndices = new unsigned short[indexCount];
            
            int	index = 0;
            verts = 0;
            for (int eye = 0; eye < 2; eye++)
            {
                for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
                {
                    const int vertBase = verts;
                    // The order of triangles doesn't matter for tiled rendering,
                    // but when we can do direct rendering to the screen, we want the
                    // order to follow the raster order to minimize the chance
                    // of tear lines.
                    //
                    // This can be checked by quartering the number of indexes, and
                    // making sure that the drawn pixels are the first pixels that
                    // the raster will scan.
                    // ÏÂÃæµÄ´úÂëÓÃÓÚ»æÖÆ¸÷¸ö¾ØÐÎ£¬Ê¹ÓÃÁù¸ö¶¥µã½«Ò»¸ö¾ØÐÎÇÐ·Ö³ÉÁ½¸öÈý½ÇÐÎ£»
                    // ²¢ÇÒ·Ö±ð²ÉÓÃÁË´Ó×óÉÏµ½ÓÒÏÂºÍ´ÓÓÒÉÏµ½×óÏÂÁ½ÖÖ·Ö¸î¾ØÐÎµÄ·½Ê½·Ö¸î²»Í¬ÏóÏÞÖÐµÄ¾ØÐÎ¡£
                    // ÒÔÏÂ×¢ÊÍËùÃèÊöµÄÈý½ÇÐÎ·½ÏòºÍÏóÏÞ¾ùÊÇÒÔ×óÉÏ½ÇÎªÔ­µã×ø±ê
                    for (int x = 0; x < sliceTess; x++)
                    {
                        for (int y = 0; y < tesselationsY; y++)
                        {
                            // flip the triangulation in opposite corners
                            if ((slice*sliceTess + x < tesselationsX / 2) ^ (y < (tesselationsY / 2)))
                            {
                                // µÚÒ»ÈýÏóÏÞ£¬Ð±±ß´Ó×óÉÏ½Çµ½ÓÒÏÂ½Ç,0Óë3¡¢2Óë4ÖØµþ
                                /*
                                 03 - - -1
                                 | \     |
                                 |   \   |
                                 |     \ |
                                 5 - - - 24
                                 */
                                pTessIndices[index + 0] = vertBase + y * (sliceTess + 1) + x;
                                pTessIndices[index + 1] = vertBase + y * (sliceTess + 1) + x + 1;
                                pTessIndices[index + 2] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;
                                
                                pTessIndices[index + 3] = vertBase + y * (sliceTess + 1) + x;
                                pTessIndices[index + 4] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;
                                pTessIndices[index + 5] = vertBase + (y + 1) * (sliceTess + 1) + x;
                            }
                            else
                            {
                                // µÚ¶þËÄÏóÏÞ£¬Ð±±ß´ÓÓÒÉÏ½Çµ½×óÏÂ½Ç,1Óë4¡¢2Óë3ÖØµþ
                                /*
                                 0  - - -14
                                 |     / |
                                 |   /   |
                                 | /     |
                                 23- - - 5
                                 */
                                pTessIndices[index + 0] = vertBase + y * (sliceTess + 1) + x;
                                pTessIndices[index + 1] = vertBase + y * (sliceTess + 1) + x + 1;
                                pTessIndices[index + 2] = vertBase + (y + 1) * (sliceTess + 1) + x;
                                
                                pTessIndices[index + 3] = vertBase + (y + 1) * (sliceTess + 1) + x;
                                pTessIndices[index + 4] = vertBase + y * (sliceTess + 1) + x + 1;
                                pTessIndices[index + 5] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;
                            }
                            index += 6;
                        }
                    }
                    verts += (tesselationsY + 1)*(sliceTess + 1);
                }
            }

            vertexBuffer = [metalDevice newBufferWithBytes:pTessVertices length:floatCount * sizeof(*pTessVertices) options:MTLResourceOptionCPUCacheModeDefault];
            indexBuffer = [metalDevice newBufferWithBytes: pTessIndices length: indexCount * sizeof(*pTessIndices) options: MTLResourceOptionCPUCacheModeDefault];

            delete[] pTessVertices;
            delete[] pTessIndices;
            
            // DEBUG: load buffers from file
            if (/* DISABLES CODE */ (0))
            {
                FILE* fIndexBuffer;
                FILE* fVertexBuffer;
                const char* pathIndexBuffer = [[[NSBundle mainBundle] pathForResource:@"indexbuffer" ofType:@"bin"] UTF8String];
                const char* pathVertexBuffer = [[[NSBundle mainBundle] pathForResource:@"vertexbuffer" ofType:@"bin"] UTF8String];
                fIndexBuffer = fopen(pathIndexBuffer, "rb");
                fVertexBuffer = fopen(pathVertexBuffer, "rb");
                
                fseek(fIndexBuffer, 0, SEEK_END);
                long ibuffersize = ftell(fIndexBuffer);
                rewind(fIndexBuffer);
                
                fseek(fVertexBuffer, 0, SEEK_END);
                long vbuffersize = ftell(fVertexBuffer);
                rewind(fVertexBuffer);
                
                char* iBuffer = (char*)malloc(sizeof(char) * ibuffersize);
                char* vBuffer = (char*)malloc(sizeof(char) * vbuffersize);
                
                size_t result = fread(iBuffer, sizeof(char), ibuffersize, fIndexBuffer);
                if (result != ibuffersize) {
                    NSLog(@"read ibuffer error");
                } else {
                    NSLog(@"ibuffer size: %zu", result);
                }
                
                
                result = fread(vBuffer, sizeof(char), vbuffersize, fVertexBuffer);
                if (result != vbuffersize) {
                    NSLog(@"read vbuffer error");
                } else {
                    NSLog(@"vbuffer size: %zu", result);
                }
                
                indexBuffer = [metalDevice newBufferWithBytes:iBuffer length:ibuffersize options:MTLResourceOptionCPUCacheModeDefault];
                vertexBuffer = [metalDevice newBufferWithBytes: vBuffer length: vbuffersize options: MTLResourceOptionCPUCacheModeDefault];
                
                free(iBuffer);
                free(vBuffer);
            }

            
            return true;
        } // end of BuildGeometry
        
        /*
         Center line
         */
        GeometryLineMetal::GeometryLineMetal(id<MTLDevice> device)
        {
            float points[4 * 2] = {
                0,1,0,1,
                0,-1,0,1,
            };
            vertexBuffer = [device newBufferWithBytes:points length:sizeof(float) * 4 * 2 options:MTLResourceOptionCPUCacheModeDefault];
        }
        
    }// Mojing
}// Baofeng