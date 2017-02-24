#pragma once
#include "Util.h"
namespace Baofeng {
    namespace Mojing
    {
        /*
         *ｓｈａｄｅｒ　加载
         * Create By jaccen
         */
        class Shader
        {
        public:
            Shader();
            virtual ~Shader();
            
            GLuint CreateProgram(const char* pVertexSource, const char* pFragmentSource);
            
            void   UseProgram(){ glUseProgram(m_Program); }
            void   UnUseProgram(){ glUseProgram(0); }
            
            void   SetUniform1f(const char* name, float value);
            void   SetUniform1i(const char* name, int value);
            void   SetUniformMat4(const char* name, const float* matrix);
            
            GLuint  GetAttribLoaction(const char* name);
            GLuint  GetPrograme(){ return m_Program; }
            
        protected:
            GLuint LoadShader(GLenum shaderType, const char* pSource);
        private:
            GLuint m_Program;
        };
    }
}


