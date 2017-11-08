#pragma  once

#include "../Base/GlUtils.h"
#include "../Base/MojingLog.h"
#ifdef MJ_OS_ANDROID
#include <jni.h>
#include <android/log.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef MJ_OS_ANDROID
#include "../3rdPart/ktx/include/ktx.h"
#endif

#include "FBO.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

#define DEBUG_PIC

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif
namespace Baofeng {
    namespace Mojing
    {
        
        static void checkGlErrorForSDK(const char* op)
        {
            MOJING_ERROR(g_APIlogger, "checkGlErrorForSDK");
            char errorstr[1024] = { 0 };
            for (GLint error = glGetError(); error; error = glGetError())
            {
                sprintf(errorstr, "after %s() glError (0x%x)\n", op, error);
                MOJING_ERROR(g_APIlogger, errorstr);
            }
            
            
            //	MOJING_ERROR(g_APIlogger, EglErrorString());
            
        }
    }
}
