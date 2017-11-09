#ifndef __DRAWTEX_H__
#define __DRAWTEX_H__

#include <GLES3/gl3.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    GLuint programObject;
    GLint samplerLoc;
    GLuint textureId;
    GLuint vboID;
    GLuint iboID;

} UserData;

GLuint createTexture( );
void InitData();
int InitTex ( UserData *userData, int index = 0);
void DrawTex ( UserData *userData);
void ShutDownTex ( UserData *userData );

extern UserData gUserData;

#ifdef __cplusplus
}
#endif

#endif

