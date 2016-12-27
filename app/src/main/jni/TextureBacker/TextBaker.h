#pragma  once
#include "Util.h"
#include "FBO.h"
#include "Shader.h"
/*
Func: TextureBacker
Function:  创建一张纹理（TargetTex)， 宽度：width，高度： height, 纹理ID： texID，在StartBacker完成设置
BackerTexture每次将一张sourceTex贴入到TargetTex中，参数指定坐标
StopBacker(): 完成贴图，释放资源
GetTexID()： 获取最终TargetTex。

Create: 2016-03-21   by jaccen
*/
namespace Baofeng {
    namespace Mojing
    {
class TextureBacker
{
public:
	TextureBacker();
	~TextureBacker();

public:
	void  StartBacker(GLuint width, GLuint height, int texID = 0);
	//参数主要是贴入图参数：TexId，贴入图Id， [x, y]贴图纹理起始点，[TexWidth.TexHeight]贴图纹理宽，[desX;desY] 在targat纹理中的位置
	void  BackerTexture(GLuint Texid, GLuint x, GLuint y, GLuint TexWidth, GLuint TexHeight, GLuint desX, GLuint desY);
	void  StopBacker();
	//返回target纹理
	void  GetTexID(int *Texid);

	FrambufferObject* GetFbo() { return m_fbo; }

	bool InitVBO();
	void DrawVBO(GLuint texid, float* Sourece, float* Target);
	void DestryVBO();
protected:
	bool InitFBO(GLuint width, GLuint height, int texID);
	bool UseFBO();
	bool UnUseFBO();
	void DestryFBO();

private:
	FrambufferObject* m_fbo;
	Shader* pShader;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint indexbuffer;
	GLuint m_posLoc;
	GLuint m_TexLoc;

};

    }
}