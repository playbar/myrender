#pragma once
#include "../../Base/MojingTypes.h"
#include "../../Base/MojingMath.h"
#include "../../Profile/ProfileV2/DayDreamParameters.h"
#include <vector>
using namespace std;
typedef vector<double> DoubleArray;

namespace Baofeng
{
	namespace Mojing
	{
		struct Mesh_820;
		struct Screen {
			float width;   // The long edge of the phone.
			float height;  // The short edge of the phone.
			float border;  // Distance from bottom of the phone to the bottom edge of screen.
		};

		struct Lenses {
			float separation;     // Center to center.
			float offset;         // Offset of lens center from top or bottom of viewer.
			float screenDistance; // Distance from lens center to the phone screen.

			enum {
				AlignTop = -1,
				AlignCenter = 0,
				AlignBottom = 1
			}alignment;  // Determines whether lenses are placed relative to top, bottom or
			// center.  It is actually a signum (-1, 0, +1) relating the scale of
			// the offset's coordinates to the device coordinates.

			// 				const int AlignTop = -1;    // Offset is measured down from top of device.
			// 				const int AlignCenter = 0;  // Center alignment ignores offset, hence scale is zero.
			// 				const int AlignBottom = 1;  // Offset is measured up from bottom of device.
		};

		struct MaxFOV {
			float outer;  // Towards the side of the screen.
			float inner;  // Towards the center line of the screen.
			float upper;  // Towards the top of the screen.
			float lower;  // Towards the bottom of the screen.
		};

		struct GvrDistortion {
			GvrDistortion(){};
			GvrDistortion(DoubleArray Coef)
			{
				m_Coef = Coef;
			};
			GvrDistortion(double* pCoef, int iCount)
			{
				while (iCount--)
				{
					m_Coef.push_back(pCoef[iCount]);
				}
			};

			CLASS_MEMBER(DoubleArray, m_, Coef);

			float distort(float r)const {
				float r2 = r * r;
				float ret = 0;
				int iSize = m_Coef.size() - 1;
				while (iSize >= 0)
				{
					ret = r2 * (ret + m_Coef[iSize]);
					iSize--;
				}

				return (ret + 1) * r;
			}

			float distortInv(float radius)const {
				// Secant method.
// 				float r0 = 0;
// 				float r1 = 1;

				float r0 = radius / 0.9;
				float r1 = radius * 0.9;

				float dr0 = radius - distort(r0);
				while (abs(r1 - r0) > 0.0001f) {
					float dr1 = radius - distort(r1);
					float r2 = r1 - dr1 * ((r1 - r0) / (dr1 - dr0));
					r0 = r1;
					r1 = r2;
					dr0 = dr1;
				}
				return r1;
			}

			String ToString()const
			{
				String Ret = "[ ";
				char szTemp[18];
				for (int i = 0; i < m_Coef.size(); i++)
				{
					sprintf(szTemp , "%4f" , m_Coef[i]);
					Ret += szTemp;
					if (i != m_Coef.size() - 1)
					{
						Ret += " , ";
					}
				}
				Ret += " ]";
				return Ret;
			}
		};


		struct Viewer {
			Lenses lenses;
			MaxFOV maxFOV;
			GvrDistortion  distortion;
			GvrDistortion  inverse;
			virtual void SetCoef(DoubleArray coef){
				distortion = GvrDistortion(coef);
				ApproximateInverse();
			};
			virtual void SetCoef(double* pCoef, int iCount){
				distortion = GvrDistortion(pCoef, iCount);
				ApproximateInverse();
			};
			virtual void SetFOV(float fFOV){
				maxFOV.inner = maxFOV.lower = maxFOV.outer = maxFOV.upper = fFOV / 2;			
			};
			virtual void ApproximateInverse(float maxRadius = 1, int numSamples = 100);

		};

