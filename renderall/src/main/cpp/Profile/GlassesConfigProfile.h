#pragma once
#include "../Base/MojingJSON.h"

namespace Baofeng
{
	namespace Mojing
	{

		class GlassesConfigProfile
		{
			static bool UpdateFromProfile_V1(JSON *pJsonRoot);
			static bool UpdateFromProfile(JSON *pJsonRoot);
			static JSON * SelectJsonNode(const char * lpszGlassesName, const char * lpszProfilePath);
			static JSON * SelectJsonNode(const char *lpszGlassesName, JSON * pJson);
			static bool CheckJsonNode(JSON *, void *pParam);
		public:
			GlassesConfigProfile();
			virtual ~GlassesConfigProfile();
			static bool UpdateFromProfile(const char * lpszGlassesName, const char * lpszProfilePath, JSON *pUpdateJson = NULL);
		};

	}
}