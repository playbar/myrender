#pragma once

#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif

#include "MojingJsonElement.h"

namespace Baofeng
{
	namespace Mojing
	{
		class MojingDisplayParameters :public MojingJsonElement
		{
			
		public:
			MojingDisplayParameters();
			virtual ~MojingDisplayParameters();

			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);
			bool UpdatePPIFromUserSetting();
			CLASS_MEMBER_J(int, m_i, ScreenWidth);
			CLASS_MEMBER_J(int , m_i, ScreenHeight);
			// CLASS_MEMBER_J(float,m_f, Density);      // 屏幕密度（像素比例：0.75/1.0/1.5/2.0）  
			// CLASS_MEMBER_J(int , m_i, DensityDPI);     // 屏幕密度（每寸像素：120/160/240/320） 
			// CLASS_MEMBER_J(float,m_f, ScaledDensity);
			CLASS_MEMBER_J(float,m_f, Xdpi);
			CLASS_MEMBER_J(float,m_f, Ydpi);
			CLASS_MEMBER_J(float,m_f, PPI);			// 实际PPI修正值，此值默认为小于1的数表示使用dpi计算
			CLASS_MEMBER_J(float, m_f, UserPPI);			// 实际PPI修正值，此值默认为小于1的数表示使用dpi计算
            CLASS_MEMBER_J(float,m_f, Scale);
			CLASS_MEMBER_J(float, m_f, AvgRenderTime);
			CLASS_MEMBER_J(float, m_f, MobileEdge);// 手机边框，默认为0.004
			float GetScreenWidthMeter()const;
			float GetScreenHeightMeter()const;
			
			float Pix2Meter(float fPix)const;
			float Meter2Pix(float fMeter)const;

			float GetScreenPPI();
			// void SetScreenMeters(void);
		};

	};// end of namespace mojing
};// end of namespace baofeng