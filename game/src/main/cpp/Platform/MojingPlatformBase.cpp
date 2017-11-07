#include <math.h>
#include "MojingPlatformBase.h"

#if defined(MJ_OS_MAC)
#include <unistd.h>
#include <sys/time.h>
#include "MojingIOSBase.h"
#elif defined(MJ_OS_WIN32)
#include  <shlobj.h>   
#pragma  comment(lib, "shell32.lib")  
#elif defined(MJ_OS_ANDROID)
#include "Android/MojingAndroidDeviceInfo.h"
#endif

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		MojingPlatformBase* MojingPlatformBase::g_pPlatform = 0;
		MojingPlatformBase::MojingPlatformBase()
		{
			SetMobileConfigVersion("0");
			SetGlassesConfigVersion("0");
			SetDefaultLocalProfilePath();
		}

		MojingPlatformBase::MojingPlatformBase(int nWidth, int nHeight, float xdpi, float ydpi, const char* Brand, const char* Model, const char* Serial)
		{
#ifdef AUTO_LANDSCAPE
			m_DisplayParam.SetScreenWidth(nWidth);
			m_DisplayParam.SetScreenHeight(nHeight);
#else
			m_DisplayParam.SetScreenWidth(fmax(nWidth , nHeight));
			m_DisplayParam.SetScreenHeight(fmin(nWidth , nHeight));
#endif
			m_DisplayParam.SetXdpi(xdpi);
			m_DisplayParam.SetYdpi(ydpi);
			m_DisplayParam.SetPPI(0);
			/*m_DisplayParam.SetScreenMeters();*/
			m_DeviceParam.SetBrand(Brand);
			m_DeviceParam.SetModel(Model);
			m_DeviceParam.SetSerial(Serial);
			/************************************************************************/
			/* 以下代码用于识别当前设备是否是一体机                                 */
			/************************************************************************/
			m_DeviceParam.CheckIsMachine();
           
           	ReadCPUName();
            
#ifdef MJ_OS_MAC
            SetOSString(GetMobileSystemVersion());
#elif defined(MJ_OS_ANDROID)
			char StrOS[16] = "";
			String OSType = "Android ";
			AndroidProperties::GetAndroidVersion(StrOS);
			if (*StrOS)
				SetOSString(OSType + StrOS);
			else
				SetOSString(OSType + "Unknown");
#elif defined(MJ_OS_WIN32)
			String OSType = "Windows ";
			OSVERSIONINFOEX osver;  
			osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			if (!GetVersionEx((LPOSVERSIONINFOA)&osver))
			{
				SetOSString(OSType + "Unknown");
			}
			else
			{
				char szVersion[256];
				sprintf(szVersion, "%d.%d Build %d Service Pack %d.%d", osver.dwMajorVersion, osver.dwMinorVersion,
					osver.dwBuildNumber, osver.wServicePackMajor, osver.wServicePackMinor);
				SetOSString(OSType + szVersion);
			}
#endif
			SetDefaultLocalProfilePath();
		}

		MojingPlatformBase::~MojingPlatformBase()
		{
			if (g_pPlatform)
			{
				delete g_pPlatform;
				g_pPlatform = NULL;
			}
		}

		unsigned char MojingPlatformBase::GetPlatformID()
		{
#ifdef MJ_OS_ANDROID
			return 1;
#elif defined(MJ_OS_IOS)
			return 2;
#elif defined(MJ_OS_WIN32)
			return 3;
#endif
		}

		void MojingPlatformBase::InitPlatform(int nWidth, int nHeight, float xdpi, float ydpi, const char* Brand, const char* Model, const char* Serial, const char* szMerchantID, const char* szAppID,
			const char* szAppKey, const char* szAppName, const char* szPackageName, const char* szUserID, const char* szChannelID, const char* PacketProfilePath)
		{
			if (g_pPlatform == NULL)
			{
				g_pPlatform = new MojingPlatformBase(nWidth, nHeight, xdpi, ydpi, Brand, Model, Serial);
				g_pPlatform->SetPacketProfilePath(PacketProfilePath ? PacketProfilePath : "");
				g_pPlatform->SetMerchantID(szMerchantID);
				g_pPlatform->SetAppID(szAppID);
				g_pPlatform->SetAppKey(szAppKey);
				g_pPlatform->SetAppName(szAppName);
				g_pPlatform->SetPackageName(szPackageName);
				g_pPlatform->SetUserID(szUserID);
				g_pPlatform->SetChannelID(szChannelID);
			}
		}

		MojingPlatformBase* MojingPlatformBase::GetPlatform()
		{
			return g_pPlatform;
		}

		void MojingPlatformBase::ReadCPUName()
		{
			SetCPUName("UNKNOWN");
#ifdef MJ_OS_ANDROID
			FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
			char line[128];
			char *p;
			const char *sep = ":";

			if (cpuinfo)
			{
				while (fgets(line, 128, cpuinfo))
				{
					if (0 == strncmp(line, "Hardware", 8))
					{

						p = strtok(line, sep);
						if (p)
						{
							p = strtok(NULL, sep);
							if (p)
							{
								char *it = NULL;
								while (*p == ' ' && *p) ++p; // Strip head spaces
								it = p;
								while (*p) p++; // to end
								p--;
								while (*p == ' ' || *p == '\n') p--; // strip tail spaces and newlines
								*(++p) = '\0';
								SetCPUName(it);
							}
						}
						break;
					}
				}
				fclose(cpuinfo);
			}
#endif
#ifdef MJ_OS_MAC
			SetCPUName(GetCPUArchName());
#endif
		}

		void MojingPlatformBase::SetDefaultLocalProfilePath()
		{
			char DefaultLocalProfilePath[260] = { 0 };
#ifdef MJ_OS_MAC
			const char* strCache = GetIOSAppPrivateDir(APP_DIR_CACHES);
			strcpy(DefaultLocalProfilePath, strCache);
			strcat(DefaultLocalProfilePath, "/MojingSDK");
			if(access(DefaultLocalProfilePath,0)==-1)
			{
				CreateIOSDir(DefaultLocalProfilePath);
			}
#elif defined(MJ_OS_WIN32)
			char szPath[MAX_PATH];
			ZeroMemory(szPath, MAX_PATH); 
			if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE))
			{
				strcpy(DefaultLocalProfilePath, szPath);
				strcat(DefaultLocalProfilePath, "\\MojingSDK");
			}
			else
			{
				strcpy(DefaultLocalProfilePath, "c:/MojingSDK");
			}
