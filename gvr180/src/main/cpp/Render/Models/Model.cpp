#include "Model.h"
#include "SphereModel.h"
#include "SkyboxModel.h"
#include "RectangleModel.h"

namespace Baofeng
{
	namespace Mojing
	{
		Model::Model(int textureType)
		{
			_textureType = textureType;
			data.vertexShader = 0;
			data.fragmentShader = 0;
			data.textureID0 = 0;
			SetClassName(__FUNCTION__);
		}
		void Model::Init()
		{
			prepareData();
			Setup();
		}
		Model::~Model()
		{
		}

		GLuint* Model::getAndroidGuiTexture()
		{
			return frameBufferTextureId_AndroidGui;
		}
		void Model::prepareData()
		{
			char A[10];
			const char* pA = A;
			data.vertexShader = "#version 300 es\n"
								"#extension GL_OVR_multiview : enable\n"
								"layout(num_views =" LAYER_COUNT_STRING ") in;\n"

								"uniform mat4 modelViewProjection[" LAYER_COUNT_STRING "];\n"
								"uniform mat4 model;\n"

								"in vec3 vertexPosition;\n"
								"in vec3 vertexNormal;\n"
								
								"out vec3 v_normal;\n"

								"void main()\n"
								"{\n"
								"    gl_Position = modelViewProjection[gl_ViewID_OVR] * vec4(vertexPosition, 1.0);\n"
								"    v_normal = (model * vec4(vertexNormal, 0.0f)).xyz;\n"
								"}\n";
			
			data.fragmentShader = "#version 300 es\n"
								"precision highp float;\n"

								"in vec3 v_normal;\n"
								"out vec4 f_color;\n"

								"vec3 light(vec3 n, vec3 l, vec3 c)\n"
								"{\n"
								"    float ndotl = max(dot(n, l), 0.0);\n"
								"    return ndotl * c;\n"
								"}\n"

								"void main()\n"
								"{\n"
								"    vec3 albedo = vec3(0.95, 0.84, 0.62);\n"
								"    vec3 n = normalize(v_normal);\n"
								"    f_color.rgb = vec3(0.0);\n"
								"    f_color.rgb += light(n, normalize(vec3(1.0)), vec3(1.0));\n"
								"    f_color.rgb += light(n, normalize(vec3(-1.0, -1.0, 0.0)), vec3(0.2, 0.23, 0.35));\n"

								"    f_color.a = 1.0;\n"
								"}\n";
			
			data.vertices = {
				//Front face
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,

				// Right face
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, -1.0f,

				// Back face
				1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,

				// Left face
				-1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,

				// Top face
				-1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,

				// Bottom face
				1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f
			};

			data.indices = {
				//Front face
				0, 1, 2,
				0, 2, 3,

				// Right face
				4, 5, 6,
				4, 6, 7,

				// Back face
				8, 9, 10,
				8, 10, 11,

				// Left face
				12, 13, 14,
				12, 14, 15,

				// Top face
				16, 17, 18,
				16, 18, 19,

				// Bottom face
				20, 21, 22,
				20, 22, 23
			};

			data.normals = {
				//Front face
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,

				// Right face
				1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f,

				// Back face
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,

				// Left face
				-1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f,

				// Top face
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,

				// Bottom face
				0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f
			};
		}
#if 0
		bool Model::setupFBO(GLuint* pTextureId, GLuint* pFboId)
		{
			if (HasEXT_multiview_ovr)
			{
				// Create array texture
				glGenTextures(1, pTextureId);
				glBindTexture(GL_TEXTURE_2D_ARRAY, *pTextureId);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// TODO: the size ?
				glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, screenWidth, screenHeight, LAYER_COUNT);
				
				/* Initialize FBO. */
				glGenFramebuffers(1, pFboId);

				/* Bind our framebuffer for rendering. */
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *pFboId);

				/* Attach texture to the framebuffer. */
				glFramebufferTextureMultiviewOVR_(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					*pTextureId, 0, 0, LAYER_COUNT);

				/* Create array depth texture */
				//glGenTextures(1, &frameBufferDepthTextureId);
				//glBindTexture(GL_TEXTURE_2D_ARRAY, frameBufferDepthTextureId);
				//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, LAYER_COUNT);

				/* Attach depth texture to the framebuffer. */
				//glFramebufferTextureMultiviewOVR_(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				//	frameBufferDepthTextureId, 0, 0, LAYER_COUNT);

