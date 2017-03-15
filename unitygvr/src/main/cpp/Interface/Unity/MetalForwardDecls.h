//
//  MetalForwardDecls.h
//  MojingSDK
//
//  Created by Tracy Ma on 16/6/15.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#ifndef MetalForwardDecls_h
#define MetalForwardDecls_h

#ifdef __OBJC__
#ifdef __cplusplus
#define OBJC_OBJECT_PTR	__strong
#else
#define OBJC_OBJECT_PTR	__unsafe_unretained
#endif
#else
#define OBJC_OBJECT_PTR
#endif


#ifdef __OBJC__
@class UIScreen;
@class UIWindow;
@class UIView;
@class UIViewController;
@class UIEvent;
@class UILocalNotification;
@class NSString;
@class NSDictionary;
@class NSSet;
@class NSData;
@class NSError;
@class NSBundle;

@class UnityViewControllerBase;
#else
typedef struct objc_object UIScreen;
typedef struct objc_object UIWindow;
typedef struct objc_object UIView;
typedef struct objc_object UIViewController;
typedef struct objc_object UIEvent;
typedef struct objc_object UILocalNotification;
typedef struct objc_object NSString;
typedef struct objc_object NSDictionary;
typedef struct objc_object NSSet;
typedef struct objc_object NSError;
typedef struct objc_object NSData;
typedef struct objc_object NSBundle;

typedef struct objc_object UnityViewControllerBase;
#endif

/*
 Metal
 */
#ifdef __OBJC__
@class CAMetalLayer;
@class MTLRenderPassDescriptor;

@protocol CAMetalDrawable;
@protocol MTLDrawable;
@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLBuffer;
@protocol MTLCommandBuffer;
@protocol MTLCommandQueue;
@protocol MTLCommandEncoder;
@protocol MTLRenderCommandEncoder;
@protocol MTLFunction;
@protocol MTLDepthStencilState;
@protocol MTLRenderPipelineState;
@protocol MTLSamplerState;

typedef id<CAMetalDrawable> CAMetalDrawableRef;
typedef id<MTLDrawable> MTLDrawableRef;
typedef id<MTLDevice> MTLDeviceRef;
typedef id<MTLTexture> MTLTextureRef;
typedef id<MTLBuffer> MTLBufferRef;
typedef id<MTLCommandBuffer> MTLCommandBufferRef;
typedef id<MTLCommandQueue> MTLCommandQueueRef;
typedef id<MTLCommandEncoder> MTLCommandEncoderRef;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderRef;
typedef id<MTLFunction> MTLFunctionRef;
typedef id<MTLDepthStencilState> MTLDepthStencilStateRef;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStateRef;
typedef id<MTLSamplerState> MTLSamplerStateRef;
#else
typedef struct objc_object CAMetalLayer;
typedef struct objc_object MTLRenderPassDescriptor;

typedef struct objc_object* CAMetalDrawableRef;
typedef struct objc_object* MTLDrawableRef;
typedef struct objc_object* MTLDeviceRef;
typedef struct objc_object* MTLTextureRef;
typedef struct objc_object* MTLBufferRef;
typedef struct objc_object* MTLCommandBufferRef;
typedef struct objc_object* MTLCommandQueueRef;
typedef struct objc_object* MTLCommandEncoderRef;
typedef struct objc_object* MTLRenderCommandEncoderRef;
typedef struct objc_object* MTLFunctionRef;
typedef struct objc_object* MTLDepthStencilStateRef;
typedef struct objc_object* MTLRenderPipelineStateRef;
typedef struct objc_object* MTLSamplerStateRef;
#endif

#endif /* MetalForwardDecls_h */
