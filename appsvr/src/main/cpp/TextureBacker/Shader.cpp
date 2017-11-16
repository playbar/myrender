#include "Shader.h"
namespace Baofeng {
    namespace Mojing
    {
Shader::Shader() :m_Program(0)
{
}


Shader::~Shader()
{
	glDeleteProgram(m_Program);
	m_Program = 0;
}

GLuint Shader::LoadShader(GLenum shaderType, const char* pSource)
{
	GLuint shader = glCreateShader(shaderType);

	if (shader) {

		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);

		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled) {

			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen) {
				char* buf = (char*)malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    // TODO: iOS log
//					LOGE("Could not compile shader %d:\n%s\n",
//						shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint Shader::CreateProgram(const char* pVertexSource, const char* pFragmentSource)
{
	//加载Vertexshader
	GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		return 0;
	}
	//加载Fragmentshader
	GLuint pixelShader = LoadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		return 0;
	}

	m_Program = glCreateProgram();

	if (m_Program) {

		glAttachShader(m_Program, vertexShader);
		//checkGlError("glAttachShader");
		glAttachShader(m_Program, pixelShader);
		//checkGlError("glAttachShader");
		glLinkProgram(m_Program);
		GLint linkStatus = GL_FALSE;

		glGetProgramiv(m_Program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {

			GLint bufLength = 0;
			glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {

				char* buf = (char*)malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(m_Program, bufLength, NULL, buf);
					char Error[1024] = { 0 };
					sprintf(Error, "Could not link program:\n%s\n", buf);
					MOJING_ERROR(g_APIlogger, Error);
					free(buf);
				}
			}
			glDeleteProgram(m_Program);
			m_Program = 0;
		}
	}
	return m_Program;
}



void Shader::SetUniform1f(const char* name, float value)
{
	glUniform1f(glGetUniformLocation(m_Program, name), value);
}

void Shader::SetUniform1i(const char* name, int value)
{

	glUniform1i(glGetUniformLocation(m_Program, name), value);
}

void Shader::SetUniformMat4(const char* name, const float* matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(m_Program, name), 1, GL_FALSE, matrix);
}

GLuint Shader::GetAttribLoaction(const char* name)
{
	return glGetAttribLocation(m_Program, name);
}
    }
}
