#ifndef __DRAWTEX_H__
#define __DRAWTEX_H__

#include "GLES2/gl2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    // Handle to a program object
    GLuint programObject;

    // Sampler location
    GLint samplerLoc;

    // Texture handle
    GLuint textureId;

} UserData;

GLuint createTexture( );
int InitTex ( UserData *userData, int index = 0);
void DrawTex ( UserData *userData);
void ShutDownTex ( UserData *userData );

extern UserData gUserData;

#ifdef __cplusplus
}
#endif

#endif

