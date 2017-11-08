#include "FBO.h"
namespace Baofeng
{
    namespace Mojing
    {
        static char head[54] = {
            0x42, //0
            0x4d, //1
            0x66, //2
            0x75, //3
            0x00, //4
            0x00, //5
            0x00, //6
            0x00, //7
            0x00, //8
            0x00, //9
            0x36, //a
            0x00, //b
            0x00, //c
            0x00, //d
            0x28, //e
            0x00,//f
            0x00, //0
            0x00, //1
            0x64, //2
            0x00, //3
            0x00, //4
            0x00, //5
            0x64, //6
            0x00, //7
            0x00, //8
            0x00, //9
            0x01, //a
            0x00, //b
            0x18, //c
            0x00, //d
            0x00, //e
            0x00,//f
            0x00, //0
            0x00, //1
            0x30, //2
            0x75//3
        };
FrambufferObject::FrambufferObject() :m_textureid(0), m_fbo(0), m_BufferData(NULL)
{

}

FrambufferObject::~FrambufferObject()
{
	if (m_BufferData)
	{
		delete[] m_BufferData;
		m_BufferData = NULL;
	}
	DestroyFBO();
}



bool FrambufferObject::CreateFBO(GLuint width, GLuint height, int texID)
{
	GLint maxRenderbufferSize = 0;

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

	if ((maxRenderbufferSize <= (int)width) || (maxRenderbufferSize <= (int)height))
	{
		return false;
	}
	m_textureid = texID;
	// 创建FBO
	glGenFramebuffers(1, &m_fbo);
	checkGlErrorForSDK("glGenFramebuffers");

	if (m_textureid == 0)
	{
		//创建Texture
		glGenTextures(1, &m_textureid);
		checkGlErrorForSDK("glGenTextures");
	}
	
	glBindTexture(GL_TEXTURE_2D, m_textureid);
	checkGlErrorForSDK("glBindTexture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	checkGlErrorForSDK("glTexImage2D");
	// 绑定rbo创建一个16-bit depthbuffer
//	glGenRenderbuffers(1, &m_depthrbo);
//	glBindRenderbuffer(GL_RENDERBUFFER, m_depthrbo);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	//Texture 作为color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureid, 0);
	checkGlErrorForSDK("glFramebufferTexture2D");

//  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrbo);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		char Error[1024] = { 0 };
		sprintf(Error, "glCheckFramebufferStatus return:0x%04x", status);
		return false;
	}
	
	
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_Width = width;
	m_Height = height;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void FrambufferObject::UseFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	checkGlErrorForSDK("glBindFramebuffer");
}

void FrambufferObject::EndFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrambufferObject::DestroyFBO()
{
	if (m_fbo != 0)
	{
    	glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}
}

bool FrambufferObject::SaveToBmp(const char* filename, GLuint x, GLuint y, GLuint TextureId, GLuint width, GLint height, GLuint dib /*= 3*/)
{
	GLuint offscreen_framebuffer = 0;
	GLint pixelLength = 0;
	GLubyte * pixelDate = NULL;
	FILE * wfile = NULL;
	GLuint W = width - x;
	GLuint H = height - y;
	//打开文件  
	wfile = fopen(filename, "wb");
	fwrite(head, 54, 1, wfile);
	//更改grab.bmp的头文件中的高度和宽度  
	fseek(wfile, 0x0012, SEEK_SET);
	fwrite(&W, sizeof(width), 1, wfile);
	fwrite(&H, sizeof(height), 1, wfile);
	//为像素分配内存  
	pixelLength = (width - x) * dib;
	if (pixelLength % 4 != 0)
	{
		pixelLength += 4 - pixelLength % 4;
	}
	pixelLength *= (height - y);
	pixelDate = (GLubyte *)malloc(pixelLength);
	if (pixelDate == 0)
	{
		//printf("/a/n分配内存失败!");
	}
#ifdef WIN_32  //opengl 使用glGetTexture
	glBindTexture(GL_TEXTURE_2D, TextureId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelDate);
#else

	glClear(GL_COLOR_BUFFER_BIT);
	//Generate a new FBO. It will contain your texture.
	glGenFramebuffers(1, &offscreen_framebuffer);
	GLint error = glGetError();
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen_framebuffer);
	error = glGetError();
	//Bind the texture to your FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureId, 0);
	error = glGetError();
	//Test if everything failed    
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		return false;
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Bind the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen_framebuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureId);
	// set the viewport as the FBO won't be the same dimension as the screen
	//	glViewport(0, 0, width, height);
	// 读取像素
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (dib == 3)
	{
		glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixelDate);

	}
	else if (dib == 4)
	{
		glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelDate);
	}
	else{
		return false;
	}

	//Bind your main FBO again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	glViewport(0, 0, screen_width, screen_height);

	glDeleteFramebuffers(1, &offscreen_framebuffer);

#endif // DEBUG
	//写入像素数据  
	fseek(wfile, 0, SEEK_END);
	fwrite(pixelDate, pixelLength, 1, wfile);
	fclose(wfile);
	free(pixelDate);
    
    return true;
}

char* FrambufferObject::GetTexData(GLuint m_texId, GLuint x, GLuint y, GLuint width, GLuint height)
{
	GLuint mfbo = 0;
	GLuint sizeLenth = (width ) * (height ) * 3 *sizeof( char);
	if (x > width)
	{
		x = 0;
	}
	if (y > height)
	{
		y = 0;
	}
	if (m_BufferData ==NULL)
	{
	  m_BufferData = new char[sizeLenth];
	  memset(m_BufferData, 255, sizeLenth);
	}
	//
	//生成FBO
	//glClear(GL_COLOR_BUFFER_BIT);
	//Generate a new FBO. It will contain your texture.
	glGenFramebuffers(1, &mfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mfbo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texId);
	//Bind the texture to your FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texId, 0);
	//Test if everything failed    
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		return NULL;
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Bind the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, mfbo);

	// set the viewport as the FBO won't be the same dimension as the screen
	//glViewport(0, 0, width, height);
	// 读取像素

	//获取Texture数据 
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, m_BufferData);

	//
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &mfbo);
	return  m_BufferData;
}

//将m_texId写到绑定到FBO的m_textureid中，[x, y, width,height]为区域位置 [desX, desY]为在大图中的偏移
void FrambufferObject::TransferFromTexture(GLuint m_texId, GLuint x, GLuint y, GLuint width, GLuint height, GLuint desX, GLuint desY)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	char *data = GetTexData(m_texId, x, y, width, height);
	glBindTexture(GL_TEXTURE_2D, m_textureid);
	glTexSubImage2D(GL_TEXTURE_2D, 0, desX, desY, (width), (height), GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}
    }
}

