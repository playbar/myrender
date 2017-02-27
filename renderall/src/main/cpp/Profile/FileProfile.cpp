#include "FileProfile.h"
#include "../Base/MojingTypes.h"

namespace Baofeng
{
	namespace Mojing
	{
		Mutex* FileProfile::g_mtAllFileIOMap = NULL;
		std::map<String, Thread*> FileProfile::g_AllFileIO;
		extern unsigned char g_EncKey[16];

		FileProfile::FileProfile(String sFileName, String sClassName, MojingJsonElement* pElement, bool bSave)
		{
			SetFilePathName(sFileName);
			if (bSave)
				SetJson(pElement->ToJson());
			else
				SetJson(NULL);
			SetSave(bSave);
			SetClassName(sClassName);
			
			SetElement(pElement);

			if (g_mtAllFileIOMap == NULL)
				g_mtAllFileIOMap = new Mutex;
		}

		FileProfile::~FileProfile()
		{
			if (m_pJson != NULL)
				m_pJson->Release();
			m_pJson = NULL;
		}

		int FileProfile::Run()
		{
			int iRet = 0;
			while (1)
			{
				g_mtAllFileIOMap->DoLock();
				std::map<String, Thread*>::iterator it = g_AllFileIO.find(GetFilePathName());
				if (it == g_AllFileIO.end() || it->second == NULL)
				{
					g_AllFileIO[GetFilePathName()] = this;
					g_mtAllFileIOMap->Unlock();
					break;
				}
				g_mtAllFileIOMap->Unlock();
				MSleep(50);
			}

			if (GetSave())
			{
				iRet = m_pJson->Save(GetFilePathName(), g_EncKey);
			}
			else
			{
				JSON *pJson = JSON::Load(GetFilePathName(), g_EncKey);
				if (pJson == NULL)
					pJson = JSON::Load(GetFilePathName());

				if (pJson)
				{
					SetJson(pJson);
					iRet = GetElement()->FromJson(pJson);
				}

			}

			g_mtAllFileIOMap->DoLock();
			g_AllFileIO[GetFilePathName()] = NULL;
			g_mtAllFileIOMap->Unlock();
			return iRet;
		}


	}
}