#pragma once
#include "../Base/MojingList.h"
#include "../Base/MojingThreads.h"
#include "../Base/MojingString.h"
#include "../Base/MojingJson.h"
#include "../Parameters/MojingJsonElement.h"
#include <map>
namespace Baofeng
{
	namespace Mojing
	{
		class FileProfile :public Thread
		{
			static Mutex* g_mtAllFileIOMap;
			static std::map<String, Thread*> g_AllFileIO;
		public:
			FileProfile(String sFileName, String sClassName, MojingJsonElement* pElement, bool bSave);
			virtual ~FileProfile();
			CLASS_INTERFACE_STR(String, m_sz, FilePathName);
			CLASS_INTERFACE_STR(String, m_sz, ClassName);
			CLASS_INTERFACE(JSON *,m_p,Json);
			CLASS_INTERFACE(bool, m_b, Save);
			CLASS_INTERFACE(MojingJsonElement*, m_p, Element);
			virtual int Run();

		};
	}
}