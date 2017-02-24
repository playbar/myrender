//
//  MojingMetalView.h
//  MojingSDK
//
//  Created by Tracy Ma on 16/6/15.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#ifndef MojingMetalView_h
#define MojingMetalView_h

#include <TargetConditionals.h>

#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
#import <QuartzCore/CAMetalLayer.h> // simulator DO NOT support metal
#endif

#import <Metal/Metal.h>
#import <UIKit/UIKit.h>

@interface MojingMetalView : UIView

#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
@property (nonatomic, readonly) id <MTLDevice> device;
@property (nonatomic, readonly) id <CAMetalDrawable> currentDrawable;
@property (nonatomic, readonly) MTLRenderPassDescriptor *renderPassDescriptor;

@property (nonatomic) MTLPixelFormat depthPixelFormat;
@property (nonatomic) MTLPixelFormat stencilPixelFormat;
@property (nonatomic) NSUInteger     sampleCount;

- (void)releaseTextures;

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;

#endif /* MJ_IOS_METAL */

@end


#endif /* MojingMetalView_h */
