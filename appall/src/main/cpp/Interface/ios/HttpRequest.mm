//
//  HttpRequest.mm
//
//  Created by 汤申亮 on 16/12/20.
//  Copyright (c) 2016年 baofeng. All rights reserved.
//

#include "../../Base/BuildType.h"
#include "HttpRequestIOS.h"
#import "HttpRequest.h"

struct InternetReturnBuffer
{
    unsigned int m_uiBufferSize;
    unsigned char m_pBuffer[1];
};

@interface HttpRequest ()

@end

@implementation HttpRequest

+ (instancetype)shareHttpRequest
{
    static id instance = nil;
    static dispatch_once_t predicate;
    dispatch_once(&predicate, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (void)POST:(NSString*)url parms:(NSString*)parms completionHandler:(void (^)(NSData* __nullable dataStr, NSInteger respStatusCode, NSError* __nullable error))completionHandler
{
    [self request:url parms:parms requestMethod:@"POST" completionHandler:completionHandler];
}

- (void)GET:(NSString*)url parms:(NSString*)parms completionHandler:(void (^)(NSData* __nullable dataStr, NSInteger respStatusCode, NSError* __nullable error))completionHandler
{
    [self request:url parms:parms requestMethod:@"GET" completionHandler:completionHandler];
}

- (void)request:(NSString*)url parms:(NSString*)parms requestMethod:(NSString*)requestMethod completionHandler:(void (^)(NSData* __nullable dataStr, NSInteger respStatusCode, NSError* __nullable error))completionHandler
{
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0); //创建信号量
    
    NSMutableURLRequest* mutableRequest = nil;
    NSMutableString* muStr = [NSMutableString stringWithString:url];
    if([requestMethod isEqualToString:@"GET"])
    {
        if (parms != nil && parms != NULL && parms.length != 0) {
            if([url rangeOfString:@"?"].location == NSNotFound)
            {
                [muStr appendString:@"?"];
            }
            parms = [parms stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
            [muStr appendString:parms];
        }
        mutableRequest = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:muStr]];
    }
    else
    {
        mutableRequest = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:muStr]];
        NSMutableData *postBody = [NSMutableData data];
        [postBody appendData:[parms dataUsingEncoding:NSUTF8StringEncoding]];
        [mutableRequest setHTTPBody:postBody];
    }
    mutableRequest.timeoutInterval = 60;
    mutableRequest.HTTPMethod = requestMethod;
    
#ifdef _DEBUG
    NSLog(@"HTTPMethod : %@ , URL : %@", requestMethod, muStr);
#endif
    
    NSURLSession* session = [NSURLSession sharedSession];
    NSURLSessionDataTask* task = [session dataTaskWithRequest:mutableRequest completionHandler:^(NSData* data, NSURLResponse* response, NSError* error) {
#ifdef _DEBUG
        NSLog(@"response : %@", response);
#endif
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse*)response;
#ifdef _DEBUG
        NSLog(@" http response status code: %ld, length = %ld", (long)httpResponse.statusCode, (long)httpResponse.expectedContentLength);
#endif
        /*
        NSString* dataStr;
        if (data) {
            if([url rangeOfString:@"glassconfigout"].location == NSNotFound)
            {
                dataStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
            }
            else
            {
                Byte *dataByte = (Byte *)[data bytes];
                NSData *adata = [[NSData alloc] initWithBytes:dataByte length:data.length];
                int len = (int)adata.length;
            }
        }
         */
        
        if(completionHandler != nil && data != nil)
        {
#ifdef _DEBUG
            NSLog(@"resp data : %@", [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding]);
#endif
            completionHandler(data, httpResponse.statusCode, error);
        }
        dispatch_semaphore_signal(semaphore);
    }];
    
    [task resume];
    
    dispatch_semaphore_wait(semaphore,DISPATCH_TIME_FOREVER);  //等待
    //NSLog(@"Http request finished.");
}
@end

void HttpRequestForIOS(const char* url, const char* params, int eRequestType, int* respcode, char** pRespString)
{
    NSString* strURL = [NSString stringWithUTF8String:url];
    NSString* strParams = [NSString stringWithUTF8String:params];
    
    if(eRequestType == IPF_POST)
    {
        [[HttpRequest shareHttpRequest] POST:strURL parms:strParams completionHandler:^(NSData* _Nullable dataStr, NSInteger respStatusCode, NSError* _Nullable error) {
            //dispatch_sync(dispatch_get_main_queue(), ^{
                if (error) {
                    NSLog(@"Request failed, url: %@,  type: %d, error msg : %@", strURL, eRequestType, error.domain);
                    (*respcode) = (int)error.code;
                }
                else {
                    (*respcode) = (int)respStatusCode;
#ifdef _DEBUG
                    NSLog(@"Request %@, url: %@, type: %d, response status code: %d", *respcode == 200 ? @"succeed" : @"failed",  strURL, eRequestType, *respcode);
#endif
                }
            //});
        }];
    }
    else
    {
        [[HttpRequest shareHttpRequest] GET:strURL parms:strParams completionHandler:^(NSData* _Nullable dataStr, NSInteger respStatusCode, NSError* _Nullable error) {
            //dispatch_sync(dispatch_get_main_queue(), ^{
                if (error) {
                    NSLog(@"Request failed, url: %@,  type: %d, error msg : %@", strURL, eRequestType, error.domain);
                    (*respcode) = (int)error.code;
                }
                else
                {
                    (*respcode) = (int)respStatusCode;
#ifdef _DEBUG
                    NSLog(@"Request %@, url: %@, type: %d, response status code: %d", *respcode == 200 ? @"succeed" : @"failed",  strURL, eRequestType, *respcode);
#endif
                    if(*respcode == 200)
                    {
                        if(eRequestType == IPF_GET_QUERY && dataStr != nil)
                        {
                            Byte *dataByte = (Byte *)[dataStr bytes];
                            if(dataByte != NULL)
                            {
                                size_t len = dataStr.length;
                                size_t iBufferSize = len + sizeof (InternetReturnBuffer);
                                *pRespString = new char[iBufferSize];
                                InternetReturnBuffer *pRetBuffer = (InternetReturnBuffer *)(*pRespString);
                                memset(pRetBuffer, 0, iBufferSize);
                                memcpy(pRetBuffer->m_pBuffer, dataByte, len);
                                pRetBuffer->m_uiBufferSize = (unsigned int)len;
                            }
                        }
                    }
                }
            //});
        }];
    }
}
