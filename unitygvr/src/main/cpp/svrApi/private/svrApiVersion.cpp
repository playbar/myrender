//=============================================================================
// FILE: svrApiVersion.cpp
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================

#include <stdlib.h>
#include <stdio.h>

#include "svrApiCore.h"

//-----------------------------------------------------------------------------
const char* svrGetVersion()
//-----------------------------------------------------------------------------
{
    static char versionBuffer[256];
    snprintf(versionBuffer, 256, "%d.%d.%d, %s - %s", SVR_MAJOR_VERSION, SVR_MINOR_VERSION, SVR_REVISION_VERSION, __DATE__, __TIME__);
    return &versionBuffer[0];
}