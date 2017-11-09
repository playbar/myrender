#pragma once
#include "Util.h"

#include <stdio.h>
#include <stdlib.h>
namespace Baofeng
{
    namespace Mojing
    {
        class FrambufferObject
        {
        public:
            FrambufferObject();
            virtual ~FrambufferObject();
        public:
            bool CreateFBO(GLuint width, GLuint height, int texID = 0);
            void UseFBO();
            void EndFBO();
            void DestroyFBO();
            
            bool   SaveToBmp(const char* filename, GLuint x, GLuint y, GLuint TextureId, GLuint width, GLint height, GLuint dib /*= 3*/);
            GLint  GetTextureId() { return m_textureid; }
            GLuint GetWidth() { return m_Width; }
            GLuint GetHeight() { return m_Height; }
            
            const char* GetBufferTexData()const { return m_BufferData; }
            // 将TexId数据写到FBo绑定的Tex中
            void  TransferFromTexture(GLuint m_texId, GLuint x, GLuint y, GLuint m_width, GLuint m_height, GLuint desX, GLuint desY);
        protected:
            char* GetTexData(GLuint m_texId, GLuint x, GLuint y, GLuint width, GLuint height);
        private:
            char*  m_BufferData;
            GLuint m_fbo;
            GLuint m_textureid;
            GLuint m_depthrbo;
            GLuint m_Width;
            GLuint m_Height;
        };
    }
}