				/* Check FBO is OK. */
				GLenum result = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
				if (result != GL_FRAMEBUFFER_COMPLETE)
				{
					//LOGE("multiview Framebuffer incomplete at %s:%i\n", __FILE__, __LINE__);
					/* Unbind framebuffer. */
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					return false;
				}
			}
			else
			{
				// left
				glGenTextures(1, pTextureId);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, pTextureId[0]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				
				GL_CheckErrors("glTexStorage2D");

				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, screenWidth, screenHeight);
				GL_CheckErrors("glTexStorage2D");

				glGenFramebuffers(1, pFboId);
				glBindFramebuffer(GL_FRAMEBUFFER, *pFboId);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTextureId[0], 0);

				GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (result != GL_FRAMEBUFFER_COMPLETE)
				{
					//LOGE("Framebuffer incomplete at %s:%i\n", __FILE__, __LINE__);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					return false;
				}
				// right
				glGenTextures(1, pTextureId + 1);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, *(pTextureId + 1));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, screenWidth, screenHeight);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTextureId[1], 0);

				result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

				// TODO:
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				if (result != GL_FRAMEBUFFER_COMPLETE)
				{
					//LOGE("Framebuffer incomplete at %s:%i\n", __FILE__, __LINE__);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					return false;
				}
			}
			return true;
		}
