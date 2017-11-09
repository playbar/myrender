#include "SphereModel.h"


namespace Baofeng
{
	namespace Mojing
	{
		SphereModel::SphereModel(int modelType, int textureType) : Model(textureType)
		{
			_modelType = modelType;
			SetClassName(__FUNCTION__);
		}

		SphereModel::~SphereModel()
		{
		}

		void SphereModel::Init()
		{
			prepareData();
			Setup();
		}

		void SphereModel::prepareData()
		{
			if (HasEXT_multiview_ovr)
			{
				data.vertexShader =
					"#version 300 es\n"
					"#extension GL_OVR_multiview : enable\n"
					"layout(num_views = " LAYER_COUNT_STRING ") in;\n"

					"uniform mat4 modelViewProjection[" LAYER_COUNT_STRING "];\n"
					"uniform mat4 model;\n"

					"layout(location = 0) in vec3 vertexPosition;\n"
					"layout(location = 1) in vec3 vertexNormal;\n"
					"layout(location = 2) in vec2 vertexUV;\n"

					"out vec2 textureCoord;\n"
					"void main()\n"
					"{\n"
					"   gl_Position = modelViewProjection[gl_ViewID_OVR] * vec4(vertexPosition,1);\n"
					"   textureCoord = vertexUV;\n"
					"}\n";
				data.fragmentShader =
					"#version 300 es\n"
					"precision mediump float;\n"
					"uniform sampler2D sTexture;\n"
					"in vec2 textureCoord;\n"
					"out vec4 finalColor;\n"
					"void main()\n"
					"{\n"
					"   finalColor = texture(sTexture, textureCoord);"
					"}\n";
			}
			else
			{
				data.vertexShader =
					"#version 300 es\n"

					"uniform mat4 modelViewProjection;\n"
					"uniform mat4 model;\n"

					"layout(location = 0) in vec3 vertexPosition;\n"
					"layout(location = 1) in vec3 vertexNormal;\n"
					"layout(location = 2) in vec2 vertexUV;\n"

					"out vec2 textureCoord;\n"
					"void main()\n"
					"{\n"
					"   gl_Position = modelViewProjection * vec4(vertexPosition,1);\n"
					"   textureCoord = vertexUV;\n"
					"}\n";
				if (_textureType == 0)
				{
					data.fragmentShader =
						"#version 300 es\n"
						"precision mediump float;\n"
						"uniform sampler2D sTexture;\n"
						"in vec2 textureCoord;\n"
						"out vec4 finalColor;\n"
						"void main()\n"
						"{\n"
						"   finalColor = texture(sTexture, textureCoord);"
						"}\n";
				}
				else
				{
					data.fragmentShader =
						"#version 300 es\n"
						"#extension GL_OES_EGL_image_external : require\n"
						"precision mediump float;\n"
						"uniform samplerExternalOES sTexture;\n"
						"in vec2 textureCoord;\n"
						"out vec4 finalColor;\n"
						"void main()\n"
						"{\n"
						"   finalColor = texture(sTexture, textureCoord);"
						"}\n";
				}
				
			}

            createSphere(20.0f);
		}

        void SphereModel::createSphere(float radius) {
            int segmentCount = 8;
            int hozSegmentCount = segmentCount * 4;
            int verSegmentCount = segmentCount * 2;

            std::vector<float> cosTheta;
            std::vector<float> sinTheta;

            double theta = NV_PI / 2;
            double thetaStep = NV_PI / (segmentCount * 2);
            for (int i = 0; i < hozSegmentCount; i++, theta += thetaStep) {
                cosTheta.push_back((float)std::cos(theta));
                sinTheta.push_back((float)std::sin(theta));
            }
            cosTheta.push_back(cosTheta[0]);
            sinTheta.push_back(sinTheta[0]);

            // Angle in x-y plane
            double angle = (NV_PI / 2);
            double angleStep = NV_PI / verSegmentCount;

            // Save vertex data
            std::vector<float> vertexPos;
            // Save texture data
            std::vector<float> vertexTexCoord;
			std::vector<float> vertexTexCoord_another;

            for (int i = 0; i <= verSegmentCount; i++, angle -= angleStep) {
				float t;

				// 上下分屏
				if (_modelType == 2)
				{
					t = (float)i / verSegmentCount / 2;
				}
				else
				{
					t = (float)i / verSegmentCount;
				}

                double radiusInCrossSection;
                float y;

                if (i == 0) {
                    radiusInCrossSection = 0;
                    y = (float)radius;
                } else if (i == verSegmentCount) {
                    radiusInCrossSection = 0;
                    y = (float)-radius;
                } else {
                    radiusInCrossSection = radius * std::cos(angle);
                    y = (float)(radius * std::sin(angle));
                }

                for (int j = 0; j <= hozSegmentCount; j++) {
					float s;
					// 左半屏
					if (_modelType == 3)
					{
						s = (float)(hozSegmentCount - j) / hozSegmentCount / 2;
					}
					else
					{
						s = (float)(hozSegmentCount - j) / hozSegmentCount;
					}

                    vertexPos.push_back( (float)(radiusInCrossSection * sinTheta[j]) );
                    vertexPos.push_back(y);
                    vertexPos.push_back( (float)(radiusInCrossSection * cosTheta[j]) );

                    vertexTexCoord.push_back(s);
                    vertexTexCoord.push_back(t);
                }
            }

			/* 下半部纹理坐标 */
			if (_modelType == 2)
			{
				for (int i = 0; i <= verSegmentCount; i++) {
					float t;
					// 下半部
					{
						t = (float)i / verSegmentCount / 2 + 0.5;
					}

					double radiusInCrossSection;
					float y;

					for (int j = 0; j <= hozSegmentCount; j++) {
						float s = (float)(hozSegmentCount - j) / hozSegmentCount;

						vertexTexCoord_another.push_back(s);
						vertexTexCoord_another.push_back(t);
					}
				}
			}
			
			/* 右半部纹理坐标 */
			if (_modelType == 3)
			{
				for (int i = 0; i <= verSegmentCount; i++) {
					float t;
					{
						t = (float)i / verSegmentCount;
					}

					for (int j = 0; j <= hozSegmentCount; j++) {
						float s;
						// 右半屏
						{
							s = (float)(hozSegmentCount - j) / hozSegmentCount / 2 + 0.5f;
						}

						vertexTexCoord_another.push_back(s);
						vertexTexCoord_another.push_back(t);
					}
				}
			}

            //
            // Fill indices buffer
            //
            std::vector<GLushort> vertexIndex;

            for (GLushort row = 0; row < verSegmentCount; row++) {
                for (GLushort col = 0; col < hozSegmentCount; col++) {
                    GLushort N10 = (row + 1) * (hozSegmentCount + 1) + col;
                    GLushort N00 = row * (hozSegmentCount + 1) + col;

					vertexIndex.push_back(N00);
					vertexIndex.push_back(N10 + 1);
					vertexIndex.push_back(N10);

					vertexIndex.push_back(N00);
					vertexIndex.push_back(N00 + 1);
					vertexIndex.push_back(N10 + 1);

                }
            }

            data.vertices = vertexPos;
			data.indices = vertexIndex;
            data.uvs = vertexTexCoord;
			data.uvs_another = vertexTexCoord_another;
        }
	}
}