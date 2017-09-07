#pragma once
#include "../../Parameters/MojingJsonElement.h"
#include <map>
#include <vector>
#include <algorithm>
using namespace std;
namespace Baofeng
{
	namespace Mojing
	{
		typedef map<unsigned short, String> DisplayInfo;
		unsigned short GetLanguageCode(const char * szLanguageName);
		bool ParseIDList(vector<unsigned int> *pIDList, JSON *pIDListNode);
		bool ParseLanguageDisplay(DisplayInfo*pDisplayMap, JSON *pDisplayNode);
		
#define VECTOR_FIND(V , VALUE) find((V).begin() , (V).end() , (VALUE))
		class GlassInfo :
			public MojingJsonElement
		{
			// 注意：对于DayDreamURL来说，需要根据PPI修正值重新调整各项尺寸。
			void UpdateDURL();
		public:
			GlassInfo();
			virtual ~GlassInfo();

			virtual JSON* ToJson();
			JSON* ToJson(unsigned short wLanguageCode);
			virtual bool FromJson(JSON*);

			CLASS_MEMBER_J(int, m_i, Segments);
			CLASS_MEMBER_J(float, m_f, MetersPerTanAngleAtCenter);
			CLASS_INTERFACE_J(float, m_f, FOV);
			CLASS_INTERFACE_J(int, m_i, NoDispersion);
			CLASS_INTERFACE_J(unsigned int, m_ui, ID);
			CLASS_INTERFACE_STR_J(String, m_sz, GlassesName);
			CLASS_INTERFACE_STR_J(String, m_sz, URL);
			CLASS_INTERFACE_STR_J(String, m_sz, DURL);
			CLASS_INTERFACE_J(float, m_f, DDScale);
			CLASS_INTERFACE_STR_J(String, m_sz, DURL_Original);
			CLASS_INTERFACE(bool, m_b, ReCalculationKT);
			
			DisplayInfo m_Display;
			float m_fR[20];
			float m_fG[20];
			float m_fB[20];
			float m_fL[20];
			// For Unreal
			float m_fRT[20];
			float m_fGT[20];
			float m_fBT[20];
		};

	}
}