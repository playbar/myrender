//
//  Material.m
//  MojingSDK
//
//  Created by Tracy Ma on 16/5/12.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#import "Material.h"

@implementation Material

- (instancetype)initWithVertexFunction:(id<MTLFunction>)vertexFunction
                      fragmentFunction:(id<MTLFunction>)fragmentFunction
{
    if ((self = [super init]))
    {
        _vertexFunction = vertexFunction;
        _fragmentFunction = fragmentFunction;
    }
    return self;
}

@end
