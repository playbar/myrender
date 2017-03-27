//
//  Material.h
//  MojingSDK
//
//  Created by Tracy Ma on 16/5/12.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

@interface Material : NSObject

@property (strong) id<MTLFunction> vertexFunction;
@property (strong) id<MTLFunction> fragmentFunction;
@property (strong) id<MTLTexture> diffuseTexture;

- (instancetype)initWithVertexFunction:(id<MTLFunction>)vertexFunction
                      fragmentFunction:(id<MTLFunction>)fragmentFunction;

@end
