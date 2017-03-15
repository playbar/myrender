#ifndef _H_DRAWLINE_
#define _H_DRAWLINE_
#include <vector>
#include "../Base/GlUtils.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingMath.h"
#include "../Base/MojingThreads.h"

/*======================================================

		绫?
    			m_LineBatch.SetLineColor(0, 1, 1);
          m_LineBatch.AddVertex(0.0f, -1.0f);
          m_LineBatch.AddVertex(0.0f, 1.0f);
          
          m_LineBatch.Draw(GL_LINES, 1.0f);

		=======================================================*/
namespace Baofeng
{
	namespace Mojing
	{
		class DrawLineProgram
		{
		private:
			bool m_BuildFailed;
			const char* m_VertexSrc;
			const char* m_FragmentSrc;
			unsigned m_Program;
			unsigned m_VertexShader;
			unsigned m_FragmentShader;
			float m_LineColorValue[4];

			int m_MvpMatrix;
			int m_LineColor;

			bool CompileShader(const GLuint shader, const char* src);
			bool BuildProgram();

		public:
			DrawLineProgram();

			void SetLineColor(float r, float g, float b);
			void UseProgram(bool bLandscape);
		};

		/*======================================================

		绫?

		=======================================================*/
		class LineDraw
		{
			struct Point
			{
				float x;
				float y;

				Point() {}
				Point(float A_x, float A_y) : x(A_x), y(A_y){}
			};
		private:
			bool		m_BuildFailed;
			unsigned 	m_VertexBuffer;
			unsigned 	m_IndexBuffer;
			unsigned 	m_VertexArrayObject;
			int			m_VertexCount;
			int 		m_IndexCount;

			DrawLineProgram m_DrawLineProgram;
			std::vector<Point> m_VertexArray;
			bool Build();
		public:
			LineDraw();

			void AddVertex(float x, float y);
			int GetVertexCount() const;

			void SetLineColor(float r, float g, float b);

			void Draw(GLenum drawLineMode, float lineWidth = 2.0f, bool bLandscape = true);
		};

		/*======================================================

			class:

		=======================================================*/
		class DrawGeometryProgram
		{
		public:
			enum
			{
				VERTEX_ATTRIBUTE_POSITION = 0,
				VERTEX_ATTRIBUTE_TEX_COORD = 1,
			};

		private:
			bool m_BuildFailed;
			const char* m_VertexSrc;
			const char* m_FragmentSrc;
			unsigned m_Program;
			unsigned m_VertexShader;
			unsigned m_FragmentShader;

			int m_MvpMatrix;

			Matrix4f m_MvpMatrixData;

			bool CompileShader(const GLuint shader, const char* src);
			bool BuildProgram();

		public:
			DrawGeometryProgram();

			void SetMvpMatrix(Matrix4f A_Matrix);
			void UseProgram();
		};

		/*======================================================

			class: draw geometry for convenient

		=======================================================*/
		class GeometryBatch
		{
		private:
			struct Point
			{
				float x, y;
				float s, t;

				Point() {}
				Point(float A_x, float A_y) : x(A_x), y(A_y), s(A_x), t(A_y){}
				Point(float A_x, float A_y, float A_s, float A_t) :
					x(A_x), y(A_y), s(A_s), t(A_t){}
			};

		private:
			bool		m_BuildSuccess;
			unsigned 	m_VertexBuffer;
			unsigned 	m_IndexBuffer;
			unsigned 	m_VertexArrayObject;
			int			m_VertexCount;
			int 		m_IndexCount;

			DrawGeometryProgram m_DrawGeoProgram;
			std::vector<Point> m_VertexArray;

		public:
			GeometryBatch();
			void AddVertex(float x, float y);
			void AddVertex(float x, float y, float s, float t);
			int GetVertexCount() const;
			bool Build();
			void Draw(GLenum drawGeoMode);
			DrawGeometryProgram& GetProgram();
		};
		
		
		////
		//clASS
		enum
		{
			VERTEX_ATTRIBUTE_POSITION  = 0,
			VERTEX_ATTRIBUTE_TEX_COORD = 1,
		};
		class DrawQuadProgram
		{
		public:
		
			DrawQuadProgram();
			~DrawQuadProgram() {}
			;
			
			void UseProgram();
			void UnUseProgram();
		private:
			bool CompileShader(const GLuint shader, const char* src);
			bool BuildProgram();
		
		private:
			bool m_BuildFailed;
			const char* m_VertexSrc;
			const char* m_FragmentSrc;
			unsigned m_Program;
			unsigned m_VertexShader;
			unsigned m_FragmentShader;
			
			Matrix4f m_MvpMatrixData;
		public:	
			int m_MvpMatrix;
			int m_color;
			int m_Texture;
		};
		
		
		class QuadDraw
		{
		public:
			QuadDraw();
			~QuadDraw();
			void BindTexture(GLuint texId);
			void UnBindTeture();

			void SetPositionVector(Vector3f& TopLeft, Vector3f& Offset);//左上角坐标，宽高
			void Draw(GLenum drawQuadMode, int eye = 0);//0 表示左眼睛，1表示右眼。
		private:
			bool Build(int eye = 0);
			void Begin();
			void End();
			DrawQuadProgram m_DrawQuedProgram;
			GLuint  mVBO;
			GLuint  mIBO;
			GLuint  mUV;
			
			int		m_VertexCount;
			int 	m_IndexCount;
			
			
			Vector3f  mPoint;//左上角坐标，针对左眼。
			Vector3f  mOffset;//偏移，宽，高
			GLuint    mTextureID;//纹理
			
			bool  m_BuildFailed;
		};
	}
}
#endif