		struct EyeVisibleTanAngles
		{
			EyeVisibleTanAngles();
			EyeVisibleTanAngles(const float Left,
				const float Top,
				const float Right,
				const float Bottom
				);
			EyeVisibleTanAngles(const EyeVisibleTanAngles& Other);

			float m_fLeft;
			float m_fTop;
			float m_fRight;
			float m_fBottom;
			float& operator [](int i){				
				switch (i)
				{
				case 1:
					return m_fTop; 
				case 2:
					return  m_fRight; 
				case 3:
					return  m_fBottom;
				default:
					return  m_fLeft;
					break;
				}
				return *((float*)NULL);// ERROR!!!
			};

			// 交集运算
			EyeVisibleTanAngles operator & (const EyeVisibleTanAngles Other)
			{
				EyeVisibleTanAngles Ret(
					fmax(m_fLeft , Other.m_fLeft),
					fmin(m_fTop, Other.m_fTop),
					fmin(m_fRight, Other.m_fRight),
					fmax(m_fBottom, Other.m_fBottom)
					);
				return Ret;
			}

			// 并集运算
			EyeVisibleTanAngles operator | (const EyeVisibleTanAngles Other)
			{
				EyeVisibleTanAngles Ret(
					fmin(m_fLeft, Other.m_fLeft),
					fmax(m_fTop, Other.m_fTop),
					fmax(m_fRight, Other.m_fRight),
					fmin(m_fBottom, Other.m_fBottom)
					);
				return Ret;
			}
			String ToString()
			{
				String S;
				char szTemp[512];
				sprintf(szTemp , "[ %.4f ,  %.4f ,  %.4f ,  %.4f ]", m_fLeft, m_fTop , m_fRight , m_fBottom);
				S = szTemp;
				return S;
			}
		};
	
		class GvrProfile
		{
		public:
			GvrProfile();
			virtual ~GvrProfile();
			CLASS_MEMBER(Screen ,m_,Screen);
			CLASS_MEMBER(Viewer, m_, Viewer);
			
			CLASS_INTERFACE(int , m_i , Width);
			CLASS_INTERFACE(int, m_i, Height);
			CLASS_INTERFACE(float*, m_fp, Vertices);
			CLASS_INTERFACE(float*, m_fp, TexUV);
			CLASS_INTERFACE(unsigned short*, m_p, Indices);

			bool InitFromDURL(const String szDURL){
				CDayDreamParameters DDP = CDayDreamParameters::FromDayDreamURL(szDURL);
				return	InitFromDayDreamParameters(DDP);
			}
			bool InitFromDayDreamParameters(const CDayDreamParameters &DDP);


			void ReAllocBuffers(const int iWidth , const int iHeight);
			void ReleaseBuffers();

			float VerticalLensOffset()const{ return  (m_Viewer.lenses.offset - m_Screen.border - m_Screen.height / 2) * m_Viewer.lenses.alignment; };
			EyeVisibleTanAngles GetLeftEyeVisibleTanAngles()const;
			EyeVisibleTanAngles GetLeftEyeNoLensTanAngles()const;
			Rectf GetLeftEyeVisibleScreenRect(EyeVisibleTanAngles undistortedFrustum) const;
			static float  GetMaxRadius(EyeVisibleTanAngles tanAngleRect);
			static GvrDistortion  ApproximateInverse(DoubleArray coef, float maxRadius = 1, int numSamples = 100) {
				GvrDistortion  D(coef);
				return ApproximateInverse(D, maxRadius, numSamples);
			}
			static GvrDistortion  ApproximateInverse(GvrDistortion  distort, float maxRadius = 1, int numSamples = 100);

			int ComputeDistortionBuffer(const int iWidth = 32, const int iHeight = 32, bool bDistortVertices = true, Mesh_820 * pMesh = NULL);
			
			void SaveDistortionBuffer(const char* pszFilePathName)const;
			// 为一体机使用的畸变文件
			void SaveDDDDFile(const char* pszFilePathName)const;
		};

	}
}

