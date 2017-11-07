//
//  HttpRequest.h
//
//  Created by 汤申亮 on 16/12/20.
//  Copyright (c) 2016年 baofeng. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HttpRequest : NSObject

+ (instancetype)shareHttpRequest;

- (void)POST:(NSString*)url parms:(NSString*)parms completionHandler:(void (^)(NSData* __nullable dataStr, NSInteger respStatusCode, NSError* __nullable error))completionHandler;

- (void)GET:(NSString*)url parms:(NSString*)parms completionHandler:(void (^)(NSData* __nullable dataStr, NSInteger respStatusCode, NSError* __nullable error))completionHandler;

@end
