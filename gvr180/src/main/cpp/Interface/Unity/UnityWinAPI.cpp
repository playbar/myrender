//
//  MojingWinAPI.cpp
//  MojingSDK
//
//  Created by 汤申亮 on 16/1/25.
//  Copyright (c) 2016年 baofeng. All rights reserved.
//

#include <unistd.h>						// usleep, etc
#include "../../Base/MojingTypes.h"
#include "../../Base/MojingMath.h"
#include "../../MojingAPI.h"
#include "../../MojingManager.h"
#include "../../MojingSDKStatus.h"
#include "../../Platform/MojingPlatformBase.h"
#include "../../Parameters/MojingParameters.h"
#include "../../Render/MojingRenderBase.h"

#include "UnityWinAPI.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

/*+++++ most interface define in UnityPlugin.cpp +++++*/
//#include "UnityPlugin.cpp"
/*+++++++++++++++++++++++++++++++++++++++++++++++*/

extern "C"
{   
	static const char* GetMobileBrand()
	{
		return "Lenevo";
	}

	static const char* GetMobileInnerModelName()
	{
		return "Lenevo X200";
	}

	static const char* GetMobileSerialNumber()
	{
		return "0000001";
	}

	static const char* GetCurAppVersion()
	{
		return "UNKNOWN";
	}

	static const char* GetCurPackageName()
	{
		return "UNKNOWN";
	}

	static const char* GetUserID()
	{
		return "USERID00000001";
	}

	static const char* GetUUID()
	{
		return "000000000-000-000-00000000000";
	}

	static string GetSDKAssetsDirPath()
	{
	    char szPath[MAX_PATH];
	    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	    {
	    	MOJING_ERROR(g_APIlogger,"GetModuleFileName failed. errorcode = " << GetLastError());
			return "c:\\MojingSDK\\Assets";
	    }
	    MOJING_ERROR(g_APIlogger,"GetModuleFileName succeed. path = " << szPath);
	    char* p = strrchr(szPath, '\\');
	    strcpy(p+1, "Assets\\StreamingAssets\\MojingSDK");
	    MOJING_ERROR(g_APIlogger,"Assets path = " << szPath);

		return string(szPath);
	}

	static bool GetMobileScreenSize(int &nWidth, int &nHeight, float &xdpi, float &ydpi, float scale)
	{
		nWidth = 1920;
		nHeight = 1080;
		xdpi = 320;
		ydpi = 320;

		return true;
	}

	UNITY_EXPORT bool Unity_Init(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID, const char* szProfilePath)
	{    
		MOJING_FUNC_TRACE(g_APIlogger);
		MOJING_TRACE(g_APIlogger, "Init under Windows...");
		bool bRet = false;
    
		char* Brand = (char *)GetMobileBrand();
		MOJING_TRACE(g_APIlogger, "Brand: " << Brand);
		char* Model = (char *)GetMobileInnerModelName();
		MOJING_TRACE(g_APIlogger, "Model: " << Model);
		char* Serial = (char *)GetMobileSerialNumber();
		MOJING_TRACE(g_APIlogger, "Serial: " << Serial);
    
		int nWidth = 0, nHeight = 0;
		float xdpi = 0, ydpi = 0;
		float scale = 1;
		bRet = GetMobileScreenSize(nWidth, nHeight, xdpi, ydpi, scale);
		if(!bRet)
		{
			return  false;
		}
		MOJING_TRACE(g_APIlogger, "Screen Size: " << nWidth << " X " << nHeight);
		MOJING_TRACE(g_APIlogger, "Screen DPI:  " << xdpi << " X " << ydpi);
    
		const char * szAppName = GetCurAppVersion();
		MOJING_TRACE(g_APIlogger, "AppName: " << szAppName);
		const char* szPackageName = GetCurPackageName();
		MOJING_TRACE(g_APIlogger, "PackageName: " << szPackageName);
    
		const char * szUserID = GetUserID();//GetVerdorUUID();
		MOJING_TRACE(g_APIlogger, "UserID: " << szUserID);
    
	#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "MerchantID: " << szMerchantID << ", AppID: " << szAppID << ", AppKey: " << szAppKey << ", ChannelID: " << szChannelID);
	#endif
    
		//string path = GetSDKAssetsDirPath();
		//const char* szProfilePath = path.c_str();//GetSDKAssetsFilePath("GlassesConfig.json");
		if(szProfilePath != NULL)
			MOJING_TRACE(g_APIlogger, "ProfilePath: " << szProfilePath);
    
		bRet = MojingSDK_Init(nWidth, nHeight, xdpi, ydpi, Brand, Model, Serial, szMerchantID, szAppID, szAppKey, szAppName, szPackageName, szUserID, szChannelID, szProfilePath);
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			MojingDisplayParameters *pDisplay = pManager->GetParameters()->GetDisplayParameters();
			if (pDisplay)
				pDisplay->SetScale(scale);
		}
        
		return bRet;
	}
    
#if 0
	UNITY_EXPORT bool Unity_OnSurfaceChanged(int newWidth, int newHeight)
	{
	    //MOJING_FUNC_TRACE(g_APIlogger);
	    //float x = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->GetXdpi();
	    //float y = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->GetYdpi();
	    //MOJING_TRACE(g_APIlogger, "Unity_OnSurfaceChanged old dpi x = " << x << ", y= " << y);
	    //MOJING_TRACE(g_APIlogger, "Unity_OnSurfaceChanged old dpi newWidth = " << newWidth << ", newHeight= " << newHeight);

	    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetScreenWidth(fmax(newWidth, newHeight));
	    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetScreenHeight(fmin(newWidth, newHeight));

	    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetXdpi(fmax(newWidth, newHeight)*336/1920);
	    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetYdpi(fmin(newWidth, newHeight)*336/1080);

	    MojingRenderBase::SetModify();
	    return true	;
	}
#endif


	UNITY_EXPORT void UnitySetGraphicsDevice (void* device, int deviceType, int eventType)
	{
		MOJING_TRACE(g_APIlogger, "UnitySetGraphicsDevice deviceType : " << deviceType << " eventType : " << eventType);

		if( ( deviceType == 0 || deviceType == 17 ) && eventType == 0 )
		{
			GLenum eRet = glewInit();
			MOJING_TRACE(g_APIlogger, "glewInit finished. ret = " << eRet);
		}
	}

	UNITY_EXPORT void Unity_AppResume()
	{
		MojingSDK_AppResume(GetUUID());
	}

	UNITY_EXPORT void Unity_AppPause()
	{
		MojingSDK_AppPause();
	}
}	// extern "C"     
