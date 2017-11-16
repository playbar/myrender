#pragma once
#include "../Base/BuildType.h"
#include "../Base/GlUtils.h"

namespace Baofeng
{
	namespace Mojing
	{
		class CEyeTextureParam
		{
		public:
			CEyeTextureParam();
			virtual ~CEyeTextureParam();
			bool Create(const int &iWidth, const int &iHeight, const GLenum &format = GL_RGBA);
			bool CheckMatch(const GLuint&iWidth, const GLuint &iHeight, const GLenum &format = GL_RGBA)const
			{
				return 0 !=m_TextureID && m_uiHeigth == iHeight && iWidth == m_uiWidth && m_Format == format;
			}
			void Release();
			void UsingTextureID(GLuint TextureID);
			CLASS_INTERFACE(GLuint, m_, TextureID);
			CLASS_INTERFACE(GLuint, m_ui, Width);
			CLASS_INTERFACE(GLuint, m_ui, Heigth);
			CLASS_INTERFACE(GLenum, m_, Format);
			CLASS_INTERFACE(bool, m_b, AutoRelease);
		};


	}
}
