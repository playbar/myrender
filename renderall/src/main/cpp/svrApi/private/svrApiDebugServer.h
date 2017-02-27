//=============================================================================
// FILE: svrApiDebugServer.h
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_DEBUGSERVER_H_
#define _SVR_API_DEBUGSERVER_H_ 

#include "cJSON.h"

typedef int(*svrDebugRpcFn)(cJSON *params, void* data);

bool    svrStartDebugServer();
bool    svrStopDebugServer();
int     svrRegisterDebugFn(svrDebugRpcFn fnPtr, char const* name, void* data);

#endif //_SVR_API_DEBUGSERVER_H_ 