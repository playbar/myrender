#include "SkyboxModel.h"
#include "../../MojingAPI.h"
namespace Baofeng
{
    namespace Mojing
    {
		SkyboxModel::SkyboxModel(int textureType) : Model(textureType)
		{
			SetClassName(__FUNCTION__);
		}
		void SkyboxModel::Init()
		{
			prepareData();
			Setup();
		}
		SkyboxModel::~SkyboxModel()
		{
		}
        void SkyboxModel::prepareData()
        {
			if (HasEXT_multiview_ovr)
			{
				data.vertexShader =
					"#version 300 es\n"
					"#extension GL_OVR_multiview : enable\n"
					"layout(num_views = " LAYER_COUNT_STRING ") in;\n"

					"uniform mat4 modelViewProjection[" LAYER_COUNT_STRING "];\n"
					"uniform mat4 model;\n"

					"in vec3 vertexPosition;\n"
					"in vec3 vertexNormal;\n"
					"in vec2 vertexUV;\n"
					"out vec2 textureCoord;\n"

					"void main()\n"
					"{\n"
					"   gl_Position = modelViewProjection[gl_ViewID_OVR] * vec4(vertexPosition, 1.0);\n"
					"   textureCoord = vertexUV;\n"
					"}\n";
				data.fragmentShader =
					"#version 300 es\n"
					"precision mediump float;\n"
					"uniform sampler2D sTexture;\n"
					"uniform int layerIndex;\n"
					"in vec2 textureCoord;\n"
					"out vec4 finalColor;\n"
					"void main()\n"
					"{\n"
					"   finalColor = texture(sTexture, textureCoord);\n"
					"}\n";
			}
			else
			{
				data.vertexShader =
					"#version 300 es\n"

					"uniform mat4 modelViewProjection;\n"
					"uniform mat4 model;\n"

					"in vec3 vertexPosition;\n"
					"in vec3 vertexNormal;\n"
					"in vec2 vertexUV;\n"
					"out vec2 textureCoord;\n"

					"void main()\n"
					"{\n"
					"   gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);\n"
					"   textureCoord = vertexUV;\n"
					"}\n";

				if (_textureType == 0)
				{
					data.fragmentShader =
						"#version 300 es\n"
						"precision mediump float;\n"
						"uniform sampler2D sTexture;\n"
						"uniform int layerIndex;\n"
						"in vec2 textureCoord;\n"
						"out vec4 finalColor;\n"
						"void main()\n"
						"{\n"
						"   finalColor = texture(sTexture, textureCoord);\n"
						"}\n";
				}
				else // movie texture
				{
					data.fragmentShader =
						"#version 300 es\n"
						"#extension GL_OES_EGL_image_external : require\n"
						"precision mediump float;\n"
						"uniform samplerExternalOES sTexture;" //
						"uniform int layerIndex;\n"
						"in vec2 textureCoord;\n"
						"out vec4 finalColor;\n"
						"void main()\n"
						"{\n"
						"   finalColor = texture(sTexture, textureCoord);\n"
						"}\n";
				}
				
				
			}

			data.vertices = {
                /*Right*/
                +1.0f , +1.0f,  -1.0f ,// 右前上0
                +1.0f , +1.0f,  +1.0f ,// 右后上1
                +1.0f , -1.0f,  +1.0f ,// 右后下2
                +1.0f , -1.0f,  -1.0f ,// 右前下3
                /*左面*/
                -1.0f , +1.0f , +1.0f ,// 左后上 4
                -1.0f , +1.0f , -1.0f ,// 左前上5
                -1.0f , -1.0f , -1.0f ,// 左前下6
                -1.0f , -1.0f , +1.0f ,// 左后下7

                /*上面*/
                -1.0f, +1.0f, +1.0f,//左后上8
                +1.0f, +1.0f, +1.0f,//右后上9
                +1.0f, +1.0f, -1.0f,//右前上10
                -1.0f, +1.0f, -1.0f,//左前上 11
                /*下面*/
                -1.0f, -1.0f, -1.0f,//左前下12
                +1.0f, -1.0f, -1.0f,//右前下13
                +1.0f, -1.0f, +1.0f,//右后下14
                -1.0f, -1.0f, +1.0f,//左后下15
                /*前面*/
                -1.0f , 1.0f , -1.0f, // 左前上 - 16
                +1.0f , 1.0f , -1.0f, // 右前上 - 17
                +1.0f ,-1.0f , -1.0f, // 右前下 - 18
                -1.0f ,-1.0f , -1.0f, // 左前下 - 19
                /*后面*/
                +1.0f , 1.0f , +1.0f, // 右后上 - 20
                -1.0f , 1.0f , +1.0f, // 左后上 - 21
                -1.0f ,-1.0f , +1.0f,  // 左后下 - 22
                +1.0f ,-1.0f , +1.0f // 右后下 - 23
            };

            float fBoxHalfSize = 20.0f;
			for (auto& v : data.vertices)
			{
				v = v * fBoxHalfSize;
			}

            data.indices = {
                // 右面
                0,2,3 ,
                0,1,2 ,
                // 左面
                4,6,7,
                4,5,6,

                // 上面
                8,10,11,
                8,9,10,

                // 下面
                12,14,15,
                12,13,14,
                // 前面
                16,18,19,
                16,17,18,
                // 后面
                20,22,23,
                20,21,22
            };

            data.uvs = {
                /*右面*/
                0 , 0 ,
                1 , 0 ,
                1 , 1 ,
                0 , 1 ,
                /*左面*/
                1 , 0 ,
                2 , 0 ,
                2 , 1 ,
                1 , 1 ,
                /*上面 */
                2 , 0 ,
                3 , 0 ,
                3 , 1 ,
                2 , 1 ,
                /*下面 */
                0 , 1 ,
                1 , 1 ,
                1 , 2 ,
                0 , 2 ,
                /*前面*/
                1 , 1 ,
                2 , 1 ,
                2 , 2 ,
                1 , 2 ,
                /*后面*/
                2 , 1 ,
                3 , 1 ,
                3 , 2 ,
                2 , 2
            };

            float SubImageSizeWidth = 1.0f/3.0f;
            float SubImageSizeHeight = 1.0f/2.0f;
			float fSafeEdge = 1.0f / 202.0f;
            for (int Said = 0 ; Said < 6 ; Said ++)
            {
                int iIndexBase = Said * 4 * 2;
                // 左上
				data.uvs[iIndexBase + 0] = data.uvs[iIndexBase + 0] * SubImageSizeWidth + fSafeEdge;
				data.uvs[iIndexBase + 1] = data.uvs[iIndexBase + 1] * SubImageSizeHeight + fSafeEdge;
                // 右上
				data.uvs[iIndexBase + 2] = data.uvs[iIndexBase + 2] * SubImageSizeWidth - fSafeEdge;
				data.uvs[iIndexBase + 3] = data.uvs[iIndexBase + 3] * SubImageSizeHeight + fSafeEdge;
                // 右下
				data.uvs[iIndexBase + 4] = data.uvs[iIndexBase + 4] * SubImageSizeWidth - fSafeEdge;
				data.uvs[iIndexBase + 5] = data.uvs[iIndexBase + 5] * SubImageSizeHeight - fSafeEdge;
                // 左下
				data.uvs[iIndexBase + 6] = data.uvs[iIndexBase + 6] * SubImageSizeWidth + fSafeEdge;
				data.uvs[iIndexBase + 7] = data.uvs[iIndexBase + 7] * SubImageSizeHeight - fSafeEdge;
            }
        }
    }
}