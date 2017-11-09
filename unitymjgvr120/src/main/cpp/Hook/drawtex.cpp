#include <malloc.h>
#include <cstdlib>
#include <android/log.h>
#include "drawtex.h"

#define LOG_TAG "drawtex"
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

UserData gUserData;

static void CheckGLError(const char* label) {
    int gl_error = glGetError();
    if (gl_error != GL_NO_ERROR) {
        LOGE("GL error @ %s: %d", label, gl_error);
        // Crash immediately to make OpenGL errors obvious.
        abort();
    }
}

GLuint  esLoadShader ( GLenum type, const char *shaderSrc )
{
    GLuint shader;
    GLint compiled;

// Create the shader object
    shader = glCreateShader ( type );

    if ( shader == 0 )
    {
        return 0;
    }

// Load the shader source
    glShaderSource ( shader, 1, &shaderSrc, NULL );

// Compile the shader
    glCompileShader ( shader );

// Check the compile status
    glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

    if ( !compiled )
    {
        GLint infoLen = 0;

        glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 )
        {
            char *infoLog = (char*)malloc ( sizeof ( char ) * infoLen );

            glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
            free ( infoLog );
        }

        glDeleteShader ( shader );
        return 0;
    }

    return shader;

}


//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
//         Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//
GLuint esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc )
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = esLoadShader ( GL_VERTEX_SHADER, vertShaderSrc );

    if ( vertexShader == 0 )
    {
        return 0;
    }

    fragmentShader = esLoadShader ( GL_FRAGMENT_SHADER, fragShaderSrc );

    if ( fragmentShader == 0 )
    {
        glDeleteShader ( vertexShader );
        return 0;
    }

    // Create the program object
    programObject = glCreateProgram ( );

    if ( programObject == 0 )
    {
        return 0;
    }

    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );

    // Link the program
    glLinkProgram ( programObject );

    // Check the link status
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

    if ( !linked )
    {
        GLint infoLen = 0;

        glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 )
        {
            char *infoLog = (char *)malloc ( sizeof ( char ) * infoLen );
            glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
            free ( infoLog );
        }

        glDeleteProgram ( programObject );
        return 0;
    }

    // Free up no longer needed shader resources
    glDeleteShader ( vertexShader );
    glDeleteShader ( fragmentShader );

    return programObject;
}


GLuint createTexture( )
{
    // Texture object handle
    GLuint textureId;

    // 2x2 Image, 3 bytes per pixel (R, G, B)
    GLubyte pixels[4 * 4] =
            {
                    255,   0,   0, 255,// Red
                    0, 255,   0,  255, // Green
                    0,   0, 255, 255, // Blue
                    255, 255,   0, 255  // Yellow
            };

    // Use tightly packed data
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, textureId );

    // Load the texture
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    // Set the filtering mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;

}

void InitData()
{
    static GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
                                   0.0f,  0.0f,        // TexCoord 0
                                   -0.5f, -0.5f, 0.0f,  // Position 1
                                   0.0f,  1.0f,        // TexCoord 1
                                   0.5f, -0.5f, 0.0f,  // Position 2
                                   1.0f,  1.0f,        // TexCoord 2
                                   0.5f,  0.5f, 0.0f,  // Position 3
                                   1.0f,  0.0f         // TexCoord 3
    };
    static GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    glGenBuffers(1, &gUserData.vboID);
    glBindBuffer(GL_ARRAY_BUFFER, gUserData.vboID);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), vVertices, GL_STATIC_DRAW);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers(1, &gUserData.iboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gUserData.iboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    return;

}

///
// Initialize the shader and program object
//
int InitTex( UserData *userData, int index)
{
    char * vShaderStr[] = {
            R"glsl(#version 300 es
                    layout(location = 0) in vec4 a_position;
                    layout(location = 1) in vec2 a_texCoord;
                    out vec2 v_texCoord;
                    void main()
                    {
                       gl_Position = a_position;
                       v_texCoord = a_texCoord;
                    })glsl",

            R"glsl(#version 300 es
                    #extension GL_OVR_multiview2 : enable
                    layout(num_views=2) in;
                    layout(location = 0) in vec4 a_position;
                    layout(location = 1) in vec2 a_texCoord;
                    out vec2 v_texCoord;
                    void main()
                    {
                       gl_Position = a_position;
                       v_texCoord = a_texCoord;
                    })glsl"
            };

    char *fShaderStr[] = {
            R"glsl(#version 300 es
            precision mediump float;
            in vec2 v_texCoord;
            layout(location = 0) out vec4 outColor;
            uniform sampler2D s_texture;
            void main()
            {
              outColor = texture( s_texture, v_texCoord );
            }   )glsl",

            R"glsl(#version 300 es
            precision mediump float;
            in vec2 v_texCoord;
            layout(location = 0) out vec4 outColor;
            uniform sampler2D s_texture;
            void main()
            {
              outColor = texture( s_texture, v_texCoord );
            }   )glsl"
    };

    // Load the shaders and get a linked program object
    userData->programObject = esLoadProgram ( vShaderStr[index], fShaderStr[index] );

//   GLsizei len = 1000;
//    GLchar data[1000];
//   glGetShaderSource(userData->programObject, 1, &len, data );

    // Get the sampler location
    userData->samplerLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

    // Load the texture
    userData->textureId = createTexture ();
    CheckGLError("InitTex");

    InitData();

    return true;
}



void DrawTex( UserData *userData)
{
    CheckGLError("drawtex begin");

    // Use the program object
    glUseProgram ( userData->programObject );

    // Set the viewport
//    glViewport ( 0, 0, esContext->width, esContext->height );

    // Clear the color buffer
//    glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
//    glClear ( GL_COLOR_BUFFER_BIT );

    // Load the vertex position

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, userData->textureId );
    glUniform1i ( userData->samplerLoc, 0 );

    static GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
                                   0.0f,  0.0f,        // TexCoord 0
                                   -0.5f, -0.5f, 0.0f,  // Position 1
                                   0.0f,  1.0f,        // TexCoord 1
                                   0.5f, -0.5f, 0.0f,  // Position 2
                                   1.0f,  1.0f,        // TexCoord 2
                                   0.5f,  0.5f, 0.0f,  // Position 3
                                   1.0f,  0.0f         // TexCoord 3
    };
    static GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), vVertices );
    glVertexAttribPointer ( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), &vVertices[3] );
    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );

//    glBindBuffer(GL_ARRAY_BUFFER, gUserData.vboID);
//    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), 0 );
//    glVertexAttribPointer ( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), (void*)(3 * sizeof(GLfloat)) );
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gUserData.iboID);
//    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    glDisableVertexAttribArray ( 0 );
    glDisableVertexAttribArray ( 1 );

    CheckGLError("drawtex");

//    glDrawArrays ( GL_POINTS, 0, 6 );
}

void ShutDownTex ( UserData *userData )
{
    // Delete texture object
    glDeleteTextures ( 1, &userData->textureId );
    // Delete program object
    glDeleteProgram ( userData->programObject );
}

