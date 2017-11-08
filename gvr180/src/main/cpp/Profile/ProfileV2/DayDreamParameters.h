#pragma once
#include "../../Base/MojingTypes.h"
#include "../../Base/MojingString.h"
namespace Baofeng
{
	namespace Mojing
	{
		typedef enum __enumScreenVerticalAlignment
		{
			SVA_BOTTOM = 0,
			SVA_CENTER = 1,
			SVA_TOP = 2
		}ScreenVerticalAlignment;
		class CDayDreamParameters
		{
		private: 
			 float * m_pK;
		public:
			CDayDreamParameters();
			virtual ~CDayDreamParameters();

			static int UpdateDayDreamURL(const char* szDayDreamURL, char * szNewDayDreamURL, float fPPI_Scale);

			static CDayDreamParameters FromDayDreamURL(const char* szDayDreamURL);
			String GetDayDreamURL();
	
			bool IsNoDistortion()const;
			CLASS_MEMBER_STR(String, m_sz, CompanyName);// 公司名
			CLASS_MEMBER_STR(String, m_sz, ViewerName);// 眼镜名
			// Primary button type??
			CLASS_MEMBER(float, m_sz, ScreenToLens);// 镜片到手机的距离 , 米 
			CLASS_MEMBER(float, m_sz, InterLens);// 瞳距 , 米 
			// Field-of-view angles
			CLASS_MEMBER(float, m_f, OuterFOV);
			CLASS_MEMBER(float, m_f, InnerFOV);
			CLASS_MEMBER(float, m_f, TopFOV);
			CLASS_MEMBER(float, m_f, BottomFOV);
			// Screen vertical alignment
			CLASS_MEMBER(ScreenVerticalAlignment, m_, ScreenVerticalAlignmentType);
			CLASS_MEMBER(float, m_f, ScreenVerticalAlignment);//Tray to lens-center distance (mm)
			// Distortion coefficients 
			CLASS_MEMBER(int, m_i, NumK);
			
			float& operator[](int i) const
			{
				return m_pK[i];
			}
		};


	}
}