#endif
		bool Model::Setup()
		{
			/* Creating program for drawing object with multiview. */
			program = createProgram(data.vertexShader, data.fragmentShader);
			if (program == 0)
			{
				//LOGE("Could not create multiview program");
				return false;
			}

			/* Get attribute and uniform locations for multiview program. */
			vertexLocation = glGetAttribLocation(program, "vertexPosition");
			GL_CheckErrors("get uv loc");
			vertexUVLocation = glGetAttribLocation(program, "vertexUV");
			GL_CheckErrors("after uv loc");
			vertexNormalLocation = glGetAttribLocation(program, "vertexNormal");
			m_ModelViewProjectionLocation = glGetUniformLocation(program, "modelViewProjection");
			m_ModelLocation = glGetUniformLocation(program, "model");
			samplerLocation = glGetUniformLocation(program, "sTexture");
#if 0
			/*
			* Set up the perspective matrices for each view. Rendering is done twice in each eye position with different
			* field of view. The narrower field of view should give half the size for the near plane in order to
			* render the center of the scene at a higher resolution. The resulting high resolution and low resolution
			* images will later be interpolated to create an image with higher resolution in the center of the screen
			* than on the outer parts of the screen.
			* 1.5707963268 rad = 90 degrees.
			* 0.9272952188 rad = 53.1301024 degrees. This angle gives half the size for the near plane.
			*/
			projectionMatrix[0] = Matrix4f::Perspective(perspective[0], perspective[1], perspective[2], perspective[3]);
			projectionMatrix[1] = Matrix4f::Perspective(perspective[4], perspective[5], perspective[6], perspective[7]);


			//projectionMatrix[0] = Matrix::matrixPerspective(1.5707963268f, 1.0f, 0.1f, 100.0f);
			//projectionMatrix[1] = Matrix::matrixPerspective(1.5707963268f, 1.0f, 0.1f, 100.0f);
#if defined(ENABLE_MULTIVIEW_HIGH_RESOLUTION)
			projectionMatrix[2] = Matrix::matrixPerspective(0.9272952188f, 1.0f, 0.1f, 100.0f);
			projectionMatrix[3] = Matrix::matrixPerspective(0.9272952188f, 1.0f, 0.1f, 100.0f);
#endif

			/* Setting up model view matrices for each of the */
			Vector3f leftCameraPos = { modelView[0], modelView[1], modelView[2] };
			Vector3f rightCameraPos = { modelView[3], modelView[4], modelView[5] };
			Vector3f lookAt = { modelView[6], modelView[7], modelView[8] };
			Vector3f upVec = { modelView[9], modelView[10], modelView[11] };

			viewMatrix[0] = Matrix4f::LookAt(leftCameraPos, lookAt, upVec);
			char buf_vm[256];
			viewMatrix[0].ToString(buf_vm, 256);
			//LOGI("++v:\n %s", buf_vm);
			viewMatrix[1] = Matrix4f::LookAt(rightCameraPos, lookAt, upVec);
#if defined(ENABLE_MULTIVIEW_HIGH_RESOLUTION)
			viewMatrix[2] = Matrix4f::LookAt(leftCameraPos, lookAt, upVec);
			viewMatrix[3] = Matrix4f::LookAt(rightCameraPos, lookAt, upVec);
#endif
			// TODO: only for render Model.
			Matrix4f::Multiply(&viewProjectionMatrix[0], viewMatrix[0], projectionMatrix[0]);
			Matrix4f::Multiply(&viewProjectionMatrix[1], viewMatrix[1], projectionMatrix[1]);
#if defined(ENABLE_MULTIVIEW_HIGH_RESOLUTION)
			Matrix4f::Multiply(&viewProjectionMatrix[2], viewMatrix[2], projectionMatrix[2]);
			Matrix4f::Multiply(&viewProjectionMatrix[3], viewMatrix[3], projectionMatrix[3]);
#endif
#endif // #if 0

			return true;
		}

		GLuint Model::loadShader(GLenum shaderType, const char* shaderSource)
		{
				GLuint shader = glCreateShader(shaderType);
				if (shader != 0)
				{
					glShaderSource(shader, 1, &shaderSource, NULL);
					glCompileShader(shader);
					GLint compiled = 0;
					glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
					if (compiled != GL_TRUE)
					{
						GLint infoLen = 0;
						glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

						if (infoLen > 0)
						{
							char * logBuffer = (char*)malloc(infoLen);

							if (logBuffer != NULL)
							{
								glGetShaderInfoLog(shader, infoLen, NULL, logBuffer);
						//		LOGE("Could not Compile Shader %d:\n%s\n", shaderType, logBuffer);
								free(logBuffer);
								logBuffer = NULL;
							}

							glDeleteShader(shader);
							shader = 0;
						}
					}
				}

				return shader;
		}

		GLuint Model::createProgram(const char* vertexSource, const char* fragmentSource)
		{
			GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
			if (vertexShader == 0)
			{
				return 0;
			}

			GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
			if (fragmentShader == 0)
			{
				return 0;
			}

			GLuint program = glCreateProgram();

			if (program != 0)
			{
				glAttachShader(program, vertexShader);
				glAttachShader(program, fragmentShader);
				GL_CheckErrors("bind attribs");
				glBindAttribLocation(program, 0, "vertexPosition");
				glBindAttribLocation(program, 1, "vertexNormal");
				glBindAttribLocation(program, 2, "vertexUV");
				GL_CheckErrors("bind attribs");
				glLinkProgram(program);
				GLint linkStatus = GL_FALSE;
				glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
				if (linkStatus != GL_TRUE)
				{
					GLint bufLength = 0;
					glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
					if (bufLength > 0)
					{
						char* logBuffer = (char*)malloc(bufLength);

						if (logBuffer != NULL)
						{
							glGetProgramInfoLog(program, bufLength, NULL, logBuffer);
					//		LOGE("Could not link program:\n%s\n", logBuffer);
							free(logBuffer);
							logBuffer = NULL;
						}
					}
					glDeleteProgram(program);
					program = 0;
				}
			}
			return program;
		}

		void renderToFboAndroidGuiPredraw()
		{
		
		}

		void Model::RenderModel(int eye, int textureId, float* pMVP)
		{
			/* Setup MVP */
			Matrix4f m4MVP ;
			memcpy(&m4MVP.M[0][0], pMVP, sizeof(float)* 16);
#if 0
			Matrix4f sensorView(sensorData[0], sensorData[1], sensorData[2], sensorData[3],
				sensorData[4], sensorData[5], sensorData[6], sensorData[7], 
				sensorData[8], sensorData[9], sensorData[10], sensorData[11],
				sensorData[12], sensorData[13], sensorData[14], sensorData[15]
				);
			modelMatrix = Matrix4f::Identity();
			
			Matrix4f::Multiply(&viewProjectionMatrix[0], sensorView, projectionMatrix[0]);
			Matrix4f::Multiply(&viewProjectionMatrix[1], sensorView, projectionMatrix[1]);
#if defined(ENABLE_MULTIVIEW_HIGH_RESOLUTION)
			Matrix4f::Multiply(&viewProjectionMatrix[2], sensorView, projectionMatrix[2]);
			Matrix4f::Multiply(&viewProjectionMatrix[3], sensorView, projectionMatrix[3]);
#endif

			Matrix4f::Multiply(&modelViewProjectionMatrix[0], modelMatrix, viewProjectionMatrix[0]);
			Matrix4f::Multiply(&modelViewProjectionMatrix[1], modelMatrix, viewProjectionMatrix[1]);
#if defined(ENABLE_MULTIVIEW_HIGH_RESOLUTION)
			Matrix4f::Multiply(&modelViewProjectionMatrix[2], modelMatrix, viewProjectionMatrix[2]);
			Matrix4f::Multiply(&modelViewProjectionMatrix[3], modelMatrix, viewProjectionMatrix[3]);
#endif
#endif
			GL_CheckErrors("begin use program");
			glUseProgram(program);
			GL_CheckErrors("end use program");

			/* Vertex position */
			glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, &data.vertices[0]);
			glEnableVertexAttribArray(vertexLocation);

			/* Vertex normal */
			if ((data.normals.size() > 0) && (vertexNormalLocation != -1))
			{
				glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, &data.normals[0]);
				glEnableVertexAttribArray(vertexNormalLocation);
			}
			
			/* UV */
			if ((data.uvs.size() > 0) && (vertexUVLocation != -1))
			{
				glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, 0, &data.uvs[0]);
				glEnableVertexAttribArray(vertexUVLocation);
			}
			if (eye == 1)
			{
				if ((data.uvs_another.size() > 0) && (vertexUVLocation != -1))
				{
					glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, 0, &data.uvs_another[0]);
					glEnableVertexAttribArray(vertexUVLocation);
				}
			}

			/* Upload model view projection matrices. */
			if (m_ModelLocation)
			{
				glUniformMatrix4fv(m_ModelLocation, 1, GL_FALSE, &modelMatrix.M[0][0]);
			}

			/* Bind model texture */
			{
				GL_CheckErrors("bind tex0");
				glActiveTexture(GL_TEXTURE0);
				if (_textureType == 0)
				{
					glBindTexture(GL_TEXTURE_2D, textureId);
				}
				else
				{
					glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId);
				}
				GL_CheckErrors("uniform tex0");
				glUniform1i(samplerLocation, 0);
				GL_CheckErrors("end tex0");
			}

			if (HasEXT_multiview_ovr)
			{
				glUniformMatrix4fv(m_ModelViewProjectionLocation, LAYER_COUNT, GL_FALSE, &modelViewProjectionMatrix[0].M[0][0]);
				
			}
			else
			{
				// TODO: left/right eye use different matrix
				glUniformMatrix4fv(m_ModelViewProjectionLocation, 1, GL_FALSE, &m4MVP.M[0][0]);
			}

			if (HasEXT_multiview_ovr)
			{
#ifdef GL_DRAW_FRAMEBUFFER
				// TODO: move to java
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObjectId);
#endif // GL_DRAW_FRAMEBUFFER
				
			}
			else
			{

			}

			GL_CheckErrors("draw");
			glDrawElements(GL_TRIANGLES, data.indices.size(), GL_UNSIGNED_SHORT, &data.indices[0]);
			GL_CheckErrors("draw");

			glDisableVertexAttribArray(vertexLocation);
			if ((data.normals.size() > 0) && (vertexNormalLocation != -1))
			{
				glDisableVertexAttribArray(vertexNormalLocation);
			}
			
			if ((data.uvs.size() > 0) && (vertexUVLocation != -1))
			{
				glDisableVertexAttribArray(vertexUVLocation);
			}
			

			glUseProgram(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		Model *Model::CreateModel(MODEL_TYPE ModelType, TEXTURE_TYPE TextureType, const char* atlasJson)
		{
			Model * pRet = NULL;
			switch (ModelType)
			{
			case MODEL_SKYBOX:
				{
					pRet = new SkyboxModel(TextureType);
					break;
				}
			case MODEL_SPHERE_2D:
				{
					pRet = new SphereModel(ModelType, TextureType);
					break;
				}
				
			case 	MODEL_SPHERE_3D_UD:
				{
					pRet = new SphereModel(ModelType, TextureType);
					break;
				}
			case 	MODEL_SPHERE_3D_LR:
				{
					pRet = new SphereModel(ModelType, TextureType);
					break;
				}
			case MODEL_RECT_2D:
			{
				pRet = new RectangleModel(TextureType, atlasJson);
					break;
			}
			default:
				break;
			}
			if (pRet)
				pRet->Init();

			return pRet;
		}
	}
}
