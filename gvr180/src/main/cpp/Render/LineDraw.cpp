
#include "LineDraw.h"
#include "../Base/MojingMath.h"
#include "../MojingManager.h"
#include "../Distortion/MojingDistortion.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDisplayParameters.h"
#include "../Base/MojingTypes.h"
#include "../Base/BuildType.h"
#include "../Base/MojingTimer.h"
#include "../MojingAPI.h"
#include "../MojingSDKStatus.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
#define DRAWLINEPROGRAM_VERTEX_ATTRIBUTE_LOCATION_POSITION 0
		DrawLineProgram::DrawLineProgram() : m_Program(0), m_BuildFailed(false)
		{
			m_VertexSrc =
				"uniform mat4 Mvpm;\n"
				"uniform vec4 vColor;\n"
				"attribute vec4 Position;\n"
				"varying vec4 oColor;\n"
				"void main()\n"
				"{\n"
				"	gl_Position = Mvpm * Position;\n"
				"	oColor = vColor;\n"
				"}\n";

			m_FragmentSrc =
#ifdef MJ_OS_WIN32
				"varying vec4 oColor;\n"
#else
				"varying highp vec4 oColor;\n"
#endif
				"void main()\n"
				"{\n"
				"	gl_FragColor = oColor;\n"
				"}\n";

			SetLineColor(1, 1, 1);
		}

		bool DrawLineProgram::CompileShader(const GLuint shader, const char * src)
		{
			glShaderSource(shader, 1, &src, 0);
			glCompileShader(shader);

			GLint r;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
			if (r == GL_FALSE)
			{
				GLchar msg[4096];
				glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
				return false;
			}
			return true;
		}

		bool DrawLineProgram::BuildProgram()
		{
			m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
			if (!CompileShader(m_VertexShader, m_VertexSrc))
			{
				return false;
			}
			m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			if (!CompileShader(m_FragmentShader, m_FragmentSrc))
			{
				return false;
			}

			m_Program = glCreateProgram();
			glAttachShader(m_Program, m_VertexShader);
			glAttachShader(m_Program, m_FragmentShader);

			glBindAttribLocation(m_Program, DRAWLINEPROGRAM_VERTEX_ATTRIBUTE_LOCATION_POSITION, "Position");

			// link and error check
			glLinkProgram(m_Program);
			GLint r;
			glGetProgramiv(m_Program, GL_LINK_STATUS, &r);
			if (r != GL_FALSE)
			{
				m_MvpMatrix = glGetUniformLocation(m_Program, "Mvpm");
				m_LineColor = glGetUniformLocation(m_Program, "vColor");
			}
			else
			{
				GLchar msg[1024];
				glGetProgramInfoLog(m_Program, sizeof(msg), 0, msg);
			}
			return true;
		}

		void DrawLineProgram::SetLineColor(float r, float g, float b)
		{
			m_LineColorValue[0] = r;
			m_LineColorValue[1] = g;
			m_LineColorValue[2] = b;
			m_LineColorValue[3] = 1.0f;
		}

		void DrawLineProgram::UseProgram(bool bLandscape)
		{
			if (m_BuildFailed)
				return;

			const Matrix4f sDefaultMatrix(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);

			const Matrix4f LandscapeOrientationMatrix = bLandscape ? Matrix4f(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				) :
				Matrix4f(
				0.0f, -1.0f, 0.0f, 0.0f, 
				1.0f, 0.0f, 0.0f, 0.0f,				
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);

			if (!glIsProgram(m_Program) || m_Program == 0)
			{
				m_BuildFailed = !BuildProgram();
			}

			if (!m_BuildFailed)
			{
				glUseProgram(m_Program);

				Matrix4f Result;
				Matrix4f::Multiply(&Result, LandscapeOrientationMatrix, sDefaultMatrix);

				glUniformMatrix4fv(m_MvpMatrix, 1, GL_FALSE, Result.M[0]);

				if (m_LineColor >= 0)
					glUniform4f(m_LineColor,
					m_LineColorValue[0],
					m_LineColorValue[1],
					m_LineColorValue[2],
					m_LineColorValue[3]);
			}
		}

		LineDraw::LineDraw() : m_VertexBuffer(0), m_IndexBuffer(0), m_BuildFailed(false)
		{}

		void LineDraw::AddVertex(float x, float y)
		{
			m_VertexArray.push_back(Point(x, y));
		}

		int LineDraw::GetVertexCount() const
		{
			return m_VertexArray.size();
		}

		void LineDraw::SetLineColor(float r, float g, float b)
		{
			m_DrawLineProgram.SetLineColor(r, g, b);
		}

		bool LineDraw::Build()
		{
			//============================  ============================//

			int nFloatCount = m_VertexArray.size() * 2;
			if (nFloatCount == 0)
				return false;

			float* pVertexBuff = new float[nFloatCount * 2];

			for (int i = 0; i < (int)m_VertexArray.size(); i++)
			{
				pVertexBuff[i * 2] = m_VertexArray[i].x;
				pVertexBuff[i * 2 + 1] = m_VertexArray[i].y;
			}
			if (!glIsBuffer(m_VertexBuffer) || 0 == m_VertexBuffer)
			{
				glGenBuffers(1, &m_VertexBuffer);
			}
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, nFloatCount * sizeof(float), (void*)pVertexBuff, GL_STATIC_DRAW);
			delete[] pVertexBuff;

			//============================  ============================//

			m_IndexCount = m_VertexArray.size();
			unsigned short* pIndexBuff = new unsigned short[m_IndexCount];
			for (int i = 0; i < m_IndexCount; i++)
				pIndexBuff[i] = i;

			if (0 == m_IndexBuffer || !glIsBuffer(m_IndexBuffer))
			{
				glGenBuffers(1, &m_IndexBuffer);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(unsigned short), (void*)pIndexBuff, GL_STATIC_DRAW);
			delete[] pIndexBuff;

			//============================  ============================//

			glEnableVertexAttribArray(DRAWLINEPROGRAM_VERTEX_ATTRIBUTE_LOCATION_POSITION);
			glVertexAttribPointer(DRAWLINEPROGRAM_VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT,
				false, 0, (void *)(0 * sizeof(float)));

			return true;
		}

		void LineDraw::Draw(GLenum drawLineMode, float lineWidth, bool bLandscape)
		{
			if (lineWidth <= 0.0f || m_BuildFailed)
				return;

			m_DrawLineProgram.UseProgram(bLandscape);

			glLineWidth(lineWidth);
			if (!glIsBuffer(m_VertexBuffer) || m_VertexBuffer == 0)
				m_BuildFailed = !Build();

			if (m_VertexBuffer != 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

				glEnableVertexAttribArray(DRAWLINEPROGRAM_VERTEX_ATTRIBUTE_LOCATION_POSITION);
				glVertexAttribPointer(DRAWLINEPROGRAM_VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT,
					false, 2 * sizeof(float), (void *)(0 * sizeof(float)));

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

				glDrawElements(drawLineMode, m_IndexCount, GL_UNSIGNED_SHORT, (void*)(0));

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
			// 保护代码
			glUseProgram(0);
		}


		/*======================================================

		class: DrawGeometryProgram

		=======================================================*/
		DrawGeometryProgram::DrawGeometryProgram() : m_Program(0), m_BuildFailed(false)
		{
			m_VertexSrc =
				"uniform mat4 Mvpm;\n"
				"attribute vec4 Position;\n"
				"attribute vec2 TexCoord;\n"
				"varying vec2 oTexCoord;\n"
				"void main()\n"
				"{\n"
				"	oTexCoord = TexCoord;\n"
				"	gl_Position = Mvpm * Position;\n"
				"}\n";

			m_FragmentSrc =
				"uniform sampler2D Texture0;\n"
#ifdef MJ_OS_WIN32
				"varying vec2 oTexCoord;\n"
#else
				"varying highp vec2 oTexCoord;\n"
#endif
				"void main()\n"
				"{\n"
				"	gl_FragColor = texture2D(Texture0, oTexCoord);\n"
				// 		"	gl_FragColor.r = 1.0 - gl_FragColor.r;\n"
				// 		"	gl_FragColor.g = 1.0 - gl_FragColor.g;\n"
				// 		"	gl_FragColor.b = 1.0 - gl_FragColor.b;\n"
				"}\n";

			m_MvpMatrixData = Matrix4f(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}

		/*------------------------------------------------------------------
		2015/09/11:
		------------------------------------------------------------------*/
		bool DrawGeometryProgram::CompileShader(const GLuint shader, const char * src)
		{
			glShaderSource(shader, 1, &src, 0);
			glCompileShader(shader);

			GLint r;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
			if (r == GL_FALSE)
			{
				GLchar msg[4096];
				glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
				return false;
			}
			return true;
		}

		/*------------------------------------------------------------------
		2015/09/11:
		------------------------------------------------------------------*/
		bool DrawGeometryProgram::BuildProgram()
		{
			m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
			if (!CompileShader(m_VertexShader, m_VertexSrc))
			{
				return false;
			}
			m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			if (!CompileShader(m_FragmentShader, m_FragmentSrc))
			{
				return false;
			}

			m_Program = glCreateProgram();
			glAttachShader(m_Program, m_VertexShader);
			glAttachShader(m_Program, m_FragmentShader);

			glBindAttribLocation(m_Program, VERTEX_ATTRIBUTE_POSITION, "Position");
			glBindAttribLocation(m_Program, VERTEX_ATTRIBUTE_TEX_COORD, "TexCoord");

			// link and error check
			glLinkProgram(m_Program);
			GLint r;
			glGetProgramiv(m_Program, GL_LINK_STATUS, &r);
			if (r != GL_FALSE)
			{
				m_MvpMatrix = glGetUniformLocation(m_Program, "Mvpm");
				MOJING_TRACE(g_APIlogger, "BuildLineShaderSuccess");

				// texture and image_external bindings
				for (int i = 0; i < 8; i++)
				{
					char name[32];
					sprintf(name, "Texture%i", i);
					const GLint uTex = glGetUniformLocation(m_Program, name);
					if (uTex != -1)
					{
						glUniform1i(uTex, i);
					}
				}
			}
			else
			{
				GLchar msg[1024];
				glGetProgramInfoLog(m_Program, sizeof(msg), 0, msg);

				MOJING_TRACE(g_APIlogger, "BuildLineShaderFailed: " << msg);
			}
			return true;
		}

		void DrawGeometryProgram::SetMvpMatrix(Matrix4f A_Matrix)
		{
			m_MvpMatrixData = A_Matrix;
		}

		void DrawGeometryProgram::UseProgram()
		{
			if (m_BuildFailed)
				return;

			const Matrix4f sDefaultMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);

			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
			if (!glIsProgram(m_Program) || m_Program == 0 /*|| pStatus->GetDirty()*/)
			{
				if (!BuildProgram())
					m_BuildFailed = true;
			}

			if (!m_BuildFailed)
			{
				glUseProgram(m_Program);

				Matrix4f Result;
				Matrix4f::Multiply(&Result, m_MvpMatrixData, sDefaultMatrix);

				glUniformMatrix4fv(m_MvpMatrix, 1, GL_FALSE, Result.M[0]);
			}
		}
		/*======================================================



		=======================================================*/
		GeometryBatch::GeometryBatch() : m_VertexBuffer(0), m_IndexBuffer(0), m_BuildSuccess(true)
		{}

		void GeometryBatch::AddVertex(float x, float y)
		{
			m_VertexArray.push_back(Point(x, y));
		}

		void GeometryBatch::AddVertex(float x, float y, float s, float t)
		{
			m_VertexArray.push_back(Point(x, y, s, t));
		}

		int GeometryBatch::GetVertexCount() const
		{
			return m_VertexArray.size();
		}

		/*------------------------------------------------------------------
		2015/09/11:
		------------------------------------------------------------------*/
		bool GeometryBatch::Build()
		{
			//============================  ============================//

			int nFloatCount = m_VertexArray.size() * 4;
			if (nFloatCount == 0)
				return false;

			float* pVertexBuff = new float[nFloatCount * 4];

			for (uint32_t i = 0; i < m_VertexArray.size(); i++)
			{
				pVertexBuff[i * 4 + 0] = m_VertexArray[i].x;
				pVertexBuff[i * 4 + 1] = m_VertexArray[i].y;

				pVertexBuff[i * 4 + 2] = m_VertexArray[i].s;
				pVertexBuff[i * 4 + 3] = m_VertexArray[i].t;
			}
			if (0 == m_VertexBuffer || !glIsBuffer(m_VertexBuffer))
			{
				glGenBuffers(1, &m_VertexBuffer);
			}
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, nFloatCount * sizeof(float), (void*)pVertexBuff, GL_STATIC_DRAW);
			delete[] pVertexBuff;

			//============================  ============================//

			m_IndexCount = m_VertexArray.size();
			unsigned short* pIndexBuff = new unsigned short[m_IndexCount];
			for (int i = 0; i < m_IndexCount; i++)
				pIndexBuff[i] = i;

			if (0 == m_IndexBuffer || !glIsBuffer(m_IndexBuffer))
			{
				glGenBuffers(1, &m_IndexBuffer);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(unsigned short), (void*)pIndexBuff, GL_STATIC_DRAW);
			delete[] pIndexBuff;

			//============================  ============================//

			glEnableVertexAttribArray(DrawGeometryProgram::VERTEX_ATTRIBUTE_POSITION);
			glVertexAttribPointer(DrawGeometryProgram::VERTEX_ATTRIBUTE_POSITION, 2, GL_FLOAT,
				false, 4 * sizeof(float), (void *)(0 * sizeof(float)));

			glEnableVertexAttribArray(DrawGeometryProgram::VERTEX_ATTRIBUTE_TEX_COORD);
			glVertexAttribPointer(DrawGeometryProgram::VERTEX_ATTRIBUTE_TEX_COORD, 2, GL_FLOAT,
				false, 4 * sizeof(float), (void *)(2 * sizeof(float)));

			return true;
		}

		DrawGeometryProgram& GeometryBatch::GetProgram()
		{
			return m_DrawGeoProgram;
		}

		/*------------------------------------------------------------------
		2015/09/11:
		------------------------------------------------------------------*/
		void GeometryBatch::Draw(GLenum drawGeoMode)
		{
			if (!m_BuildSuccess)
				return;

			//MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();

			m_DrawGeoProgram.UseProgram();

			if (!glIsBuffer(m_VertexBuffer) || m_VertexBuffer == 0 /*|| pStatus->GetDirty()*/)
				m_BuildSuccess = Build();

			if (m_VertexBuffer != 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

				glEnableVertexAttribArray(DrawGeometryProgram::VERTEX_ATTRIBUTE_POSITION);
				glVertexAttribPointer(DrawGeometryProgram::VERTEX_ATTRIBUTE_POSITION, 2,
					GL_FLOAT, false, 4 * sizeof(float), (void *)(0 * sizeof(float)));

				glEnableVertexAttribArray(DrawGeometryProgram::VERTEX_ATTRIBUTE_TEX_COORD);
				glVertexAttribPointer(DrawGeometryProgram::VERTEX_ATTRIBUTE_TEX_COORD, 2,
					GL_FLOAT, false, 4 * sizeof(float), (void *)(2 * sizeof(float)));

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

				glDrawElements(drawGeoMode, m_IndexCount, GL_UNSIGNED_SHORT, (void*)(0));

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glUseProgram(0);
			}
		}
		
		///////////////////////////////////
		DrawQuadProgram::DrawQuadProgram()
			: m_BuildFailed()
			, m_VertexSrc()
			, m_FragmentSrc()
			, m_Program()
			, m_VertexShader()
			, m_FragmentShader()
			, m_MvpMatrixData()
				, m_Texture(0)
		{
			m_VertexSrc =
#ifdef MJ_OS_WIN32
			"attribute vec3 Position;\n"
			"attribute vec2 TexCoord;\n"
			"varying vec2 oUV;\n"
#else
			"attribute highp vec3 Position;\n"
			"attribute highp vec2 TexCoord;\n"
			"varying highp vec2 oUV;\n"
#endif
			"uniform mat4 Mvpm;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = Mvpm * vec4(Position, 1.0);\n"
				"oUV = TexCoord;\n"
			"}\n";

			m_FragmentSrc =
				"uniform   sampler2D  mtexture;\n "
#ifdef MJ_OS_WIN32
				"varying vec2 oUV;\n"
#else
				"varying highp vec2 oUV;\n"
#endif
				"void main()\n"
				"{\n"
				"	gl_FragColor = texture2D(mtexture, oUV);\n"
				"}\n";
			
			m_MvpMatrixData = Matrix4f(
				1.0f,0.0f,0.0f,0.0f,
				0.0f,1.0f,0.0f,0.0f,
				0.0f,0.0f,0.0f,0.0f,
				0.0f,0.0f,0.0f,1.0f);
		}
		
		void DrawQuadProgram::UseProgram()
		{
			if (m_BuildFailed)
				return;

			const Matrix4f sDefaultMatrix(
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1);

			const Matrix4f LandscapeOrientationMatrix(
				1.0f,0.0f,0.0f,0.0f,
				0.0f,1.0f,0.0f,0.0f,
				0.0f,0.0f,0.0f,0.0f,
				0.0f,0.0f,0.0f,1.0f);
			if (!glIsProgram(m_Program) || m_Program == 0)
			{
				m_BuildFailed = !BuildProgram();
			}

			if (!m_BuildFailed)
			{
				glUseProgram(m_Program);

				Matrix4f Result;
				Matrix4f::Multiply(&Result, LandscapeOrientationMatrix, sDefaultMatrix);

				glUniformMatrix4fv(m_MvpMatrix, 1, GL_FALSE, Result.M[0]);

			}
		}
		
		

		bool DrawQuadProgram::CompileShader(const GLuint shader, const char* src)
		{
			glShaderSource(shader, 1, &src, 0);
			glCompileShader(shader);

			GLint r;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
			if (r == GL_FALSE)
			{
				GLchar msg[4096];
				glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
				return false;
			}
			return true;
		}

		
		bool DrawQuadProgram::BuildProgram()
		{
			m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
			if (!CompileShader(m_VertexShader, m_VertexSrc))
			{
				return false;
			}
			m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			if (!CompileShader(m_FragmentShader, m_FragmentSrc))
			{
				return false;
			}

			m_Program = glCreateProgram();
			glAttachShader(m_Program, m_VertexShader);
			glAttachShader(m_Program, m_FragmentShader);

			glBindAttribLocation(m_Program, VERTEX_ATTRIBUTE_POSITION, "Position");
			glBindAttribLocation(m_Program, VERTEX_ATTRIBUTE_TEX_COORD, "TexCoord");
			// link and error check
			glLinkProgram(m_Program);
			GLint r;
			glGetProgramiv(m_Program, GL_LINK_STATUS, &r);
			if (r != GL_FALSE)
			{
				m_MvpMatrix = glGetUniformLocation(m_Program, "Mvpm");
				//m_MvpMatrix = glGetUniformLocation(m_Program, "Mvpm");
			//	m_color = glGetUniformLocation(m_Program, "vColor");
				m_Texture =  glGetUniformLocation(m_Program, "mtexture");
			}
			else
			{
				GLchar msg[1024];
				glGetProgramInfoLog(m_Program, sizeof(msg), 0, msg);
			}
			return true;
		}
		
		void DrawQuadProgram::UnUseProgram()
		{
			glUseProgram(0);
		}
		
		
		
		
		/////////////////////
		
		QuadDraw::QuadDraw()
			: m_DrawQuedProgram()
			, mVBO()
			, mIBO()
			, m_VertexCount()
			, m_IndexCount()
			, mTextureID()
			, m_BuildFailed(false)
			, mPoint(-1.0, 1.0, 0.0)
			, mOffset(1.0, 2.0, 0.0)
		{

		}
		QuadDraw::~QuadDraw()
		{
			glDeleteBuffers(1, &mVBO);
			glDeleteBuffers(1, &mIBO);

			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
		
		}
		
		void QuadDraw::Begin()
		{
			m_DrawQuedProgram.UseProgram();
			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION);
			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_TEX_COORD);
		}
		
				
		void QuadDraw::End()
		{
			glDisableVertexAttribArray(VERTEX_ATTRIBUTE_TEX_COORD);
			glDisableVertexAttribArray(VERTEX_ATTRIBUTE_POSITION);
			glUseProgram(0);
		}
		bool QuadDraw::Build(int eye)
		{
			m_VertexCount = 4 * 2;
			m_IndexCount = 6 * 2;
			unsigned short triangleIndex[] = {
				0,1,2,
				0,2,3,
			    4, 5,6,
				4, 6, 7
			};

			GLfloat vertexUV[] = {
				0.0f,0.0f,
				0.0f,1.0f,
				1.0f,1.0f,
				1.0f,.0f,
				0.0f,0.0f,
				0.0f,1.0f,
				1.0f,1.0f,
				1.0f,.0f,
					
			};
			GLfloat triangleVertices[24] =  {
				    0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f
			};
		//	if (eye == 0)
			{
			
					//定点数据已经索引生成
				triangleVertices[0] = mPoint.x;
				triangleVertices[1] = mPoint.y;
				triangleVertices[2] = mPoint.z;
				
				triangleVertices[3] = mPoint.x;
				triangleVertices[4] = mPoint.y - mOffset.y;
				triangleVertices[5] = mPoint.z;
				
				triangleVertices[6] = mPoint.x + mOffset.x;
				triangleVertices[7] = mPoint.y - mOffset.y; 
				triangleVertices[8] = mPoint.z;
				
				triangleVertices[9] = mPoint.x + mOffset.x; 
				triangleVertices[10] = mPoint.y;
				triangleVertices[11] = mPoint.z;
					
			}
	//		else
			{
				mPoint +=  Vector3f(1.0, 0.0, 0.0);
				
					//定点数据已经索引生成
				triangleVertices[12] = mPoint.x;
				triangleVertices[13] = mPoint.y;
				triangleVertices[14] = mPoint.z;
				
				triangleVertices[15] = mPoint.x;
				triangleVertices[16] = mPoint.y - mOffset.y;
				triangleVertices[17] = mPoint.z;
				
				triangleVertices[18] = mPoint.x + mOffset.x;
				triangleVertices[19] = mPoint.y - mOffset.y; 
				triangleVertices[20] = mPoint.z;
				
				triangleVertices[21] = mPoint.x + mOffset.x; 
				triangleVertices[22] = mPoint.y;
				triangleVertices[23] = mPoint.z;
			}
			
			//if (!mVBO)
			{
				glGenBuffers(1, &mVBO);
				glBindBuffer(GL_ARRAY_BUFFER, mVBO);
				glBufferData(GL_ARRAY_BUFFER,
					sizeof(triangleVertices),
					&triangleVertices,
					GL_STATIC_DRAW);
			}
		//	if (!mUV)
			{
				glGenBuffers(1, &mUV);
				glBindBuffer(GL_ARRAY_BUFFER, mUV);
				glBufferData(GL_ARRAY_BUFFER,
					sizeof(vertexUV),
					&vertexUV,
					GL_STATIC_DRAW);
			}

		//	if (!mIBO)
			{
				glGenBuffers(1, &mIBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2 * sizeof(unsigned short), &triangleIndex[0], GL_STATIC_DRAW);
			}
			
			return true;
		}
		
	


		void QuadDraw::BindTexture(GLuint texId)
		{
			mTextureID = texId;
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, mTextureID);
			//glUniform1i(m_DrawQuedProgram.m_Texture, 0);
		}


		void QuadDraw::UnBindTeture()
		{
			glDeleteTextures(1, &mTextureID);
		}


		void QuadDraw::SetPositionVector(Vector3f& TopLeft, 
			Vector3f& Offset)
		{
		//	GLint view[4];
		//	glGetIntegerv(GL_VIEWPORT, view); //viewport
			
		//	glViewPort();
			mPoint = TopLeft;
			mOffset = Offset;
			Build();
		}


		void QuadDraw::Draw(GLenum drawQuadMode,int eye)
		{
			
			if (m_BuildFailed)
			{
				return;
			}
			if (!glIsBuffer(mVBO) || mVBO == 0)
				m_BuildFailed = !Build(eye);
			
		/*
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), 0);
			glBindBuffer(GL_ARRAY_BUFFER, mUV);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2f), (const GLvoid*)0);
			
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
			   // Bind the texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mTextureID);
			glUniform1i(m_DrawQuedProgram.m_Texture, 0);
			
			glDrawElements(drawQuadMode, m_IndexCount, GL_UNSIGNED_SHORT,0);
			*/
			//绑定顶点数据
			Begin();
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), (const GLvoid*)12);
			glBindBuffer(GL_ARRAY_BUFFER, mUV);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mTextureID);
			glUniform1i(m_DrawQuedProgram.m_Texture, 0);

			glDrawElements(drawQuadMode, 6 * 2, GL_UNSIGNED_SHORT, 0);
			End();
	
		}

	}
}
