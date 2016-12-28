#include "EyeTextureParam.h"

namespace Baofeng
{
	namespace Mojing
	{
		CEyeTextureParam::CEyeTextureParam() :
			m_TextureID(0),
			m_uiWidth(0),
			m_uiHeigth(0),
			m_Format(0),
			m_bAutoRelease(false)
		{
		}


		CEyeTextureParam::~CEyeTextureParam()
		{
			Release();
		}

		void CEyeTextureParam::Release()
		{
			if (m_bAutoRelease)
			{// 需要自动释放资源
				if (m_TextureID != 0 && glIsTexture(m_TextureID))
				{
					glDeleteTextures(1, &m_TextureID);
				}
			}
			m_TextureID = 0;
			m_uiWidth = 0;
			m_uiHeigth = 0;
			m_Format = 0;
			m_bAutoRelease = false;
		}

		void CEyeTextureParam::UsingTextureID(GLuint TextureID)
		{
			if (m_TextureID != TextureID)
			{
				Release();
				m_TextureID = TextureID;
			}
		}

		bool CEyeTextureParam::Create(const int &iWidth, const int &iHeight, const GLenum &format)
		{// 创建给模型用的纹理
			GLuint iId;
			glGenTextures(1, &iId);
			glBindTexture(GL_TEXTURE_2D, iId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, format, iWidth, iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);

			m_TextureID = iId;
			m_uiWidth = iWidth;
			m_uiHeigth = iHeight;
			m_Format = format;
			m_bAutoRelease = true;
			return true;
		}
	}
}
