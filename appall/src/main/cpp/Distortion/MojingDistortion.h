#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "../Base/MojingMath.h"
#include <vector>
#define DISTORTION_PARAMETES_COUNT 7
#define UNREAL_DISTORTION_PARAMETES_COUNT 9 // 增加两个元素，下标7为目标点的X坐标，下标8为目标点的Y坐标

namespace Baofeng
{
	namespace Mojing
	{

		struct DistortionVertexNode
		{
			DistortionVertexNode() :
				m_fX(0), m_fY(0),
				m_fBX(0), m_fBY(0),
				m_fGX(0), m_fGY(0),
				m_fRX(0), m_fRY(0),
				m_fOne(1), //== 1
				m_fZero(0) //== 0
			{
			};
			float m_fX, m_fY;
			float m_fRX, m_fRY; 
			float m_fGX, m_fGY; 
			float m_fBX, m_fBY;
			float m_fOne; //== 1
			float m_fZero; //== 0
			String GetString()const
			{
				static char szNote[512];
				sprintf(szNote , "{ FVector2D(%ff, %ff), FVector2D(%ff, %ff), FVector2D(%ff, %ff), FVector2D(%ff, %ff), %ff, %ff }" , 
					m_fX, m_fY, m_fBX, m_fBY, m_fGX, m_fGY, m_fRX, m_fRY, m_fOne, m_fZero);
				String S = szNote;
				return S;
			}
		};
		struct DistortionVertexBuffer
		{
			DistortionVertexBuffer():
			m_iWidth(0),
			m_iHeight(0),
			m_iVertexCount(0),
			m_iIndexCount(0),
			m_pDistortionVertexIndex(NULL),
			m_pDistortionVertex(NULL)
			{};
			// 使用顶点数构造，输入的宽、高是顶点的数量，比格子数量多1
			DistortionVertexBuffer(int iWidth, int iHeight) :
				m_iWidth(iWidth),
				m_iHeight(iHeight),
				m_iVertexCount(2 * m_iWidth * iHeight), // 双眼顶点总数
				m_iIndexCount(2 * ((m_iWidth - 1) * (iHeight - 1)) * 6) // 双眼顶点构成的格子总数
				
			{
				m_pDistortionVertex = new DistortionVertexNode[m_iVertexCount];
				m_pDistortionVertexIndex = new unsigned short[m_iIndexCount];
			};
			virtual ~DistortionVertexBuffer(){
				if (m_pDistortionVertex)
					delete m_pDistortionVertex;
				if (m_pDistortionVertexIndex)
					delete m_pDistortionVertexIndex;
			};
			// 得到左眼的特定顶点
			DistortionVertexNode* GetLeftVertexNode(int iX, int iY){
				return m_pDistortionVertex + iX + iY * m_iWidth;
			};

			// 得到右眼的特定顶点
			DistortionVertexNode* GetRightVertexNode(int iX, int iY){
				return GetLeftVertexNode(iX, iY) + m_iWidth * m_iHeight;
			};

			int GetIndexOfNode(DistortionVertexNode* pNode){
				return pNode - m_pDistortionVertex;
			}

			String GetString()const
			{
				String strRet;
				char szTemp[512];
				sprintf(szTemp, "static const uint32 NumVerts = %d;\nstatic const uint32 NumTris = %d / 3;\n " , 
					m_iVertexCount, m_iIndexCount);

				int iCol = sqrt(m_iVertexCount / 2.0);
				strRet += szTemp;
				strRet += "static const FDistortionVertex Verts[NumVerts] =\n\t{";
				for (int i = 0; i < m_iVertexCount; i++)
				{
					strRet += "\n\t\t";
					if (i == 0)
						strRet += "// left eye x in [-1 , 0] , y in [-1 , 1] && Color.X in [0 , 0.5] , Color.Y[0 , 1]\n\t\t";
					else if (i == m_iVertexCount / 2)
						strRet += "// right eye  x in [0 ,1] , y in [-1 , 1] && Color.X in [0.5 , 1] , Color.Y[0 , 1]\n\t\t";

					int iItem = i;
					if (i >= m_iVertexCount / 2)
					{
						iItem -= m_iVertexCount / 2;
					}
					sprintf(szTemp, "/* %3d , %3d*/ ", iItem % iCol , iItem / iCol);
					strRet += szTemp;
					strRet += m_pDistortionVertex[i].GetString() + ",";
				}
				strRet += "\n\t};\n\n";

				strRet += "static const uint16 Indices[] = { ";
				for (int i = 0; i < m_iIndexCount; i++)
				{ // 
					if (0 == (i % 6))
					{
						strRet += "\n\t";
					}
					
					sprintf(szTemp, "%4d ", m_pDistortionVertexIndex[i]);
					strRet += szTemp;
					if (i != m_iIndexCount - 1)
						strRet += ",";
				}
				strRet += "\n};";
				return strRet;
			}
			int m_iWidth; // 每一行有多少个顶点
			int m_iHeight;// 每一列有多少个顶点
			int m_iVertexCount;
			int m_iIndexCount;
			DistortionVertexNode* m_pDistortionVertex;
			unsigned short * m_pDistortionVertexIndex;
		};