#else
			strcpy(DefaultLocalProfilePath, "/sdcard/MojingSDK");
#endif
			SetDefaultLocalProfilePath(DefaultLocalProfilePath);
		}

		char* MojingPlatformBase::GetDefaultLocalLogPath()
		{
			char *pDefaultLocalProfilePath = new char[260];
#if defined(MJ_OS_MAC) || defined(MJ_OS_IPHONE)
			const char* strCache = GetIOSAppPrivateDir(APP_DIR_CACHES);
			strcpy(pDefaultLocalProfilePath, strCache);
			strcat(pDefaultLocalProfilePath, "/MojingSDK");
			if(access(pDefaultLocalProfilePath,0)==-1)
			{
				CreateIOSDir(pDefaultLocalProfilePath);
			}
            char logPath[260];
            strcpy(logPath, pDefaultLocalProfilePath);
            strcat(logPath, "/log");
            if(access(logPath,0)==-1)
            {
                CreateIOSDir(logPath);
            }
#elif defined(MJ_OS_WIN32)
			char szPath[MAX_PATH];
			ZeroMemory(szPath, MAX_PATH);
			if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE))
			{
				strcpy(pDefaultLocalProfilePath, szPath);
				strcat(pDefaultLocalProfilePath, "\\MojingSDK");
			}
			else
			{
				strcpy(pDefaultLocalProfilePath, "c:/MojingSDK");
			}
#else
			strcpy(pDefaultLocalProfilePath, "/sdcard/MojingSDK");
#endif		
			return pDefaultLocalProfilePath;
		}
	};// end of namespace mojing
};// end of namespace baofeng
