#ifndef _RENDER_MODELS_MODEL_H
#define _RENDER_MODELS_MODEL_H

#include <vector>
#include "../../Base/GlUtils.h"
#include "../../Base/MojingMath.h"
#include "../../Base/MojingString.h"
/* 启用高分辨率优化，有问题，默认不使用 */
#if defined(ENABLE_MULTIVIEW_HIGH_RESOLUTION)
#define LAYER_COUNT 4
#else
#define LAYER_COUNT 2
#endif

#define STRINGIZE( x )			#x
#define STRINGIZE_VALUE( x )	STRINGIZE( x )

#define LAYER_COUNT_STRING		STRINGIZE_VALUE( LAYER_COUNT )

#define USE_NATIVE_FBO 0

namespace Baofeng
{
	namespace Mojing
	{
		enum MODEL_TYPE
		{
			MODEL_SKYBOX		= 0,
			MODEL_SPHERE_2D		= 1,
			MODEL_SPHERE_3D_UD	= 2,
			MODEL_SPHERE_3D_LR	= 3,
			MODEL_RECT_2D		= 4
		};
		enum TEXTURE_TYPE
		{
			TextureType_DEFAULT = 0 , 
			TextureType_MOVIE = 1
		};
		struct ModelData
		{
			char const * vertexShader;
			char const * fragmentShader;
			GLuint textureID0;
			std::vector<GLfloat> vertices;
			std::vector<GLushort> indices;
			std::vector<GLfloat> normals;
			std::vector<GLfloat> uvs;
			std::vector<GLfloat> uvs_another; // 另外半部纹理坐标，用于 上下/ 左右 分屏
		};

		class Model
		{
		public:
			Model(int textureType);
			virtual ~Model();

			virtual void Init();
			static Model * CreateModel(MODEL_TYPE ModelType, TEXTURE_TYPE TextureType, const char* atlasJson);
			GLuint* getAndroidGuiTexture();

			void RenderModel(int eye, int textureId, float* pMVP);
			
		protected:
			virtual void prepareData();
			bool Setup();
#if 0
			bool setupFBO(GLuint* pTextureId, GLuint* pFboId);
#endif
			GLuint loadShader(GLenum shaderType, const char* shaderSource);
			GLuint createProgram(const char* vertexSource, const char * fragmentSource);

			CLASS_MEMBER_STR(String , m_s , ClassName);
		protected:
			/* 0: 普通纹理, 1: movie texture */
			int _textureType;
			/* Model data */
			ModelData data;

			/* Shader */
			GLuint program;

			/* FBO */
			const GLuint fboWidth = 1024;
			const GLuint fboHeight = 1024;
			GLuint frameBufferTextureId[2]; // multiview: single eye
			GLuint frameBufferObjectId;
			GLuint frameBufferDepthTextureId;

			GLuint frameBufferTextureId_AndroidGui[2];
			GLuint frameBufferObjectId_AndroidGui;

			GLint vertexLocation = 0;
			GLint vertexNormalLocation = 1;
			GLint vertexUVLocation = 2;
			GLint m_ModelViewProjectionLocation;
			GLint m_ModelLocation;
			GLint samplerLocation;

			/* Matrix */
			Baofeng::Mojing::Matrix4f projectionMatrix[LAYER_COUNT];
			Matrix4f viewMatrix[LAYER_COUNT];
			Matrix4f viewProjectionMatrix[LAYER_COUNT];
			Matrix4f modelViewProjectionMatrix[LAYER_COUNT];
			Matrix4f modelMatrix;

		};
	}
}

#endif