		struct hmdInfoInternal_t
		{
			//	LensConfig lens;

			float	lensSeparation;		// in meters, 0.059f for prototype

			// These values are always as if the display is in landscape
			// mode, being swapped from the system values if the manifest
			// is configured for portrait.
			float	widthMeters;		// in meters, 0.1105f for Galaxy S4
			float	heightMeters;		// in meters, 0.06214f for Galaxy S4
			int		widthPixels;		// 1920
			int		heightPixels;		// 1080
			float	MetersPerTanAngleAtCenter;
			// This is a product of the lens distortion and the screen size,
			// but there is no truly correct answer.
			//
			// There is a tradeoff in resolution and coverage.
			// Too small of an fov will leave unrendered pixels visible, but too
			// large wastes resolution or fill rate.  It is unreasonable to
			// increase it until the corners are completely covered, but we do
			// want most of the outside edges completely covered.
			//
			// Applications might choose to render a larger fov when angular
			// acceleration is high to reduce black pull in at the edges by
			// TimeWarp.
			// float	eyeTextureFov;<--畸变中没有使用的变量
		};

		struct Mesh_820
		{
			std::vector<float> vertices_left;
			std::vector<float> vertices_right;
			std::vector<unsigned int> indices;
			void SaveToFile(const char* szFileName)const;
		};
		class Distortion
		{
			float m_KR[21];
			float m_KG[21];
			float m_KB[21];

			float m_KRT[21];
			float m_KGT[21];
			float m_KBT[21];

			float m_KL[21];// 亮度系数，默认是1

			void ReCalculationKT(float* fKT);
			void WarpTexCoordChroma(const hmdInfoInternal_t & hmdInfo, float in[2], float red[2], float green[2], float blue[2], float light[1]);
			// Vector3f DistortionFnScaleRadiusSquaredChroma(float rsq);
			Vector3f DistortionFnScaleRadiusSquared_V3(float rsq)const;
			Vector4f DistortionFnScaleRadiusSquared_V4(float rsq)const;
			Vector3f DistortionFnScaleRadiusSquared_Unreal(float rsq)const;

			float EvalCatmullRomSpline(float const *K, float scaledVal, int NumSegments)const;
			
			void * BuildDistortionBuffer(const hmdInfoInternal_t &HMDI, int eyeBlocksWide /* = 32*/, int eyeBlocksHigh/* = 32*/);
			void * BuildDistortionBuffer_V2(const hmdInfoInternal_t &HMDI, int eyeBlocksWide /* = 32*/, int eyeBlocksHigh/* = 32*/);
			DistortionVertexBuffer* BuildDistortionVertexBuffer(int eyeBlocksWide /* = 32*/, int eyeBlocksHigh/* = 32*/);
		public:
			Distortion();
			virtual ~Distortion();
			void * BuildDistortionBuffer(int eyeBlocksWide/* = 32*/, int eyeBlocksHigh/* = 32*/);
			
			// 参考谷歌的畸变方程和畸变算法，制作Mesh表
			void * BuildDistortionBufferFromDURL(Mesh_820& mesh, const int eyeBlocksWide/* = 32*/, const int eyeBlocksHigh/* = 32*/);

			/*0   1    2    3    4    5    6    7   8   9*/
			/*X , Y , Rx , Ry , Gx , Gy , Bx , By , A , 0*/
			void * BuildDistortionBuffer_V2(Mesh_820& mesh, int eyeBlocksWide/* = 32*/, int eyeBlocksHigh/* = 32*/);
			void * BuildDistortionBufferOverlay(int eyeBlocksWide/* = 32*/, int eyeBlocksHigh/* = 32*/, unsigned int OverlayWidth, unsigned int OverlayHeight);
			int UNREAL_BuildDistortionMesh(int eyeBlocksWide /* = 40*/, int eyeBlocksHigh/* = 40*/, void* fppVertex, void* fppIndex);
			int UNITY_BuildDistortionMesh(int eyeBlocksWide /* = 40*/, int eyeBlocksHigh/* = 40*/, void* fppVertex, void* fppUV, void* fppIndex);
			CLASS_MEMBER(float , m_f , MetersPerTanAngleAtCenter);
			CLASS_MEMBER_STR(String, m_sz, GlassKey);
			CLASS_MEMBER_STR(String, m_sz, DURL);
			CLASS_MEMBER(float, m_f, LensSeparation);
			CLASS_INTERFACE(int, m_i, Segment);
			CLASS_MEMBER(float, m_f, YOffset);
			CLASS_MEMBER(float, m_f, FOV);
			// 是否没有色散
			CLASS_INTERFACE(bool, m_b, NoDispersion);
			bool SaveToFile(const char* szFilePathName)const;
			void SetDistortionParamet(int iSegment, bool bNoDispersion , float* fKR, float * fKG, float * fKB, float *fKL = NULL, float * fKRT = NULL, float * fKGT = NULL, float * fKBT = NULL);
			int GetDistortionParamet(float* fKR, float * fKG, float * fKB);
			bool IsDistortionNeeded(void) { return m_iSegment != 0; };
			
		};
	}//namespace Mojing
}//namespace Baofeng

