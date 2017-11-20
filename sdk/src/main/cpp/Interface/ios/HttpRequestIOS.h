//
//  HttpRequestIOS.h
//  MojingSDK
//
//  Created by 汤申亮 on 16/12/21.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#ifndef HttpRequestIOS_h
#define HttpRequestIOS_h

enum InternetProfileFunction{
    IPF_POST,		//Post mode
    IPF_GET_QUERY,  //Get mode, query
    IPF_GET_UP		//Get mode, insert or update
};

void HttpRequestForIOS(const char* url, const char* params, int eRequestType, int* respcode, char** pRespString);

#endif /* HttpRequestIOS_h */
