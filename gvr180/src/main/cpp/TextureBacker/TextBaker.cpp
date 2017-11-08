#include "TextBaker.h"

#include "../MojingManager.h"

#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDisplayParameters.h"
#include "../Base/MojingMath.h"

namespace Baofeng {
    namespace Mojing
    {

static const char gVertexShader[] =
"attribute vec3 a_position;\n"
"attribute vec2 a_texCoord;\n"
"uniform mat4 MVP;  \n"
"varying  vec2 v_texCoord;\n"

"void main() {\n"
"  gl_Position = MVP*vec4(a_position,1.0);\n"
"  v_texCoord = a_texCoord;\n"
"}\n";

static const char gFragmentShader[] =
"precision mediump float;\n"
"uniform sampler2D TextureMap;	\n"
"varying  vec2	v_texCoord; \n"

"void main() {\n"
//"gl_FragColor =vec4(0.0, 1.0, 0.0, 1.0);\n"// 
"gl_FragColor = texture2D(TextureMap, v_texCoord); \n"
"}\n";



GLfloat Vertices[] = { -1.0f, 1.0f, 0.0f,  // Position 0
-1.0f, -1.0f, 0.0f,  // Position 1
1.0f, -1.0f, 0.0f,  // Position 2
1.0f, 1.0f, 0.0f,  // Position 3
};
GLfloat uvVertices[] = {
	0.0f, 0.0f,        // TexCoord 0 
	0.0f, 1.0f,        // TexCoord 1
	1.0f, 1.0f,        // TexCoord 2
	1.0f, 0.0f         // TexCoord 3
};


float mvp1[16] =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};
GLushort indices[] = { 0, 1, 2, 0, 2, 3 };


TextureBacker::TextureBacker() :m_fbo(NULL), pShader(NULL)
{
//	MOJING_FUNC_TRACE(g_APIlogger);
	vertexbuffer = 0;
	uvbuffer = 0;
	indexbuffer = 0;
	m_posLoc = 0;
	m_TexLoc = 1;
}

TextureBacker::~TextureBacker()
{
	if (pShader)
	{
		delete pShader;
		pShader = NULL;
	}
	if (m_fbo)
	{
		delete m_fbo;
		m_fbo = NULL;
	}
}

void TextureBacker::StartBacker(GLuint width, GLuint height, int texID)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	if (pShader == NULL)
	{
		pShader = new Shader();
		pShader->CreateProgram(gVertexShader, gFragmentShader);
	}
	InitVBO();
	InitFBO(width, height, texID);

	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	pShader->UseProgram();

	m_posLoc = pShader->GetAttribLoaction("a_position");
	m_TexLoc = pShader->GetAttribLoaction("a_texCoord");

	pShader->UnUseProgram();
}

void TextureBacker::BackerTexture(GLuint Texid, GLuint x, GLuint y, GLuint width, GLuint height, GLuint desX, GLuint desY)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	//MOJING_TRACE(g_APIlogger, "BackerTexture X = " << x << "y= " << y << " w= " << width << "h= " << height << "dex= " << desX);
	float Target[4][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 1}
	};

	float fX = 0.0f, fY = 0.0f, fWidth = 0.0f, fHeight = 0.0f;
	float gw = m_fbo->GetWidth();
	float gh = m_fbo->GetHeight();

	fX = x * 1.0f / width;
	fY = y * 1.0f / height;
	fWidth = width * 1.0f / width;
	fHeight = height  * 1.0f / height;
	// 同理，计算目标图上的显示范围
	float fDesX, fDesY, fDestWidth, fDestHeight;
	fDesX = (desX + width *0.5) * 1.0f / gw*2.0 - 1.0;
	fDesY = ((desY + height*0.5)* 1.0f / gh*2.0 - 1.0) * (-1.0);
	fDestWidth = (float)width / (float)gw;
	fDestHeight = (float)height / (float)gh;

	Target[0][0] = fDestWidth;
	Target[1][1] = fDestHeight;
	Target[3][0] = fDesX ;
	Target[3][1] = fDesY;


	// 缩放矩阵，使得源区域限定于[fX , fY , fX + fWidth , fY + fHeight ]

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_fbo->UseFBO();
	glViewport(0, 0, gw, gh);

	DrawVBO((GLuint)(size_t)Texid,  NULL, Target[0]);
	MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
	glViewport(0, 0, pDisplayParameters->GetScreenWidth(), pDisplayParameters->GetScreenHeight());

	m_fbo->EndFBO();
	glDisable(GL_BLEND);
}

void TextureBacker::StopBacker()
{
	DestryFBO();
	DestryVBO();
}

void TextureBacker::GetTexID(int *Texid)
{
	*Texid = (int)m_fbo->GetTextureId();
}
//初始化VBO
bool TextureBacker::InitVBO()
{
	MOJING_FUNC_TRACE(g_APIlogger);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), &Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);

	glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), &uvVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), &indices, GL_STATIC_DRAW);

	return true;
}


void TextureBacker::DrawVBO(GLuint texid, float* Sourece, float* Target)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	//UnUseFBO();
	pShader->UseProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texid);

	pShader->SetUniform1i("TextureMap", 0);
	pShader->SetUniformMat4("MVP", Target);

	glEnableVertexAttribArray(m_posLoc);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(m_posLoc, // The attribute we want to configure
		3,                           // size
		GL_FLOAT,                    // type
		GL_FALSE,                    // normalized?
		0,
		(void*)0);
	//
	glEnableVertexAttribArray(m_TexLoc);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		m_TexLoc,                   // The attribute we want to configure
		2,                            // size : U+V => 2
		GL_FLOAT,                     // type
		GL_FALSE,                     // normalized?
		0,                            // stride
		(void*)0                      // array buffer offset
		);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,        // mode
		6,                   // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0             // element array buffer offset
		);
	glBindTexture(GL_TEXTURE_2D, 0);
	pShader->UnUseProgram();
	// 3. 请求将图像绘制到窗口  
	glFlush();
}

void TextureBacker::DestryVBO()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &indexbuffer);
}

bool TextureBacker::InitFBO(GLuint width, GLuint height, int texID)
{
	m_fbo = new FrambufferObject();
	m_fbo->CreateFBO(width, height, texID);
    return true;
}

bool TextureBacker::UseFBO()
{
	m_fbo->UseFBO();
    return true;
}

bool TextureBacker::UnUseFBO()
{
	m_fbo->EndFBO();
    return true;
}

void TextureBacker::DestryFBO()
{
	m_fbo->DestroyFBO();
}
    }
}
