#pragma once
#include "../Base/MojingJSON.h"

namespace Baofeng
{
	namespace Mojing
	{
		class MobileConfigProfile
		{
			static bool UpdateFromProfile_V1(JSON *pJsonRoot);
			static bool UpdateFromProfile(JSON *pJsonRoot);
			static JSON* SelectJsonNode(const char *lpszMobileName, const char* filename);
			static JSON* SelectJsonNode(const char *lpszMobileName, JSON * pJson);
			static bool CheckJsonNode(JSON *pNode, void *pParam);
			static bool SaveToSDCardProfile(JSON *pJsonNode, String strFileName);
			static void ReplaceJsonNode(JSON *pSrc, JSON *pTarget, const char* szNodeName);
		public:
			MobileConfigProfile();
			virtual ~MobileConfigProfile();
			static bool UpdateFromProfile(const char * lpszProfilePath, JSON *pUpdateJson = NULL);
			CLASS_MEMBER_STR(String , m_str , PacketProfilePath);
			static String GetEliminateBuiltin();
		};

	}
}