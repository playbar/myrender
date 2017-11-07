#include <dirent.h>
#include <unistd.h>
#include "MojingAPI.h"
#include "Base/MojingTypes.h"
#include "MojingManager.h"
#include "MojingSDKStatus.h"

#include "Tracker/MojingTracker.h"
#include "Base/MojingSystem.h"
#include "Platform/MojingPlatformBase.h"
#include "Render/MojingRenderBase.h"
#ifdef MJ_OS_IOS
#include "Render/Metal/MojingRenderMetal.h"
#include "Interface/ios/MojingIOSBase.h"
#include "Tracker/MojingControllerSocket.h"
#endif
#include "Profile/GlassesConfigProfile.h"
#include "Profile/MobileConfigProfile.h"
#include "Profile/MojingProfileKey.h"
#include "Reporter/GyroTempCalibrationReporter.h"
#include "Reporter/CrashReporter.h"
#include "Reporter/MobileConfigUpdate.h"
#include "Reporter/DatabaseInfoReporter.h"
#include "Reporter/MojingMerchantVerify.h"
#include "Distortion/MojingDistortion.h"

#ifdef MJ_OS_ANDROID
#include "3rdPart/ktx/include/ktx.h"
#endif

#ifdef MJ_OS_ANDROID
#include "Hook/HookGVR/HookGVRTools.h"
#include "Tracker/AndroidInternalSensorChecker.h"
//#include "Tracker/MojingControlPose.h"
#include "Tracker/MojingControllerSocket.h"
#include "Reporter/UserActionReporter.h"
#endif

#include "3rdPart/MD5/MD5.h"
#include "3rdPart/AES/AES.h"

#include "Profile/ProfileThreadMGR.h"

#if defined(MJ_OS_WIN32)
#include "3rdPart/Curl/include/windows/curl.h"
#elif defined(MJ_OS_IOS)
//#include "3rdPart/Curl/include/ios/curl.h"
#else
#include "3rdPart/Curl/include/android/curl.h"
#endif

using namespace Baofeng;
using namespace Baofeng::Mojing;
#ifdef ENABLE_LOGGER
MojingLogger g_APIlogger("MojingSDK_API");
#endif

#ifndef DO_NOT_USING_TEXTURE_BACKER
#include "TextureBacker/TextBaker.h"
TextureBacker* g_pTexBacker = NULL;
#endif
#define USE_MOJING_RENDER_EFFECTS 0

#ifdef MJ_OS_ANDROID
extern char *g_pPacketName;
#endif

#ifdef _DEBUG
#include "Profile/ProfileV2/DayDreamParameters.h"
#include "Distortion/GVR/GvrProfile.h"
#endif


#ifdef USING_MINIDUMP
USING_MINIDUMP;
bool MojingSDK_MinidumpCallBack(google_breakpad::MinidumpDescriptor const&Descriptor, void*Context, bool bSucceed)
{
	bool bRet = false;
	if (bSucceed)
	{// 成功写了Dump文件
		g_MojingMinidump.tmNow = time(NULL);
		const char * szFilePathName = Descriptor.path();
		CrashReporter *pReporter = CrashReporter::GetCrashReporter(szFilePathName);
		// 传入参数true表示 等待传输完成
		bRet = pReporter->DoReport(true);
	}
	return bRet;
}
#endif



// 1. Init/shutdown.
bool mj_Initialize()
{
	// We must set up the system for the plugin to work
	if (!System::IsInitialized())
	{
#ifdef USING_MINIDUMP
		memset(&g_MojingMinidump, 0, sizeof(g_MojingMinidump));
#endif
		System::Init(Log::ConfigureDefaultLog(LogMask_All));

#ifdef MJ_OS_MAC
//		curl_global_init(CURL_GLOBAL_NOTHING);
#else
		curl_global_init(CURL_GLOBAL_ALL);
#endif
	}
	
	return true;
}

void MJ_Shutdown()
{
#ifdef MJ_OS_MAC
//    curl_global_cleanup();
#else
    curl_global_cleanup();
#endif
	// We should clean up the system to be complete
	System::Destroy();
}

#ifdef MJ_OS_ANDROID
static void CheckLibSqlite()
{
	bool bSqliteExist = false;
	bool bSqlite3Exist = false;
	bool bCurlExist = false;

	if (access("/system/lib/libsqlite.so", F_OK) == 0)
	{
		bSqliteExist = true;
	}
	if (access("/system/lib/libsqlite3.so", F_OK) == 0)
	{
		bSqlite3Exist = true;
	}
	if (access("/system/lib/libcurl.so", F_OK) == 0)
	{
		bCurlExist = true;
	}
	JSON* joMsg = JSON::CreateObject();
	joMsg->AddStringItem("mobile", GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->GetMobile());
	joMsg->AddStringItem("os", GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->GetOS());
	joMsg->AddBoolItem("libcurl", bCurlExist);
	joMsg->AddBoolItem("libsqlite", bSqliteExist);
	joMsg->AddBoolItem("libsqlite3", bSqlite3Exist);
	MojingSDK_ReportLog(2000, "CheckLibSqlite", joMsg->PrintValue(0, false), true);
}
#endif

#if defined(MJ_OS_ANDROID)
eglSetup_t egl;
#endif

bool MojingSDK_Init(int nWidth, int nHeight, float xdpi, float ydpi, const char* Brand, const char* Model, const char* Serial, const char* szMerchantID, const char* szAppID,
	const char* szAppKey, const char* szAppName, const char* szPackageName, const char* szUserID, const char* szChannelID, const char* ProfilePath)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	bool bRet = false;
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	

	MOJING_TRACE(g_APIlogger, "Init Mojing SDK , AppName =  " << szAppName << " , SDK Version = " << pStatus->GetSDKVersion() << " , Build = " << pStatus->GetSDKBuildNumber());

#ifdef MJ_OS_ANDROID
	MOJING_TRACE(g_APIlogger, "Packet Name = " << szPackageName << "(OnLoad = " << (g_pPacketName == NULL ? "NULL" : g_pPacketName) << ")");
#endif
    
	if (pStatus->GetInitStatus() == INIT_NOW || pStatus->GetInitStatus() == INIT_DONE)
	{
		MOJING_ERROR(g_APIlogger, "Start SDK again!");
		return pStatus->IsMojingSDKEnbaled();
	}

	pStatus->SetInitStatus(INIT_NOW);
	MojingPlatformBase::InitPlatform(nWidth, nHeight, xdpi, ydpi, Brand, Model, Serial, szMerchantID, szAppID,
		szAppKey, szAppName, szPackageName, szUserID, szChannelID, ProfilePath);


	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		pStatus->SetInitStatus(INIT_DONE);
		pStatus->SetApp(szAppName);

//#ifdef MJ_OS_ANDROID
//		// 注意： 因为下面的代码会开辟很多的线程，有可能会导致下HOOK的时候卡死
//		HookGVRTools::Init();
//#endif // MJ_OS_ANDROID

		//if (strcmp(szAppID, "UNKNOWN") != 0)
		{
#ifdef MJ_OS_ANDROID
			MojingMerchantVerify::GetMojingMerchantVerify()->AppVerify(szMerchantID, szAppID, szAppKey, szPackageName);
#endif
#ifdef MJ_OS_MAC
			MojingMerchantVerify::GetMojingMerchantVerify()->AppVerify(szMerchantID, szAppID, szAppKey, szChannelID);
#endif
		}

		if (ProfilePath)
		{// 从安装包自带的文件中更新
			MOJING_TRACE(g_APIlogger, "UpdateFromProfile");
			// MojingPlatformBase::GetPlatform()->SetPacketProfilePath(ProfilePath);
			MobileConfigProfile::UpdateFromProfile(MojingPlatformBase::GetPlatform()->GetPacketProfilePath());
		}
		else
		{

		}
		// 注意：读取本地配置文件必须在MobileConfigUpdate::GetMobileConfigUpdate()->UpdateConfig();之前，以保证优先使用本机配置文件
		pManager->GetParameters()->GetFactoryCalibrationParameters()->Load();
		pManager->GetParameters()->GetGyroOffsetReportSet()->Load();

		MOJING_TRACE(g_APIlogger, "Update config...");
#ifdef REPORT_ABNORMAL_SENSOR
		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->UpdateConfig(); 
#endif
		MobileConfigUpdate::GetMobileConfigUpdate()->UpdateConfig();

		MOJING_TRACE(g_APIlogger, "Set report param...");
		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetApp(szAppName);
		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetBrand(pManager->GetParameters()->GetDeviceParameters()->GetBrand());
		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetMobile(pManager->GetParameters()->GetDeviceParameters()->GetModel());
		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetSerial(pManager->GetParameters()->GetDeviceParameters()->GetSerial());
	}
	else
	{
		MOJING_ERROR(g_APIlogger , "Can not get manager object!");
	}
	switch (pStatus->GetInitStatus())
	{
	case INIT_DONE:
		MOJING_TRACE(g_APIlogger, "Start SDK SUCCEED!");
		bRet = true;
		break;
	default:
		MOJING_ERROR(g_APIlogger, "Start SDK FAILD! Code = " << pStatus->GetInitStatus());
		bRet = false;
		break;
	}

#ifdef	USING_MINIDUMP
	MojingSDK_CheckCrashReport();
#endif

#ifdef MJ_OS_ANDROID
	//CheckLibSqlite();
#endif

#include <stdio.h>  
	
#ifdef _DEBUG
	const char* MJ4 = "CgzmmrTpo47prZTplZwSDeaatOmjjumtlOmVnDQdSnsDPSW28309KhAAAEBCAABAQgAAQEIAAEBCWAA1UI0XPToIKVyPPXsULj5QAGAA";
	CDayDreamParameters DDP = CDayDreamParameters::FromDayDreamURL(MJ4);

	GvrProfile GP;
	Screen S;
	Viewer V;
	V.lenses.separation = 0.060f;
	V.lenses.offset = 0.035f;
	V.lenses.screenDistance = 0.042f;
	V.lenses.alignment = V.lenses.AlignBottom;
	V.maxFOV.outer = 50.0f;
	V.maxFOV.inner = 50.0f;
	V.maxFOV.upper = 50.0f;
	V.maxFOV.lower = 50.0f;
	DoubleArray temparray;
	//= { 0.441f, 0.156f };
	temparray.push_back(0.3f);
	temparray.push_back(0.0f);
	V.SetCoef(temparray); 
	S.width = 0.114f;
	S.height = 0.0635f;
	S.border = 0.0035f;

	int size = V.inverse.GetCoef().size();
	double cof0 = V.inverse.GetCoef()[0];
	double cof1 = V.inverse.GetCoef()[1];
	double cof2 = V.inverse.GetCoef()[2];
	double cof3 = V.inverse.GetCoef()[3];
	double cof4 = V.inverse.GetCoef()[4];
	double cof5 = V.inverse.GetCoef()[5];

	GP.SetScreen(S);
	GP.SetViewer(V);
	
	
 	MojingProfileKey Key;
// 	float X, Y, Z;
// 	Quatf F(0.1,0.2,0.3,0.4);
// 	F.Normalize();
// 	Matrix4f m4(F);
// 	
// 	F.GetEulerAngles(&X,&Y,&Z);
	// Key.SetString("SWAFHF-AZZG4F-CBEYD9-9323ZT-XBXX9T-CGCTAC");
	// 1-10-18
	Key.SetString("2Q2XWR-9YWQSQ-WUHFH7-WWE2WW-8RW84W-XT2FEX");
	MOJING_TRACE(g_APIlogger , "2Q2XWR-9YWQSQ-WUHFH7-WWE2WW-8RW84W-XT2FEX >>>>> MID = " << Key.GetManufacturerID() <<
		"PID = " << Key.GetProductID() <<
		"GID = " << Key.GetGlassID());
	Key.SetString("82C2X-XZ86FM-XCCHSZ-4CADDX-CBADDB-YUYZZ7");
	MOJING_TRACE(g_APIlogger, "82C2X-XZ86FM-XCCHSZ-4CADDX-CBADDB-YUYZZ7 >>>>> MID = " << Key.GetManufacturerID() <<
		"PID = " << Key.GetProductID() <<
		"GID = " << Key.GetGlassID());

	String sfo3i = Key.GetGlassKeyIDString("82C2X-XZ86FM-XCCHSZ-4CADDX-CBADDB-YUYZZ7");
// 	HX944-HMHKZN-9HF58G-4CYU9W-SW4B4X-CNACFN
// BS9X3-8MAQEX-HMFVHG-DTQQXV-9Y2B8V-2GHK9H
	Key.SetString("HX944-HMHKZN-9HF58G-4CYU9W-SW4B4X-CNACFN");
	MOJING_TRACE(g_APIlogger, "EGWUSS-HR86YL-QHC8C8-EB2YCH-AHDDXH-85A4Y5 >>>>> MID = " << Key.GetManufacturerID() <<
		"PID = " << Key.GetProductID() <<
		"GID = " << Key.GetGlassID());
	
	Key.SetString("STYNEX-YTA22N-FGZNZB-XGXQ4S-YVZGZG-2T23DT");
	MOJING_TRACE(g_APIlogger, "STYNEX-YTA22N-FGZNZB-XGXQ4S-YVZGZG-2T23DT >>>>> MID = " << Key.GetManufacturerID() <<
		"PID = " << Key.GetProductID() <<
		"GID = " << Key.GetGlassID());
#define  PRINT_KEY( MID , PID , GID) do \
	{\
	MOJING_TRACE(g_APIlogger, "/* " << MID << " , " << PID << " , " << GID << " */ \"" << MojingProfileKey::GetStringWithID(MID, PID, GID) << "\""); \
	} while (0)

	MOJING_TRACE(g_APIlogger , "/*****************PRINT KEYS**********************/");
	PRINT_KEY(1, 100, 100);
	PRINT_KEY(1, 10, 18);
	PRINT_KEY(1, 14, 22);
	char szTTT[64];
	strcpy( szTTT , MojingProfileKey::GetStringWithID(1, 14, 22));
	strcpy(szTTT, MojingProfileKey::GetStringWithID(1, 10, 18));
	// PRINT_KEY(235, 235, 235);
	// From = (-0.074446 , 0.143870 ,0.259640 , 0.952022)  , 
	Quatf qFrom (-0.074446 , 0.143870 ,0.259640 , 0.952022);
	// To = (-0.090886 , 0.143829 ,0.262451 , 0.949828) ,
	Quatf qTo (-0.090886 , 0.143829 ,0.262451 , 0.949828);
	//  Fix = (-0.015399 , -0.003783 ,-0.005614 , 0.999858)
	Quatf qFix = Tracker::CalculateTimeWarpMatrix(qFrom, qTo);
// 	PRINT_KEY(201, 201, 201);
// 	PRINT_KEY(202, 202, 202);
// 	PRINT_KEY(203, 203, 203);
// 	PRINT_KEY(204, 204, 204);
// 	PRINT_KEY(205, 205, 205);
// 	PRINT_KEY(211, 211, 211);
// 	PRINT_KEY(212, 212, 212);
// 	PRINT_KEY(213, 213, 213);
// 	PRINT_KEY(214, 214, 214);
// 	PRINT_KEY(215, 215, 215);
// 	PRINT_KEY(209, 209, 209);
// 	PRINT_KEY(208, 208, 208);
// 	PRINT_KEY(207, 207, 207);
// 	PRINT_KEY(204, 2040, 2040);
// 	PRINT_KEY(201, 2010, 2010);
// 	PRINT_KEY(207, 2070, 2070);
// 	PRINT_KEY(206, 206, 206);
// 	PRINT_KEY(206, 2060, 2060);
// 	PRINT_KEY(216, 216, 216);
// 	PRINT_KEY(217, 217, 217);


	Matrix4f m4_1(-0.9622019, 0.050022796, 0.26770386, 0.0, -0.050241955, -0.99871886, 0.006035808, 0.0, 0.2676628, -0.0076422975, 0.9634824, 0.0, 0.0, 0.0, 0.0, 1.0);
	Matrix4f m4_2(-0.998472, 0.048034094, -0.027343038, 0.0, -0.04802069, -0.9988463, -0.0011470434, 0.0, -0.027366575, 1.677408E-4, 0.99962604, 0.0, 0.0, 0.0, 0.0, 1.0);
	Matrix4f m4_3(-0.9972236, 0.06603057, 0.03442409, 0.0, 0.06536844, 0.9976604, -0.020018741, 0.0, -0.0356654, -0.017712913, -0.9992068, 0.0, 0.0, 0.0, 0.0, 1.0);
	Matrix4f m4_4(0.81121016, 0.21428956, 0.54407656, 0.0, 0.16280709, 0.81087744, -0.5621145, 0.0, -0.5616344, 0.5445721, 0.6229038, 0.0, 0.0, 0.0, 0.0, 1.0);
	Matrix4f m4_5(-0.7540048, -0.3833938, 0.5333708, 0.0, 0.036665857, -0.83529, -0.5485842, 0.0, 0.65584356, -0.39407894, 0.6438706, 0.0, 0.0, 0.0, 0.0, 1.0);
	Matrix4f m4_6(0.964424, 0.048408896, 0.25990197, 0.0, 0.02341491, 0.96359146, -0.2663631, 0.0, -0.26333287, 0.26297173, 0.9281744, 0.0, 0.0, 0.0, 0.0, 1.0);
	Matrix4f m4_7(0.8367598, 0.29626533, 0.46049803, 0.0, -0.06541535, 0.8890492, -0.45311263, 0.0, -0.54364735, 0.349023, 0.7633015, 0.0, 0.0, 0.0, 0.0, 1.0);
	Matrix4f m4_8(0.8002299, 0.19707704, -0.5663869, 0.0, 0.05527658, 0.9161991, 0.3968944, 0.0, 0.59714144, -0.34891444, 0.72227573, 0.0, 0.0, 0.0, 0.0, 1.0);
	float fpTopLeft[] = { -3.800403, 3.800403, -4.0, -2.6002758, 2.6002758, -4.0, -1.4001484, 1.4001484, -4.0, -0.2000212, 3.800403, -4.0, -0.2000212, 2.6002758, -4.0, -0.2000212, 1.4001484, -4.0, -3.800403, 0.2000212, -4.0, -2.6002758, 0.2000212, -4.0, -1.4001484, 0.2000212, -4.0, -0.2000212, 0.2000212, -4.0 };
	float fpBottomRight[] = { -3.4003603, 3.4003603, -4.0, -2.2002332, 2.2002332, -4.0, -1.000106, 1.000106, -4.0, 0.2000212, 3.4003603, -4.0, 0.2000212, 2.2002332, -4.0, 0.2000212, 1.000106, -4.0, -3.4003603, -0.2000212, -4.0, -2.2002332, -0.2000212, -4.0, -1.000106, -0.2000212, -4.0, 0.2000212, -0.2000212, -4.0 };

	Matrix4f m4[] = {
		Matrix4f::Identity(),
		m4_1,
		m4_2,
		m4_3,
		m4_4,
		m4_5,
		m4_6,
		m4_7,
		m4_8	
	};
	double dBegin = Timer::GetSeconds();
	for (int i = 0; i < 2000; i++)
	{
		m4[0] *= m4[i % 8 + 1];
	}
	double dEnd = Timer::GetSeconds();
	char szTime[64];
	strcpy(szTime , Timer::FormatDoubleTimeInMS(dEnd - dBegin));
	MOJING_TRACE(g_APIlogger, "2000 MxM = " << szTime);
	Vector2f v2TL(-4.800509, 4.800509);
	Vector2f v2BR(4.800509, -4.800509);
	Vector2f v2Check1 = MojingSDK_Math_DirectionalRadiaInRect(m4_1, v2TL, v2BR, -4.0f);
	Vector2f v2Check2 = MojingSDK_Math_DirectionalRadiaInRect(m4_2, v2TL, v2BR, -4.0f);
	Vector2f v2Check3 = MojingSDK_Math_DirectionalRadiaInRect(m4_3, v2TL, v2BR, -4.0f);
	Vector2f v2Check4 = MojingSDK_Math_DirectionalRadiaInRect(m4_4, v2TL, v2BR, -4.0f);
	Vector2f v2Check5 = MojingSDK_Math_DirectionalRadiaInRect(m4_5, v2TL, v2BR, -4.0f);
	Vector2f v2Check6 = MojingSDK_Math_DirectionalRadiaInRect(m4_6, v2TL, v2BR, -4.0f);
	Vector2f v2Check7 = MojingSDK_Math_DirectionalRadiaInRect(m4_7, v2TL, v2BR, -4.0f);
	Vector2f v2Check8 = MojingSDK_Math_DirectionalRadiaInRect(m4_7, v2TL, v2BR, -4.0f);
	
	Matrix4f m4_A(-0.097965926, 0.3924748, -0.91453195, 0.0, 0.1699879, 0.91204673, 0.3731989, 0.0, 0.98056567, -0.11889844, -0.15606529, 0.0, -0.02949216, 0.0054967697, 4.4703483E-10, 1.0);
	Vector2f v2TL2(8.161278, -6.4965224);
	Vector2f v2BR2(9.473417, -0.52405554);
	Vector2f V2Check = MojingSDK_Math_DirectionalRadiaInRect(m4_A, v2TL2, v2BR2, 1.403449);

	/* sample for report log */
	//MojingSDK_ReprotLog(ERROR_LOG_LEVEL, "MojingSDK", "MojingSDK init failed.", true);
	//MojingSDK_ReprotLog(ERROR_LOG_LEVEL, "MojingWorld", "{\"ERROR\":\"Get default MojingWorld failed.\"}", true);


	int iSelect = -1;
	int iSelect1 = MojingSDK_Math_SelectRectByDirectional(m4_1, 10, fpTopLeft, fpBottomRight);
	int iSelect2 = MojingSDK_Math_SelectRectByDirectional(m4_2, 10, fpTopLeft, fpBottomRight);
	int iSelect3 = MojingSDK_Math_SelectRectByDirectional(m4_3, 10, fpTopLeft, fpBottomRight);
	int iSelect4 = MojingSDK_Math_SelectRectByDirectional(m4_4, 10, fpTopLeft, fpBottomRight);
	int iSelect5 = MojingSDK_Math_SelectRectByDirectional(m4_5, 10, fpTopLeft, fpBottomRight);
	int iSelect6 = MojingSDK_Math_SelectRectByDirectional(m4_6, 10, fpTopLeft, fpBottomRight);
	int iSelect7 = MojingSDK_Math_SelectRectByDirectional(m4_7, 10, fpTopLeft, fpBottomRight);
	int iSelect8 = MojingSDK_Math_SelectRectByDirectional(m4_8, 10, fpTopLeft, fpBottomRight);
#endif
	return bRet;
}

void MojingSDK_Validate(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID)
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	//Please init first!
	if (pStatus->GetInitStatus() == INIT_NO)
	{
		MOJING_WARN(g_APIlogger, "Please init Mojing SDK first.");
		return;
	}

	MojingPlatformBase *pPlatform = MojingPlatformBase::GetPlatform();
	if (pPlatform)
	{
		pPlatform->SetMerchantID(szMerchantID);
		pPlatform->SetAppID(szAppID);
		pPlatform->SetAppKey(szAppKey);
		pPlatform->SetChannelID(szChannelID);
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MOJING_TRACE(g_APIlogger, "Mojing SDK validate. MerchantID: " << szMerchantID << ", AppID: " << szAppID << ", AppKey: " << szAppKey << ", ChannelID: " << szChannelID);
#ifdef MJ_OS_ANDROID
		MojingMerchantVerify::GetMojingMerchantVerify()->AppVerify(szMerchantID, szAppID, szAppKey, pPlatform->GetPackageName());
#endif
#ifdef MJ_OS_MAC
		MojingMerchantVerify::GetMojingMerchantVerify()->AppVerify(szMerchantID, szAppID, szAppKey, szChannelID);
#endif
	}
}

void MojingSDK_CheckPackage(const char* szAppName, const char* szCaseCode)
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	//Please init first!
	if (pStatus->GetInitStatus() == INIT_NO)
	{
		MOJING_WARN(g_APIlogger, "Please init Mojing SDK first.");
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MOJING_TRACE(g_APIlogger, "Mojing SDK check package name. App Name: " << szAppName << ", szCaseCode: " << szCaseCode);
#ifdef MJ_OS_ANDROID
		MojingMerchantVerify::GetMojingMerchantVerify()->AppCheckPackage(szAppName, szCaseCode);
#endif
	}
}


void MojingSDK_SetGlassesSerialNumber(const char* lpszSN)
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus == NULL || !pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_SetGlassesSerialNumber before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}

	if (lpszSN)
	{
		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetMojingSN(lpszSN);
	}
}

bool MojingSDK_SetEngineVersion(const char* lpszEngine)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus == NULL || !pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_SetEngineVersion before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return false;
	}
	
	//MOJING_TRACE(g_APIlogger , "MojingSDK_SetEngineVersion - 1" << lpszEngine);

	if (lpszEngine && *lpszEngine)
	{
		//MOJING_TRACE(g_APIlogger, "MojingSDK_SetEngineVersion - 2" << lpszEngine);

		char *szTempEngine = new char[strlen(lpszEngine) + 1];
		char *szTempDest = szTempEngine;
		const char *szTempSrc = lpszEngine;
		// 复制引擎信息，并且转换为小写
		while (*szTempDest++ = (char)tolower(*szTempSrc++));
		//MOJING_TRACE(g_APIlogger, "MojingSDK_SetEngineVersion - 3" << lpszEngine);

		if (strstr(szTempEngine, "unreal"))
		{
			pStatus->SetEngineStatus(ENGINE_UNREAL);
		}
		else if (strstr(szTempEngine, "unity"))
		{
			pStatus->SetEngineStatus(ENGINE_UNITY);
		}
		else if (strstr(szTempEngine, "gear"))
		{
			pStatus->SetEngineStatus(ENGINE_GEAR);
		}
		else if (strstr(szTempEngine, "gvr"))
		{
			pStatus->SetEngineStatus(ENGINE_GVR);
		}
		else
		{
			pStatus->SetEngineStatus(ENGINE_NONE);
		}
		//MOJING_TRACE(g_APIlogger, "MojingSDK_SetEngineVersion - 4" << lpszEngine);
		if (strcmp(lpszEngine, "Unreal Engine Init"))
		{
			JSON* joMsg = JSON::CreateObject();
			MojingPlatformBase* pPlatform = MojingPlatformBase::GetPlatform();
			if (pPlatform)
			{
				joMsg->AddStringItem("os", pPlatform->GetOSString());
			}
			else
			{
				joMsg->AddStringItem("os", "UNKNOWN");
			}
			joMsg->AddStringItem("engine", lpszEngine);
			MojingSDK_ReportLog(2000, "ReportEngineInfo", joMsg->PrintValue(0, false), true);
		}
		//MOJING_TRACE(g_APIlogger, "MojingSDK_SetEngineVersion - 5" << lpszEngine);
		MOJING_TRACE(g_APIlogger, "Set Engine = " << lpszEngine);
		pStatus->SetEngine(lpszEngine);
		delete[] szTempEngine;
	}
	else
	{
		pStatus->SetEngineStatus(ENGINE_NONE);
		pStatus->SetEngine("");
		MOJING_TRACE(g_APIlogger, "Set Engine = NONE" );
		//MOJING_TRACE(g_APIlogger, "MojingSDK_SetEngineVersion - 0" << lpszEngine);
	}
	
	return true;
}
// 获取指定镜片的畸变框物理尺寸
float MojingSDK_GetDistortionR(const char* strGlassesKey)
{
	float fRet = -1;
	Baofeng::Mojing::Parameters* pParameters = Baofeng::Mojing::Manager::GetMojingManager()->GetParameters();
	GlassesConfigProfileV2* pGlassesConfig = pParameters->GetGlassesConfigProfile();
	MojingProfileKey Key;
	if (Key.SetString(strGlassesKey) && pGlassesConfig->CheckIsFinalKey(Key))
	{
		GlassInfo* pGlass = pGlassesConfig->GetGlass(Key.GetGlassID());
		if (pGlass)
			fRet = pGlass->GetMetersPerTanAngleAtCenter();
	}

	return fRet;
}

bool MojingSDK_ReportReInit()
{
	mj_Initialize();
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			return pReporter->ReportReInit();
		}
	}
	return false;
}

void MojingSDK_AppExit()
{
	mj_Initialize();
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			return pReporter->AppExit();
		}
	}
}


bool MojingSDK_AppResume(const char* szUniqueID)
{
	mj_Initialize();
	//MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus && (ENGINE_GVR & pStatus->GetEngineStatus()))
	{
		String sGlass = MojingSDK_GetMojingWorldKey(MOJING_WORLDKEY_DEFAULT);
		if (sGlass != "")
		{
			sGlass = MojingProfileKey::GetGlassKeyIDString(sGlass);

			if (sGlass != "UNKNOWN")
			{
				GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGlassesReportName(sGlass);
			}
			else
			{
			}
		}
		else
		{
		}
	}

	MojingRenderBase::SetModify();
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			return pReporter->AppResume(szUniqueID);
		}
	}
	return false;
}

void MojingSDK_AppPause()
{
	mj_Initialize();
	//MOJING_FUNC_TRACE(g_APIlogger);
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->AppPause();
		}
	}
}

void MojingSDK_AppPageStart(const char* szPageName)
{
	mj_Initialize();
	//MOJING_FUNC_TRACE(g_APIlogger);
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->AppPageStart(szPageName);
		}
	}
}

void MojingSDK_AppPageEnd(const char* szPageName)
{
	mj_Initialize();
	//MOJING_FUNC_TRACE(g_APIlogger);
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->AppPageEnd(szPageName);
		}
	}
}

void MojingSDK_AppSetEvent(const char* szEventName, const char* szEventChannelID,
							const char* szEventInName, float eInData, const char* szEventOutName, float eOutData)
{
	mj_Initialize();
	//MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_AppSetEvent before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->AppSetEvent(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData, false);
		}
	}
}

void MojingSDK_ReportLog(int iLogType, const char* szTypeName, const char* szLogContent, bool pd)
{
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_ReportLog before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
	//MOJING_FUNC_TRACE(g_APIlogger);
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->ReportLog(iLogType, szTypeName, szLogContent, pd);
		}
	}
}

#ifdef MJ_OS_ANDROID
void MojingSDK_ReportUserAction(const char* szActionType, const char* szItemID, const char* strJsonValue)
{
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_ReportUserAction before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
	//MOJING_FUNC_TRACE(g_APIlogger);
	UserActionReporter::GetUserActionReporter()->Post(szActionType, szItemID, strJsonValue);
}
#endif

void MojingSDK_AppSetReportImmediate(bool bReportImmediate)
{
	mj_Initialize();
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_AppSetReportImmediate before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->SetReportImmediate(bReportImmediate);
		}
	}
}

void MojingSDK_AppSetContinueInterval(int interval)
{
	mj_Initialize();
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_AppSetContinueInterval before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->SetContinueInterval(interval);
		}
	}
}

void MojingSDK_AppSetReportInterval(int interval)
{
	mj_Initialize();
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_AppSetReportInterval before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		DatabaseInfoReporter* pReporter = pManager->GetReporter();
		if (pReporter)
		{
			pReporter->SetReportInterval(interval);
		}
	}
}

bool MojingSDK_StartTracker(int nSampleFrequence, const char* szGlassName)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
#ifdef REPORT_ABNORMAL_SENSOR
	static bool g_bSendErrorReport = false;
#endif
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();

	if (MojingPlatformBase::GetIsMachine())
		nSampleFrequence = 200;// 一体机强制使用200hz陀螺仪

	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "StartTracker befor SDK init! InitStatus = " << pStatus->GetInitStatus());
		return false;
	}

	bool ret = false;
	if (nSampleFrequence < 50)
	{
		pStatus->SetTrackerStatus(TRACKER_STOP);
		MOJING_ERROR(g_APIlogger, "StartTracker Faild! SampleFrequence < 50");
		return false;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		if (pTracker)
		{
#ifdef MJ_OS_ANDROID
			//部分手机直接配置为Java层陀螺仪
			if (pManager->GetParameters()->GetDeviceParameters()->GetSensorDataFromJava())
			{
				pTracker->SetDataFromExternal(true);
			}
			else
			{
				//通过用户设置确定为Java层还是Native层陀螺仪
				UserSettingProfile* pProfile = pManager->GetParameters()->GetUserSettingProfile();
				pProfile->Load();
				if ((pProfile != NULL) && (szGlassName == NULL))
				{
					pTracker->SetDataFromExternal(pProfile->GetSensorDataFromJava());
				}
			}
#endif 
			// Must call StartTracker() first to set sensor before CheckSensors()
			ret = pTracker->StartTrack(nSampleFrequence, pManager->GetParameters()->GetDeviceParameters()->GetModel(), szGlassName);

#ifdef REPORT_ABNORMAL_SENSOR
			int iCheckSensors = pTracker->CheckSensors();
			if (iCheckSensors != 0 && !g_bSendErrorReport)
			{
				GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGyroOffsetReportSet(pTracker->GetCheckSensorString());
				GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->Post();
				g_bSendErrorReport = true;
			}
#endif
		}
		else
		{
			MOJING_ERROR(g_APIlogger , "Tracker == NULL");
		}
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "Manager == NULL");
	}
#ifdef	USING_MINIDUMP
	g_MojingMinidump.m_bSensorOn = ret;
#endif

	MOJING_TRACE(g_APIlogger, "StartTracker " << (ret ? "SUCCEEDED!" : "FAILD!"));
	return ret;
}

void MojingSDK_SendSensorData(float* pArray, double dLastSampleTime)
{
	ENTER_MINIDUMP_FUNCTION;
//	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_SendSensorData before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
	
	Manager::GetMojingManager()->GetTracker()->SendSensorData(pArray, dLastSampleTime);
}

void MojingSDK_SendControllerData(const Baofeng::UByte* pArray, int dataLen)
{
	ENTER_MINIDUMP_FUNCTION;
	//	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_SnedControllerData before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
#ifdef MJ_OS_ANDROID
    Manager::GetMojingManager()->GetControlTracker()->ReadMMapData(pArray, dataLen);
#endif
}

void MojingSDK_SendControllerDataV2(float* pArray, double dTimestamp, bool bRecenter)
{
	ENTER_MINIDUMP_FUNCTION;
	//MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_SendControllerDataV2 before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}
	
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
	Manager::GetMojingManager()->GetControlTracker()->SendControllerData(pArray, dTimestamp, bRecenter);
#endif
}

int MojingSDK_CheckSensors()
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "StartTracker befor SDK init! InitStatus = " << pStatus->GetInitStatus());
		return -1;
	}
	return Manager::GetMojingManager()->GetTracker()->CheckSensors();
}
int MojingSDK_GetMaxSensorsSampleRate()
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "StartTracker befor SDK init! InitStatus = " << pStatus->GetInitStatus());
		return -1;
	}
	return Manager::GetMojingManager()->GetTracker()->GetMaxSensorsSampleRate();
}
int MojingSDK_StartTrackerCalibration()
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_StartCalibration FAILD! InitStatus = " << pStatus->GetInitStatus());
		return -1;
	}

	if (pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_StartCalibration FAILD! TRACKER_START = " << pStatus->GetInitStatus());
		return -2;
	}
	if (NULL == Manager::GetMojingManager()->GetParameters()->GetFactoryCalibrationParameters())
		return -3;
	/*Manager::GetMojingManager()->GetTracker()->ResetCalibrationResetCount();*/
	//Manager::GetMojingManager()->GetTracker()->SetCalibrationRate(0);
	Manager::GetMojingManager()->GetParameters()->GetFactoryCalibrationParameters()->SetCalibrated(false);
	return 0;
}

#ifdef MJ_OS_ANDROID
int MojingSDK_StartTrackerChecker(int nSampleFrequence)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
#ifdef REPORT_ABNORMAL_SENSOR
	static bool g_bSendErrorReport = false;
#endif
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "StartTracker befor SDK init! InitStatus = " << pStatus->GetInitStatus());
		return false;
	}

	bool ret = false;
	if (nSampleFrequence < 50)
	{
		pStatus->SetTrackerStatus(TRACKER_STOP);
		MOJING_ERROR(g_APIlogger, "StartTracker Faild! SampleFrequence < 50");
		return false;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		if (pTracker)
		{
#ifdef REPORT_ABNORMAL_SENSOR
			int iCheckSensors = pTracker->CheckSensors();
			if (iCheckSensors != 0 && !g_bSendErrorReport)
			{
				GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGyroOffsetReportSet(pTracker->GetCheckSensorString());
				GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->Post();
				g_bSendErrorReport = true;
			}
#endif
			ret = pTracker->StartTrackChecker(nSampleFrequence);
		}
	}

	MOJING_TRACE(g_APIlogger, "StartTrackerChecker " << (ret ? "SUCCEEDED!" : "FAILD!"));
	return ret;
}

int MojingSDK_GetTrackerCheckerResult(__tagSampleCheckeResult *pOutCheckeResult)
{
	memcpy(pOutCheckeResult, AndroidInternalSensorChecker::GetSampleCheckeResult(), sizeof(__tagSampleCheckeResult));

	return 1;
}

void MojingSDK_DD_SetEnableTracker(bool bEnable)// 开启或关闭DD陀螺仪
{
	g_bEnableDDTracker = bEnable;
}
bool MojingSDK_DD_GetEnableTracker()// 获取DD陀螺仪状态
{
	return g_bEnableDDTracker;
}
bool MojingSDK_DD_GetLastHeadView(float* pfViewMatrix)// 当DD陀螺仪关闭时，存放真实的陀螺仪数据
{
	if (!g_bEnableDDTracker)
	{
		memcpy(pfViewMatrix, g_fDDHeaderView, sizeof(float)* 16);
	}
	return !g_bEnableDDTracker;
}

#endif // ANDROID

float MojingSDK_IsTrackerCalibrated()
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_StartCalibration FAILD! InitStatus = " << pStatus->GetInitStatus());
		return 0;
	}

	if (pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_StartCalibration FAILD! TRACKER_START = " << pStatus->GetInitStatus());
		return 0;
	}
	
	if (Manager::GetMojingManager()->GetParameters()->GetFactoryCalibrationParameters()->GetCalibrated())
	{
		MOJING_TRACE(g_APIlogger , "MojingSDK GetCalibrated Succeeded");
		return 1;
	}
	Tracker* pTracker = Manager::GetMojingManager()->GetTracker();
// 	if (pTracker->GetCalibrationResetCount() >= 3)
// 	{
// 		return -1;
// 	}

	float fRet = pTracker->GetCalibrationRate();
	MOJING_TRACE(g_APIlogger, "MojingSDK GetCalibrated " << fRet);
	return fRet ;
}

void MojingSDK_ResetSensorOrientation(void)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "ResetSensorOrientation FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		pTracker->RecenterYaw();
	}
}

void MojingSDK_ResetSensorOrientation2(void)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "ResetSensorOrientation2 FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		pTracker->Recenter();
	}
}


void MojingSDK_ResetTracker(void)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "ResetTracker FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		pTracker->ResetTracker();
	}
}

double MojingSDK_getLastSensorState(float* fArray)
{
	double Ret = 0;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "getLastSensorState FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		memset(fArray, 0, sizeof(float)* 10);
		return Ret;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		if (pTracker)
		{
			Ret = pTracker->getLastSensorState(fArray);			
		}
	}

	return Ret;
}

uint64_t MojingSDK_getLastHeadView(float* pfViewMatrix)
{
	ENTER_MINIDUMP_FUNCTION;
//#ifdef _DEBUG
//	MOJING_FUNC_TRACE(g_APIlogger);
//#endif
	uint64_t Ret = 0;
	
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "getLastHeadView FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		memset(pfViewMatrix , 0 , sizeof(float ) * 16);
		pfViewMatrix[0] = pfViewMatrix[5] = pfViewMatrix[10] = pfViewMatrix[15] = 1.0f;
		return Ret;
	}
	/*首先尝试从正在绘制畸变的显示帧数据中获取正确的显示矩阵*/
	MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
	double dTime = 0;
	if (pRender && pRender->GetEnableTimeWarp())
	{// 不需要TimeWarp的时候是不需要取得预测数据的
		Ret = pRender->GetCurrentModelFrameInfo(&dTime, pfViewMatrix);
	}
	if (Ret == 0)
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			Tracker* pTracker = pManager->GetTracker();
			Matrix4f viewMatrix;
			pTracker->getLastHeadView(viewMatrix);

			for (int i = 0; i < 16; i++)
			{
				pfViewMatrix[i] = viewMatrix.M[i / 4][i % 4];
			}
		}
	}
    
    return Ret;
}

int MojingSDK_getPredictionHeadView(float* pfViewMatrix, double time)
{
	ENTER_MINIDUMP_FUNCTION;
	//MOJING_FUNC_TRACE(g_APIlogger);

	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "getLastHeadView FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		memset(pfViewMatrix, 0, sizeof(float)* 16);
		pfViewMatrix[0] = pfViewMatrix[5] = pfViewMatrix[10] = pfViewMatrix[15] = 1.0f;
		return -1;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		Matrix4f viewMatrix = Matrix4f(pTracker->getPredictionPosition(time));

		for (int i = 0; i < 16; i++)
		{
			pfViewMatrix[i] = viewMatrix.M[i / 4][i % 4];
		}
	}
	return 0;
}

void MojingSDK_getLastHeadEulerAngles(float* pfEulerAngles)
{	
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "getLastHeadEulerAngles FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		memset(pfEulerAngles, 0, sizeof(float)* 3);
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		float yaw, pitch, roll;
		pTracker->getLastHeadEulerAngles(&yaw, &pitch, &roll);
		pfEulerAngles[0] = yaw;
		pfEulerAngles[1] = pitch;
		pfEulerAngles[2] = roll;
	}
}

void MojingSDK_getLastHeadQuarternion(float *w, float *x, float *y, float *z)
{
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled() || pStatus->GetTrackerStatus() != TRACKER_START)
	{
		MOJING_ERROR(g_APIlogger, "getLastHeadQuarternion FAILD! InitStatus = " << pStatus->GetInitStatus() << " , TrackerStatus = " << pStatus->GetTrackerStatus());
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
		double dTime = 0;
		UInt64 ui64Ret = 0;
		if (pRender && pRender->GetEnableTimeWarp())
		{// 不需要TimeWarp的时候是不需要取得预测数据的
			ui64Ret = pRender->GetCurrentModelFrameInfo(&dTime, x, y, z, w);
		}
		
		if (ui64Ret == 0)
		{
			Tracker* pTracker = pManager->GetTracker();
			pTracker->getLastHeadQuarternion(x, y, z, w);
		}
		
	}
	else
	{
		*w = 1;
		*x = 0;
		*y = 0;
		*z = 0;
	}
}

void MojingSDK_StopTracker(void)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call StopTracker befor Init! InitStatus = " << pStatus->GetInitStatus());
		return;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		Tracker* pTracker = pManager->GetTracker();
		if (pTracker)
		{
			if (pStatus->GetSensorOrigin() != SENSOR_FROM_JAVA)
			{
				if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
				{	
					int iCount = 0;
					while (pStatus->GetTrackerStatus() == TRACKER_START_NOW && iCount++ < 1000)
					{// 5ms
						usleep(5000);
					}
				}
				if (pStatus->GetTrackerStatus() != TRACKER_START)
				{
					MOJING_ERROR(g_APIlogger, "Call StopTracker And  Tracker NOT START! Status = " << pStatus->GetTrackerStatus());
					return;
				}
			}
			pTracker->StopTrack();
			MOJING_TRACE(g_APIlogger, "StopTrack SUCCEEDED");
		}
	}

#ifdef	USING_MINIDUMP
	g_MojingMinidump.m_bSensorOn = pStatus->GetTrackerStatus() == TRACKER_START;
#endif
}

bool MojingSDK_EnterMojingWorld(const char * szGlassesName, bool bEnableMultiThread, bool bEnableTimeWarp)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "EnterMojingWorld with out Init SDK!");
		return false;
	}
	MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
	MachineListNode CurrentMachineType = pDeviceParameters->GetCurrentMachine();
	bool bIsUnreal = (pStatus->GetEngineStatus() == ENGINE_UNREAL);
	bool bIsGear = (pStatus->GetEngineStatus() == ENGINE_GEAR);
	bool bIsUnityWithQ820 = ((pStatus->GetEngineStatus() == ENGINE_UNITY) &&
		(pDeviceParameters->GetCurrentMachine().m_iID == 2) &&
		(pDeviceParameters->GetAbility() & DEVICE_ABILITY_SVR) != 0);

	if (!bIsUnreal && !bIsUnityWithQ820 && !bIsGear)
	{
		if (NULL != MojingRenderBase::GetCurrentRender())
		{
			MOJING_ERROR(g_APIlogger, "EnterMojingWorld again!");
			return true;
		}

		/*获取GLES 和 EGL 信息*/
		char EGL[EGL_INFO_BUFFER_LENGTH] = "Unknown";
		char GLES[EGL_INFO_BUFFER_LENGTH] = "Unknown";
		char GPUNAME[EGL_INFO_BUFFER_LENGTH] = "Unknown";
#if defined(MJ_OS_MAC)
        const char* glVer = GetGLVersion();
        if(glVer != NULL)
            strcpy(EGL, glVer);
        const char* glesVer = GetGLESVersion();
        if(glesVer != NULL)
            strcpy(EGL, glesVer);
#elif defined(MJ_OS_ANDROID)
		MOJING_TRACE(g_APIlogger, "EglSetup");

		// 此函数将把egl.pbufferSurface作为默认的绘制目标
		egl = EglSetup(EGL_NO_CONTEXT, GL_ES_VERSION,	// no share context,
			8, 8, 8, 0, 4, // r g b
			EGL_CONTEXT_PRIORITY_MEDIUM_IMG,
			EGL, GLES, GPUNAME);


#elif defined(MJ_OS_WIN32)

#endif 

		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetEGL(EGL);
		GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGLES(GLES);
		MojingPlatformBase::GetPlatform()->SetGPUName(GPUNAME);

	}	//float *pTest2 = new float[20];
	
	if (MojingSDK_ChangeMojingWorld(szGlassesName))
	{

		if (!bIsUnreal// UNREAL不需要 创建Render对象
			&& !bIsUnityWithQ820
			&& !bIsGear)// 高通820的Unity模式不创建Render对象
		{// Unreal 不用创建绘制对象
			//MOJING_TRACE(g_APIlogger ,"MojingRenderBase::CreateCurrentRender - 1");
			MojingRenderBase::CreateCurrentRender(bEnableMultiThread, bEnableTimeWarp);
			//MOJING_TRACE(g_APIlogger, "MojingRenderBase::CreateCurrentRender - 2");
			bool bRet = MojingRenderBase::GetCurrentRender() != NULL;
			MOJING_TRACE(g_APIlogger, "MojingRenderBase::CreateCurrentRender - 3 :: " << bRet);
			return bRet;
		}
		else
		{
			MOJING_TRACE(g_APIlogger, "Skip creat render object , " << bIsUnreal << bIsUnityWithQ820);
			return true;
		}
	}
	return false;
}

int MojingSDK_GetDistortionMesh(const char * szGlassesName, int iScreenWidth, int iScreenHeight, const int iWidthCells, const int iHeightCells,
	float *pFOV, float *pGlassesSeparationInMeter,	
	void * pOutVectex, void * pOutUV, void * pOutIndex)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "EnterMojingWorld with out Init SDK!");
		return 0;
	}

	if (MojingSDK_ChangeMojingWorld(szGlassesName))
	{
		MojingDisplayParameters *pDisplay = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();

		// 宽度或者高度=0，表示不需要修改分辨率
		if (iScreenWidth > 0 && iScreenHeight > 0)
		{
			pDisplay->SetScreenWidth(fmax(iScreenWidth, iScreenHeight));
			pDisplay->SetScreenHeight(fmin(iScreenWidth, iScreenHeight));
		}

		int iRet = iWidthCells * iScreenHeight * 2;		

		Distortion *pDistortion = Manager::GetMojingManager()->GetDistortion();

		if (pFOV)
			*pFOV = pDistortion->GetFOV();
		if (pGlassesSeparationInMeter)
			*pGlassesSeparationInMeter = pDistortion->GetLensSeparation();

		if (pStatus->GetEngineStatus() == ENGINE_UNREAL)
		{
			iRet = (iWidthCells + 1) * (iScreenHeight + 1 )* 2;
			if (pOutVectex && pOutIndex)
			{
				return pDistortion->UNREAL_BuildDistortionMesh(iWidthCells, iHeightCells, pOutVectex, pOutIndex);
			}
		}
		else if (pStatus->GetEngineStatus() == ENGINE_UNITY)
		{
			if (pOutVectex && pOutUV && pOutIndex)
			{
				return pDistortion->UNITY_BuildDistortionMesh(iWidthCells, iHeightCells, pOutVectex, pOutUV, pOutIndex);
			}
		}
		return iRet;
	}
	return 0;
}
extern unsigned char Baofeng::Mojing::g_EncKey[16];
bool MojingSDK_ChangeMojingWorld(const char * szGlassesName)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (szGlassesName == NULL)
	{
		MOJING_ERROR(g_APIlogger, "Glass Name Is Null!!!");
		return false;
	}
	if (*szGlassesName == 0)
	{
		MOJING_ERROR(g_APIlogger, "Glass Name Is Empty!!!");
		return false;
	}
	
	MOJING_TRACE(g_APIlogger, "Set Glasses : \"" << szGlassesName << "\"");
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_ChangeMojingWorld with out Init SDK!");
		return false;
	}
	const char* pszCurrentGlassesName = pStatus->GetGlassesName();
	if (0 != strcmp(szGlassesName, pszCurrentGlassesName) && *szGlassesName != 0)
	{
		// 注意：当UpdateFromProfile执行成功时，会自动调用MojingRenderBase::SetModify()。所以这里不用额外调用
		Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
		bool bRet = false;
		if (pParameters && pParameters->GetGlassesConfigProfile())
		{
			MOJING_TRACE(g_APIlogger , "Using GlassesConfigProfileV2");
			bRet = pParameters->GetGlassesConfigProfile()->UsingMojingWorldKey(szGlassesName);
		}
		else
		{
			MOJING_TRACE(g_APIlogger, "Using Online GlassesConfigProfile");
			bRet = GlassesConfigProfile::UpdateFromProfile(szGlassesName, MojingPlatformBase::GetPlatform()->GetPacketProfilePath());
		}

		if (bRet)
		{
			// 注意：这个GlassName是UUID格式
			pStatus->SetGlassesName(szGlassesName);
			GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGlassesName(MojingProfileKey::GetGlassKeyIDString(szGlassesName));
			GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGlassesReportName(MojingProfileKey::GetGlassKeyIDString(szGlassesName));
#ifdef	USING_MINIDUMP
			g_MojingMinidump.SetGlassesName(MojingProfileKey::GetGlassKeyIDString(szGlassesName));
			g_MojingMinidump.m_bInMojingWorld = true;
#endif
			MojingSDK_SetMojingWorldKey(MOJING_WORLDKEY_LAST, szGlassesName);
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "Can not find Distortion info @ " << szGlassesName);
		}

		return bRet;
	}
	return true;
}

bool MojingSDK_SetDefaultMojingWorld(const char * szGlassesName)
{
	return MojingSDK_SetMojingWorldKey(MOJING_WORLDKEY_DEFAULT, szGlassesName);
}

bool MojingSDK_SetMojingWorldKey(const char* szKeyType, const char* szGlassesName)
{
	String szKey = szKeyType;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	MOJING_TRACE(g_APIlogger, "Set " << szKey  << ": \"" << szGlassesName << "\"");
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_SetMojingWorldKey with out Init SDK!");
		return false;
	}

	MojingProfileKey profileKey;
	bool bRet = profileKey.SetString(szGlassesName);
	if (!bRet)
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_SetMojingWorldKey GlassesName is invalid!");
		return false;
	}

	profileKey.SetAppID(0);
	String szProfileKey = profileKey.GetString();
	//MOJING_TRACE(g_APIlogger, "szGlassesName : " << szGlassesName << " ++++ szProfileKey : " << szProfileKey.ToCStr());
	String strFilePath = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
	strFilePath += "/MojingWorld.dat";
	JSON* jsSave = JSON::Load(strFilePath.ToCStr(), g_EncKey);
	if (jsSave == NULL)
	{
		jsSave = JSON::CreateObject();
		jsSave->AddStringItem(szKey, szProfileKey);
	}
	else
	{
		JSON* jsDefault = jsSave->GetItemByName(szKey);
		if (jsDefault == NULL)
		{
			jsSave->AddStringItem(szKey, szProfileKey);
		}
		else
		{
			jsDefault->Value = szProfileKey;
		}
	}
	jsSave->Save(strFilePath.ToCStr(), g_EncKey);

	return true;
}
String MojingSDK_GetMojingWorldKey(const char* szKeyType)
{
	String Ret = "";
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
			if (pGlassesConfigProfile)
			{
				String strFilePath = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
				strFilePath += "/MojingWorld.dat";
				JSON* jsSave = JSON::Load(strFilePath.ToCStr(), g_EncKey);
				if (jsSave != NULL)
				{

					JSON* jsDefault = jsSave->GetItemByName(szKeyType);
					if (jsDefault != NULL)
					{
						Ret = jsDefault->Value.ToCStr();
					}
				}
			}
		}
	}
	return Ret;
}

String MojingSDK_GetDefaultMojingWorld(const char* strLanguageCodeByISO639)
{
	String strRet = "{\"ERROR\":\"Get default MojingWorld failed.\"}";
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
		if (pGlassesConfigProfile)
		{
			strRet = pGlassesConfigProfile->GetMojingWorldJson(MojingSDK_GetMojingWorldKey(MOJING_WORLDKEY_DEFAULT), strLanguageCodeByISO639);
		}
	}
	//MOJING_TRACE(g_APIlogger, "Get default MojingWorld : " << strRet.ToCStr());
	return strRet;
}


String MojingSDK_GetLastMojingWorld(const char* strLanguageCodeByISO639)
{
	String strRet = "{\"ERROR\":\"Get last MojingWorld failed.\"}";
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
		if (pGlassesConfigProfile)
		{
			strRet = pGlassesConfigProfile->GetMojingWorldJson(MojingSDK_GetMojingWorldKey(MOJING_WORLDKEY_LAST), strLanguageCodeByISO639);
		}
	}
	//MOJING_TRACE(g_APIlogger, "Get last MojingWorld : " << strRet.ToCStr());
	return strRet;
}
/*
int MojingSDK_GetMojingWorldDistortionMesh(int iScreenWidth, int iScreenHeight, int iWidthCells, int iHeightCells, void * pVerts, void * pIndices)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	MOJING_TRACE(g_APIlogger, "Get Distortion Mesh " << iScreenWidth << " x " << iScreenHeight);

	// 输入的宽、高是格子的数量
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_GetMojingWorldDistortionMesh FAILD! InitStatus = " << pStatus->GetInitStatus());
		return -1;
	}

	if (String(pStatus->GetGlassesName()) != "")
	{
		Distortion *pDistortion = Manager::GetMojingManager()->GetDistortion();
		if (pDistortion->GetSegment() != 0)
		{
			if (pVerts == NULL)
				return 2 * (iWidthCells + 1) * (iHeightCells + 1);
			MojingDisplayParameters *pDisplay = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
			
			// 宽度或者高度=0，表示不需要修改分辨率
			if (iScreenWidth > 0 && iScreenHeight > 0)
			{
				pDisplay->SetScreenWidth(fmax(iScreenWidth, iScreenHeight));
				pDisplay->SetScreenHeight(fmin(iScreenWidth, iScreenHeight));
			}

			Unreal_DistortionVertexBuffer* pDistortionVertexBuffer = pDistortion->BuildUnrealDistortionVertexBuffer(iWidthCells , iHeightCells );
			if (pDistortionVertexBuffer)
			{
				memcpy(pVerts, pDistortionVertexBuffer->m_pDistortionVertex, sizeof(Unreal_DistortionVertexNode)* pDistortionVertexBuffer->m_iVertexCount);
				memcpy(pIndices, pDistortionVertexBuffer->m_pDistortionVertexIndex, sizeof(short) * pDistortionVertexBuffer->m_iIndexCount);
				delete pDistortionVertexBuffer;
				return 2 * (iWidthCells + 1) * (iHeightCells + 1);
			}
		}
		{
			return -1;
		}
	}
	return 0;
}*/
int MojingSDK_GetTextureSize()
{
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_GetTextureSize FAILD! InitStatus = " << pStatus->GetInitStatus());
		return -1;
	}
	
#if defined MJ_OS_MAC
    return 1024;
#else 
// #if defined MJ_OS_WIN32
// 	return 2048;
// #else
	Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
	MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
	int height = pDisplay->GetScreenHeight();
	int width = pDisplay->GetScreenWidth();

	int iScreenSize = fmin(height , width);//(height > width) ? height / 2 : width / 2;
	// 安卓手机最大提供1024的分辨率
	//return fmin(1024 , iScreenSize &0xFFFFFF00);
	if (Manager::GetMojingManager()->GetParameters()->GetDeviceParameters()->GetCurrentMachine().m_iID == 2)
	{// 820一体机，分辨率1440
		//MOJING_TRACE(g_APIlogger , "MachineType = 2 , Set TextureSize = 1440" );
		return 1440;
	}
	return max(1024u , iScreenSize & 0xFFFFFF00);
//#endif
#endif
}

#ifdef MJ_OS_ANDROID
bool MojingSDK_IsUseUnityForSVR()
{
    MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
    if (pDeviceParameters == NULL)
    {
        MOJING_TRACE(g_APIlogger, "MojingSDK_IsUseUnityForSVR: get DeviceParameters failed.");
        return false;
	}
	if (pDeviceParameters->GetAbility() & DEVICE_ABILITY_SVR && (pDeviceParameters->GetCurrentMachine().m_iID == 2))
	{
	MOJING_TRACE(g_APIlogger, "Run in SVR device...");
	return true;
	}

    return false;
}

bool MojingSDK_IsInMachine()
{
	MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
	if (pDeviceParameters == NULL)
	{
		MOJING_TRACE(g_APIlogger, "MojingSDK_IsInMachine: get DeviceParameters failed.");
		return false;
	}

	if (pDeviceParameters->GetIsMachine())
	{
		MOJING_TRACE(g_APIlogger, "Run in MATRIX...");
		return true;
	}
	return false;
}

bool MojingSDK_IsUseForDayDream()
{
    MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();

    if (pStatus == NULL)
    {
        MOJING_TRACE(g_APIlogger, "GetSDKStatus: get SDKStatus failed.");
        return false;
    }
    if (pStatus->GetEngineStatus() & ENGINE_GVR)
    {
        MOJING_TRACE(g_APIlogger, "Run in GVR device...");
        return true;
    }
    return false;
}
#endif

float MojingSDK_GetFOV()
{
	ENTER_MINIDUMP_FUNCTION;
	//MOJING_FUNC_TRACE(g_APIlogger);
	float fRet = 0.000;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		const char *szGlassesName = pStatus->GetGlassesName();
		Distortion* pDistortion = Manager::GetMojingManager()->GetDistortion();

		if (MojingRenderBase::GetCurrentRender() != NULL)
		{	
			if (pDistortion)
			{
				fRet = pDistortion->GetFOV();
				if (0 == szGlassesName || 0 == *szGlassesName)
				{
					MOJING_TRACE(g_APIlogger , "Can not get GlassesName from MojingSDKStatus , reset from pDistortion");
					pStatus->SetGlassesName(pDistortion->GetGlassKey());
				}				
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "Can not get FOV , pDistortion is NULL...");
			}
		}
		else if (pStatus->GetEngineStatus() == ENGINE_GVR)
		{
			MOJING_TRACE(g_APIlogger, "Engine = GVR");
			MojingProfileKey Key;
			String sKey = "";
			if (0 != szGlassesName && 0 != *szGlassesName)
			{
				MOJING_TRACE(g_APIlogger, "Try settings....");
				sKey = szGlassesName;
			}
			else
			{// 注意：因为下面的函数每次都要去读取文件，所以尽量从pStatus->GetGlassesName();获取畸变镜片
				MOJING_TRACE(g_APIlogger, "Try default....");
				sKey = MojingSDK_GetMojingWorldKey(MOJING_WORLDKEY_DEFAULT);
			}
			if (Key.SetString(sKey))
			{
				Baofeng::Mojing::Parameters* pParameters = Baofeng::Mojing::Manager::GetMojingManager()->GetParameters();
				GlassesConfigProfileV2* pGlassesConfig = pParameters->GetGlassesConfigProfile();
				GlassInfo *pGlassInfo = pGlassesConfig->GetGlass(Key.GetGlassID());
				if (pGlassInfo)
				{
					fRet = pGlassInfo->GetFOV();
				}
				else
				{
					MOJING_TRACE(g_APIlogger, "Can not find glasses info....");
				}
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "Invalid Key..." << sKey.ToCStr());
			}
		}
		else if (0 != szGlassesName && 0 != *szGlassesName)
		{
			Distortion* pDistortion = Manager::GetMojingManager()->GetDistortion();
			if (pDistortion)
			{
				fRet = pDistortion->GetFOV();
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "Can not get FOV , pDistortion is NULL...");
			}
		}
		else
		{
			MOJING_ERROR(g_APIlogger , "Can not get FOV , GlassName is EMPTY...");
		}
	}
//	MOJING_TRACE(g_APIlogger, "FOV = " << fRet);
	return fRet;
}

void MojingSDK_SetCenterLine(int iWidth, int colR, int colG, int colB, int colA)
{
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		const char *szGlassesName = pStatus->GetGlassesName();
		if (0 != szGlassesName && 0 != *szGlassesName)
		{
			MojingRenderBase* pCurrentRender = MojingRenderBase::GetCurrentRender();
			if (pCurrentRender)
			{
				pCurrentRender->SetCenterLine(iWidth, colR, colG, colB, colA);
			}
		}
	}
}
#define EYE_LEFT	0
#define EYE_CENTER	1
#define EYE_RIGHT	2
void MojingSDK_GetProjectionMatrix(int eye, bool bVrMode, float fFOV, float fNear, float fFar, float* pfProjectionMatrix, int* pfViewRect)
{
	ENTER_MINIDUMP_FUNCTION;
	Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
	MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
	int iHeight = pDisplay->GetScreenHeight();
	int iWidth = pDisplay->GetScreenWidth();

	float fAspect;
	// iMin = 手柄屏幕长边的一半
	int iMin = (iWidth > iHeight) ? iWidth / 2 : iHeight / 2;
	// iMax 
	int iMax = (iWidth > iHeight) ? iHeight : iWidth;

	if (bVrMode)
	{
		// VR模式，双摄像机
		if (MojingSDK_IsGlassesNeedDistortion())
		{
			// 需要做反畸变，长宽比为1
			fAspect = 1.0f;
			if (eye == EYE_LEFT)
			{
				pfViewRect[0] = 0;//left
				pfViewRect[1] = (iMax - iMin) / 2; // bottom
				pfViewRect[2] = iMin; //right
				pfViewRect[3] = iMin;// (iMax + iMin) / 2; //top
			} 
			else if (eye == EYE_RIGHT)
			{
				pfViewRect[0] = iMin;//left
				pfViewRect[1] = (iMax - iMin) / 2; // bottom
				pfViewRect[2] = iMin;// iMin * 2; //right
				pfViewRect[3] = iMin;// (iMax + iMin) / 2; //top
			}
		}
		else
		{
			// 不需要做反畸变，长宽比为半屏的长宽比
			fAspect = (iMin * 1.0f) / (iMax * 1.0f);
			if (eye == EYE_LEFT)
			{
				pfViewRect[0] = 0;//left
				pfViewRect[1] = 0; // bottom
				pfViewRect[2] = iMin; //right
				pfViewRect[3] = iMax; //top
			}
			else if (eye == EYE_RIGHT)
			{
				pfViewRect[0] = iMin;//left
				pfViewRect[1] = 0; // bottom
				pfViewRect[2] = iMin;//iMin * 2; //right
				pfViewRect[3] = iMax; //top
			}
		}
	}
	else
	{
		// 普通模式，单摄像机，使用全部屏幕
		fAspect = (iMin * 2.0f) / (iMax * 1.0f);
		if (eye == EYE_CENTER)
		{
			pfViewRect[0] = 0;//left
			pfViewRect[1] = 0; // bottom
			pfViewRect[2] = iMin * 2; //right
			pfViewRect[3] = iMax; //top
		}
	}

	Matrix4f m;
	m.PerspectiveRH(fFOV, fAspect, fNear, fFar);
	memcpy(pfProjectionMatrix, m.M[0], 16);
}

float MojingSDK_GetGlassesSeparation()
{
	ENTER_MINIDUMP_FUNCTION;
	// MOJING_FUNC_TRACE(g_APIlogger);
	float fRet = 0.000;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Distortion* pDistortion = Manager::GetMojingManager()->GetDistortion();
		fRet = pDistortion->GetLensSeparation();
	}
	// MOJING_TRACE(g_APIlogger, "Lens Separation = " << fRet);
	return fRet;
}
float MojingSDK_GetGlassesSeparationInPix()
{
	ENTER_MINIDUMP_FUNCTION;
	float fRet = 0.00000f;
	const char *pGlcassName = MojingSDK_GetGlasses();
	if (pGlcassName && *pGlcassName)
	{
		fRet = MojingSDK_GetGlassesSeparation();
		if (fRet > 0.01)// 1CM
		{
			MojingDisplayParameters* pDisplay = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
			fRet = pDisplay->Meter2Pix(fRet);
		}
	}
	return fRet;
}
void MojingSDK_GetScreenSize(float* pSize)
{
	ENTER_MINIDUMP_FUNCTION;
	// MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		MojingDisplayParameters* pDisplay = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
		pSize[0] = pDisplay->GetScreenWidthMeter();
		pSize[1] = pDisplay->GetScreenHeightMeter();
	}
	MOJING_TRACE(g_APIlogger, "Screen Size: " << pSize[0] << " x " << pSize[1]);
}

bool MojingSDK_LeaveMojingWorld()
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "LeaveMojingWorld with out Init SDK!");
		return false;
	}
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
    if (MojingRenderMetal::GetInstance())
    {
        pStatus->SetGlassesName("");
        GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGlassesName("");
        MojingRenderMetal::Release();
    }
#endif
#endif
	if (NULL == MojingRenderBase::GetCurrentRender())
	{
		MOJING_ERROR(g_APIlogger, "LeaveMojingWorld again!");
		return true;
	}
	pStatus->SetGlassesName("");
	GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGlassesName("");

	MojingRenderBase::ReleaseCurrentRender();
	bool bRet = MojingRenderBase::GetCurrentRender() == NULL;

#ifdef	USING_MINIDUMP
	if (bRet)
	{
		g_MojingMinidump.SetGlassesName("");
		g_MojingMinidump.m_bInMojingWorld = false;
	}
#endif

	return bRet;
}

bool MojingSDK_OnSurfaceChanged(int newWidth, int newHeight)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);

	MojingDisplayParameters * pDisplay = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
    
#ifdef MJ_OS_WIN32
	pDisplay->SetScreenWidth(fmax(newWidth, newHeight));
	pDisplay->SetScreenHeight(fmin(newWidth, newHeight));
    pDisplay->SetXdpi(fmax(newWidth, newHeight)*336/1920);
    pDisplay->SetYdpi(fmin(newWidth, newHeight)*336/1080);
#else
#ifdef AUTO_LANDSCAPE
	pDisplay->SetScreenWidth(newWidth);
	pDisplay->SetScreenHeight(newHeight);
#else
	pDisplay->SetScreenWidth(fmax(newWidth, newHeight));
	pDisplay->SetScreenHeight(fmin(newWidth, newHeight));
#endif
#endif
    
// 	pDisplay->SetScreenMeters();
	MojingRenderBase::SetModify();
	MOJING_TRACE(g_APIlogger, "Change Surface : " << pDisplay->GetScreenWidth() << " x " << pDisplay->GetScreenHeight());
	return true	;
}

void MojingSDK_SetTextureID(int nLeftEyeTextureID, int nrightEyeTextureID)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MOJING_TRACE(g_APIlogger, "Set texture ID to (" << nLeftEyeTextureID << ", " << nrightEyeTextureID  << ")");
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
		if (pRender)
		{
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
			if (pStatus->GetVerfiyStatus() != VERIFY_OK)
			{
				pRender->SetEyeTexID(0, 0);
			}
			else
			{
				pRender->SetEyeTexID(nrightEyeTextureID, nrightEyeTextureID);
			}
// 			pRender->SetLeftEyeTexID(nrightEyeTextureID);
// 			pRender->SetRightEyeTexID(nrightEyeTextureID);
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "Set texture ID failed.");
		}
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "Not Init.");
	}
}

bool MojingSDK_DrawTexture(void)
{
	ENTER_MINIDUMP_FUNCTION;
#ifdef _DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif

	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call DrawTexture befor Init! InitStatus = " << pStatus->GetInitStatus());
		return false;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
		if (pRender == NULL)
		{
			MOJING_ERROR(g_APIlogger, "Render with out Mojing Word!!");
			return false;
		}
		return pRender->WarpToScreen();
	}
	return false;
}
void MojingSDK_SetOverlayPosition(const float fLeft, const float fTop, const float fWidth, const float fHeight)
{
	ENTER_MINIDUMP_FUNCTION;
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
        MojingRenderMetal* pMetalRender = MojingRenderMetal::GetInstance();
#endif
#endif
//        if ((pRender == NULL) && (pMetalRender == NULL))
//		{
//			MOJING_ERROR(g_APIlogger, "Render without Mojing Word!!");
//			return ;
//		}
		// 注意：这里不再做有效性检查，等到绘制的时候再去处理
		Vector4f OverlayRect = Vector4f(fLeft, fTop, fWidth, fHeight); 
#ifdef _DEBUG
		char szTemp[256];
		sprintf(szTemp, "MojingSDK_SetOverlayPosition : Left&& Right : Rect = {%1.2f , %1.2f , %1.2f , %1.2f}", OverlayRect.x, OverlayRect.y, OverlayRect.z, OverlayRect.w);
		//MOJING_TRACE(g_APIlogger, szTemp);
#endif	
        if (pRender != NULL)
        {
            pRender->SetLeftOverlayRect(OverlayRect);
            pRender->SetRightOverlayRect(OverlayRect);
        }
		
        
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
        if (pMetalRender != nullptr)
        {
            pMetalRender->SetLeftOverlayRect({fLeft, fTop, fWidth, fHeight});
            pMetalRender->SetRightOverlayRect({fLeft, fTop, fWidth, fHeight});
        }
#endif
#endif
	}
}

Vector3f PointInNear(float fNear, Vector3f fEye, Vector3f fPoint)
{
	float fdX = fPoint.x - fEye.x;
	float fdY = fPoint.y - fEye.y;

	float fXNear = fdX / (fPoint.z - fEye.z) * fNear;
	float fYNear = fdY / (fPoint.z - fEye.z) * fNear;

	return Vector3f(fXNear , fYNear , fNear);
}
#define FIX_COUNT (1000.0f)

void MojingSDK_Math_GetOverlayPosition3D(float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre, Vector4f &OverlayRectLeft, Vector4f &OverlayRectRight)
{
	Manager* pManager = Manager::GetMojingManager();
	float fYOffset = pManager->GetDistortion()->GetYOffset() / pManager->GetParameters()->GetDisplayParameters()->GetScreenHeightMeter();

	if (fDistanceInMetre < 1e5)
	{
		OverlayRectLeft = OverlayRectRight = Vector4f(fLeft, fTop, fWidth, fHeight);
		return;
	}
	float fFOV = MojingSDK_GetFOV() / 180 * PI;
	double dTan = tan(fFOV / 2);
	// 目标平面的宽度
	float fWidthInDistance = 2 * dTan * fDistanceInMetre;
	float fHeightInDistance = 2 * dTan * fDistanceInMetre;
	// 近平面的宽度,近平面为屏幕平面，也就是宽度为1的平面
	float fWidthInNear = 1;
	float fHeightInNear = 1;
	float fNear = fWidthInNear / 2 / dTan;
	Vector3f v3Center = Vector3f((fLeft + fWidth / 2)* fWidthInDistance, (fTop + fHeight / 2)* fHeightInDistance, fDistanceInMetre);

	Vector3f fLeftEye = Vector3f(MojingSDK_GetGlassesSeparation() / -2, 0, 0);
	Vector3f v3LeftEyeNear = PointInNear(fNear, fLeftEye, v3Center);
	float fX_L = trunc((v3LeftEyeNear.x - fWidth / 2) * FIX_COUNT) / FIX_COUNT;
	float fY_L = trunc((v3LeftEyeNear.y - fHeight / 2) * FIX_COUNT) / FIX_COUNT + fYOffset / 2;
	OverlayRectLeft = Vector4f(fX_L, fY_L, fWidth, fHeight);

	Vector3f fRightEye = Vector3f(MojingSDK_GetGlassesSeparation() / 2, 0, 0);
	Vector3f v3RightEyeNear = PointInNear(fNear, fRightEye, v3Center);

	/*以下代码用于忽略误差*/

	float fX_R = trunc((v3RightEyeNear.x - fWidth / 2) * FIX_COUNT) / FIX_COUNT;
	float fY_R = trunc((v3RightEyeNear.y - fHeight / 2) * FIX_COUNT) / FIX_COUNT;
	OverlayRectRight = Vector4f(fX_R, fY_R, fWidth, fHeight);
}

Rectf MojingSDK_Math_OverlayPosition3D(unsigned int eyeTextureType, Rectf rcRect, float fDistanceInMetre)
{
	Rectf rcRet;
	float fFOV = MojingSDK_GetFOV() / 180 * PI;
	double dTan = tan(fFOV / 2);
	// 目标平面的宽度
	float fWidthInDistance = 2 * dTan * fDistanceInMetre;
	float fHeightInDistance = 2 * dTan * fDistanceInMetre;
	// 近平面的宽度,近平面为屏幕平面，也就是宽度为1的平面
	float fWidthInNear = 1;
	float fHeightInNear = 1;
	float fNear = fWidthInNear / 2 / dTan;
	// 中心点在目标平面上的位置
	
	Vector3f v3Center = Vector3f((rcRect.x + rcRect.w / 2)* fWidthInDistance, (rcRect.y + rcRect.h/ 2)* fHeightInDistance, fDistanceInMetre);
	Vector3f fEye = Vector3f(MojingSDK_GetGlassesSeparation() / -2, 0, 0);
	if (eyeTextureType & TEXTURE_RIGHT_EYE)
	{
		/*以米为单位的眼坐标*/
		fEye.x = -fEye.x;
	}
	Vector3f v3EyeNear = PointInNear(fNear, fEye, v3Center);
	float fX = trunc((v3EyeNear.x - rcRect.w / 2) * FIX_COUNT) / FIX_COUNT;
	float fY = trunc((v3EyeNear.y - rcRect.h / 2) * FIX_COUNT) / FIX_COUNT;
	rcRet = Rectf(fX, fY, rcRect.w, rcRect.h);
	return rcRet;

}
void MojingSDK_SetOverlayPosition3D_V2(unsigned int eyeTextureType , float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre)
{

	ENTER_MINIDUMP_FUNCTION;
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
        MojingRenderMetal* pMetalRender = MojingRenderMetal::GetInstance();
#endif
#endif
		
		// fLeft、fTop、fWidth、fHeight是在平面宽度为1的地方，以左上角为0，0的坐标
		
		float fYOffset = pManager->GetDistortion()->GetYOffset() / pManager->GetParameters()->GetDisplayParameters()->GetScreenHeightMeter();


		// 1 计算贴图矩形的三维坐标
		float fFOV = MojingSDK_GetFOV() / 180 * PI;
		double dTan = tan(fFOV / 2);
		// 目标平面的宽度
		float fWidthInDistance = 2 * dTan * fDistanceInMetre;
		float fHeightInDistance = 2 * dTan * fDistanceInMetre;
		// 近平面的宽度,近平面为屏幕平面，也就是宽度为1的平面
		float fWidthInNear = 1;
		float fHeightInNear = 1;
		float fNear = fWidthInNear / 2 / dTan;
		// 中心点在目标平面上的位置
		Vector3f v3Center = Vector3f((fLeft + fWidth / 2)* fWidthInDistance, (fTop + fHeight / 2)* fHeightInDistance, fDistanceInMetre);
		if (eyeTextureType & TEXTURE_LEFT_EYE)
		{
			/*以米为单位的眼坐标*/
			Vector3f fLeftEye = Vector3f(MojingSDK_GetGlassesSeparation() / -2, 0, 0);
			Vector3f v3LeftEyeNear = PointInNear(fNear, fLeftEye, v3Center);
			float fX = trunc((v3LeftEyeNear.x - fWidth / 2) * FIX_COUNT) / FIX_COUNT;
			float fY = trunc((v3LeftEyeNear.y - fHeight / 2) * FIX_COUNT) / FIX_COUNT;
			Vector4f OverlayRectLeft = Vector4f(fX, fY - fYOffset / 2.0f, fWidth, fHeight);

#if 0
			MOJING_TRACE(g_APIlogger, "Left Eye =( " << fLeftEye.x << " , " << fLeftEye.y << " , " << fLeftEye.z << " ) , Near = " << fNear << " Size = ( " << fWidthInNear << " , " << fHeightInNear << " ) , "
				<< " Center Point = ( " << v3Center.x << " , " << v3Center.y << " , " << v3Center.z << " )  "
				<< " Center Point in Near = ( " << v3LeftEyeNear.x << " , " << v3LeftEyeNear.y << " , " << v3LeftEyeNear.z << " )  "
				<< " Center Point in Screen = ( " << v3LeftEyeNear.x / fWidthInNear << " , " << v3LeftEyeNear.y / fHeightInNear << " )  "
				<< " Display Rect ( " << OverlayRectLeft.x << " , " << OverlayRectLeft.y << " , " << OverlayRectLeft.z << " , " << OverlayRectLeft.w << " )");
#endif
			

			
			// 坐标点 是 在平面宽度为[0，1]的位置
			
			if (pRender != NULL)
			{
				pRender->SetLeftOverlayRect(OverlayRectLeft);
			}
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
            if (pMetalRender != nullptr)
            {
#ifdef _DEBUG
                char szTemp[500];
                sprintf(szTemp, "Use Metal MojingSDK_SetOverlayPosition3D_V2 SetLeftOverlayRect : Rect = {%1.2f , %1.2f , %1.2f , %1.2f}", fX, fY, fWidth, fHeight);
                MOJING_TRACE(g_APIlogger, szTemp);
#endif
                pMetalRender->SetLeftOverlayRect({fX, fY, fWidth, fHeight});
            }
#endif
#endif
		}
		if (eyeTextureType & TEXTURE_RIGHT_EYE)
		{
			/*以米为单位的眼坐标*/
			Vector3f fRightEye = Vector3f(MojingSDK_GetGlassesSeparation() / 2, 0, 0);
			Vector3f v3RightEyeNear = PointInNear(fNear, fRightEye, v3Center);
			
			/*以下代码用于忽略误差*/
			
			float fX = trunc((v3RightEyeNear.x - fWidth / 2) * FIX_COUNT) / FIX_COUNT;
			float fY = trunc((v3RightEyeNear.y - fHeight / 2) * FIX_COUNT) / FIX_COUNT;
			Vector4f OverlayRectRight = Vector4f(fX, fY - fYOffset / 2.0f, fWidth, fHeight);
#if 0
			MOJING_TRACE(g_APIlogger, "Right Eye =( " << fRightEye.x << " , " << fRightEye.y << " , " << fRightEye.z << " ) , Near = " << fNear << " Size = ( " << fWidthInNear << " , " << fHeightInNear << " ) , "
				<< " Center Point = ( " << v3Center.x << " , " << v3Center.y << " , " << v3Center.z << " )  "
				<< " Center Point in Near = ( " << v3RightEyeNear.x << " , " << v3RightEyeNear.y << " , " << v3RightEyeNear.z << " )  "
				<< " Center Point in Screen = ( " << v3RightEyeNear.x / fWidthInNear << " , " << v3RightEyeNear.y / fHeightInNear << " )  "
				<< " Display Rect ( " << OverlayRectRight.x << " , " << OverlayRectRight.y << " , " << OverlayRectRight.z << " , " << OverlayRectRight.w << " )");
#endif
			if (pRender != NULL)
			{
				pRender->SetRightOverlayRect(OverlayRectRight);
			}
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
            if (pMetalRender != nullptr)
            {
#ifdef _DEBUG
                char szTemp[500];
                sprintf(szTemp, "Use Metal MojingSDK_SetOverlayPosition3D_V2 SetRightOverlayRect : Rect = {%1.2f , %1.2f , %1.2f , %1.2f}", fX, fY, fWidth, fHeight);
                MOJING_TRACE(g_APIlogger, szTemp);
#endif
                pMetalRender->SetRightOverlayRect({fX, fY, fWidth, fHeight});
            }
#endif
#endif
		}
	}
	return;
}

void MojingSDK_SetOverlayPosition3D(unsigned int eyeTextureType, /*float fLeft, float fTop, */float fWidth, float fHeight, float fDistanceInMetre)
{
	ENTER_MINIDUMP_FUNCTION;
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
        MojingRenderMetal* pMetalRender = MojingRenderMetal::GetInstance();
#endif
#endif

//		if ((pRender == NULL) && (pMetalRender == NULL))
//		{
//			MOJING_ERROR(g_APIlogger, "Render with out Mojing Word!!");
//			return;
//		}
		fDistanceInMetre = fabsf(fDistanceInMetre);// 这里必须是绝对值
		if (fDistanceInMetre < 0.001)
		{// 小于1毫米，则设置为1毫米。否则后面可能会出现除零错
			fDistanceInMetre = 0.001;
		}
		float fLeft = 0.5 - fWidth / 2;
		float fTop = 0.5 - fHeight / 2;
		float fFOV = MojingSDK_GetFOV() / 180 * PI;
		double dTan = tan(fFOV / 2);

		// 注意 ： fLeft、fTop坐标的位置现在是在[0 , 1]空间的值，这个数据应该根据fDistanceInMetre调整
		// 这里调整到[-1 ，+1]空间
		float fCenterOfOverlayX = (fLeft + fWidth / 2) * 2 - 1;
		float fCenterOfOverlayY = (fTop + fHeight / 2) * 2 - 1;
		// fCenterOfOverlayX、fCenterOfOverlayY是在 [ -1 , +1 ]空间
		// 根据FOV 找到截距是[-1 , +1]的地方，然后换算成fDistanceInMetre位置以米为单位的X 和 Y
		
		fCenterOfOverlayX *= fDistanceInMetre * dTan;
		fCenterOfOverlayY *= fDistanceInMetre * dTan;
		// 程序执行到这里，fCenterOfOverlayX和fCenterOfOverlayY是在距离为fDistanceInMetre平面上，Overlay中点的X、Y坐标

		float fNearDistanceInMetre = 0.3f;// Unity
		float fWidthInNear = fNearDistanceInMetre * dTan;
		float fHeightInNear = fNearDistanceInMetre * dTan;

		float fCenterInNearX = (0.5 - fCenterOfOverlayX) / fDistanceInMetre * fNearDistanceInMetre;
		float fCenterInNearY = (0.5 - fCenterOfOverlayY) / fDistanceInMetre * fNearDistanceInMetre;




		Vector4f v3CenterL(MojingSDK_GetGlassesSeparation() / (2) + fCenterOfOverlayX, fCenterOfOverlayY, fDistanceInMetre, 1);
		Vector4f v3CenterR(MojingSDK_GetGlassesSeparation() / (-2) + fCenterOfOverlayX, fCenterOfOverlayY, fDistanceInMetre, 1);

		// 远平面+1是为了防止浮点数计算的时候出现不必要的剪裁
		Matrix4f m4Projection = Matrix4f::PerspectiveLH(fFOV, 1, fDistanceInMetre / 20, fDistanceInMetre + 1);
		if (eyeTextureType & TEXTURE_LEFT_EYE)
		{
			Vector4f v3Target = m4Projection.Transform(v3CenterL);
			float fXOffset = (v3Target.x / v3CenterL.z - fCenterOfOverlayX) / 2;
			float fYOffset = (v3Target.y / v3CenterL.z - fCenterOfOverlayY) / 2;

			// 注意：这里不再做有效性检查，等到绘制的时候再去处理
			Vector4f OverlayRect = Vector4f(fLeft + fXOffset, fTop + fYOffset, fWidth, fHeight);
#ifdef _DEBUG
			char szTemp[256];
			sprintf(szTemp, "MojingSDK_SetOverlayPosition2 : Left : Rect = {%1.2f , %1.2f , %1.2f , %1.2f}", OverlayRect.x, OverlayRect.y, OverlayRect.z, OverlayRect.w);
			MOJING_TRACE(g_APIlogger, szTemp);
#endif	
            if (pRender != NULL)
            {
                pRender->SetLeftOverlayRect(OverlayRect);
            }
			
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
            if (pMetalRender != nullptr)
            {
                pMetalRender->SetLeftOverlayRect({fLeft + fXOffset, fTop + fYOffset, fWidth, fHeight});
            }
#endif
#endif
		}
		if (eyeTextureType & TEXTURE_RIGHT_EYE)
		{
			Vector4f v3Target = m4Projection.Transform(v3CenterR);
			float fXOffset = (v3Target.x / v3CenterR.z - fCenterOfOverlayX) / 2;
			float fYOffset = (v3Target.y / v3CenterR.z - fCenterOfOverlayY) / 2;

			// 注意：这里不再做有效性检查，等到绘制的时候再去处理
			Vector4f OverlayRect = Vector4f(fLeft + fXOffset, fTop + fYOffset, fWidth, fHeight);
#ifdef _DEBUG
			char szTemp[256];
			sprintf(szTemp, "MojingSDK_SetOverlayPosition2 : Right: Rect = {%1.2f , %1.2f , %1.2f , %1.2f}", OverlayRect.x, OverlayRect.y, OverlayRect.z, OverlayRect.w);
			MOJING_TRACE(g_APIlogger, szTemp);
#endif	
            if (pRender != NULL)
            {
                pRender->SetRightOverlayRect(OverlayRect);
            }
			
            
#ifdef MJ_OS_IOS
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
            if (pMetalRender != nullptr)
            {
                pMetalRender->SetRightOverlayRect({fLeft + fXOffset, fTop + fYOffset, fWidth, fHeight});
            }
#endif
#endif
		}
	}
}

void MojingSDK_SetEnableTimeWarp(bool bEnable)
{
	MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
	if (pRender)
	{
		pRender->SetEnableTimeWarp(bEnable);
	}
}

#ifndef MJ_OS_WIN32
// GLsizei width;  /*!< */
// GLsizei height; /*!< */
// GLsizei depth;  /*!< */
bool MojingSDK_ktxLoadTextureM(const void* const ktxData, const int idataSize,unsigned int* pTexture, unsigned int* pTarget, int* pHeight, int* pWidth, int* pDepth, bool* pIsMipmapped, unsigned int* pGlerror, int* ktxError)
{
#ifdef ENABLE_KTX
	GLenum target;
	GLboolean isMipmapped;
	KTX_error_code ktxerror;
	KTX_dimensions kd;
	ktxerror = ktxLoadTextureM((void *)ktxData, idataSize, pTexture, pTarget, &kd, &isMipmapped, pGlerror, NULL, NULL);
	*ktxError = (int)ktxerror;
	if (KTX_SUCCESS == ktxerror) 
	{
		*pIsMipmapped = (isMipmapped == GL_TRUE);
		*pHeight = kd.height;
		*pWidth = kd.width;
		*pDepth = kd.depth;
		target = *pTarget;
		return true;
	}
#endif
	return false;
}

bool MojingSDK_ktxLoadTextureN(const char* const filename, unsigned int* pTexture, unsigned int* pTarget, int* pHeight, int* pWidth, int* pDepth, bool* pIsMipmapped, unsigned int* pGlerror, int* ktxError)
{
#ifdef ENABLE_KTX
 	GLboolean isMipmapped;
	KTX_error_code ktxerror;
	KTX_dimensions kd;
	ktxerror = ktxLoadTextureN(filename, pTexture, pTarget, &kd, &isMipmapped, pGlerror, NULL, NULL);
	*ktxError = (int)ktxerror;
	if (KTX_SUCCESS == ktxerror) 
	{
		*pIsMipmapped = (isMipmapped == GL_TRUE);
		*pHeight = kd.height;
		*pWidth = kd.width;
		*pDepth = kd.depth;
		return true;
	}
#endif

	return false;
}
#endif

bool loadFiletoMem(const char* filename, void **pMem, int *size)
{
	FILE* file = fopen(filename, "rb");
	if (NULL == file)
	{
		return false;
	}
	
	fseek(file, 0L, SEEK_END);
	int filesize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	*pMem = malloc(filesize);

	if (NULL == *pMem)
	{
		return false;
	}
	*size = filesize;
	int readSize = 0;
	while (1)
	{
		readSize += fread((char *)(*pMem) + readSize, sizeof(char), filesize - readSize, file);
		if (filesize <= readSize)
		{
			return true;
		}
	}
}

bool MojingSDK_DrawTexture(int nLeftEyeTextureID, int nrightEyeTextureID, int iLeftLayerTextureID, int iRightLayerTextureID)
{
	/*注意：*/
	ENTER_MINIDUMP_FUNCTION;
// #ifdef _DEBUG
// 	MOJING_FUNC_TRACE(g_APIlogger);
// #endif
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call DrawTexture befor Init! InitStatus = " << pStatus->GetInitStatus());
		return false;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
		if (pRender == NULL)
		{
			MOJING_ERROR(g_APIlogger, "Render with out Mojing Word!!");
			return false;
		}

		if (pStatus->GetVerfiyStatus() != VERIFY_OK)
		{
			pRender->SetOverlayTextureID(0 , 0);
			pRender->SetEyeTexID(0, 0);
			//			pRender->SetLeftEyeTexID(0);
		}
		else
		{
			pRender->SetOverlayTextureID(iLeftLayerTextureID , iRightLayerTextureID);
			pRender->SetEyeTexID(nLeftEyeTextureID, nrightEyeTextureID);
		}

		return pRender->WarpToScreen();
	}
	return false;
}

void MojingSDK_SetImageYOffset(float fYOffset)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	MOJING_TRACE(g_APIlogger, "Set YOffset = " << fYOffset);
	Manager::GetMojingManager()->GetDistortion()->SetYOffset(fYOffset);
	MojingRenderBase::SetModify();
}

String MojingSDK_GetUserSettings()
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	String strRet = "";
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call MojingSDK_GetUserSettings befor Init! InitStatus = " << pStatus->GetInitStatus());
		return "";
	}
	UserSettingProfile* pProfile = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile();
	if (pProfile)
	{
		JSON *pJson = pProfile->ToJson();
		if (pJson)
		{
			char * pJsonValue = pJson->PrintValue(0,false);
			strRet = pJsonValue;
			MJ_FREE(pJsonValue);
			pJson->Release();
		}
	}
	return strRet;
}

bool   MojingSDK_SetUserSettings(const char * sUserSettings)
{
	bool bRet = false;
	MOJING_FUNC_TRACE(g_APIlogger);
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger , "MojingSDK_SetUserSettings : " << sUserSettings);
#endif
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call MojingSDK_SetUserSettings befor Init! InitStatus = " << pStatus->GetInitStatus());
		return false;
	}
	JSON *pJson = JSON::Parse(sUserSettings);
	if (pJson)
	{
		UserSettingProfile* pProfile = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile();
		if (pProfile)
		{
			UserSettingProfile TempProfile = *pProfile;
// 			float fOldPPI = 0;
// 			bool bOldEnableScreenSize = TempProfile.GetEnableScreenSize();
// 			if (TempProfile.GetEnableScreenSize())
// 			{
// 				fOldPPI = TempProfile.GetScreenSize();
// 			}

			bRet = pProfile->FromJson(pJson);
			if (bRet)
			{
				MojingRenderBase::SetModify();
				bRet = pProfile->Save();
				if (!bRet)
				{
					MOJING_ERROR(g_APIlogger, "Can not save to file");
				}
				MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
				pDisplayParameters->UpdatePPIFromUserSetting();

			}
			else
			{
				*pProfile = TempProfile;// 解析失败，恢复初始值
				MOJING_ERROR(g_APIlogger, "Can not parse json");
			}
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "Can not get UserSettingProfile object");
		}
		pJson->Release();
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "INVALID JSON STRING");
	}
	return bRet;
}


int MojingSDK_GetSensorOrigin()
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	int sensorOrigin = SENSOR_ORIGIN_EXTERNAL_SDK;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call MojingSDK_GetSensorOrigin befor Init! InitStatus = " << pStatus->GetInitStatus());
		return sensorOrigin;
	}
	UserSettingProfile* pProfile = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile();
	if (pProfile)
	{
		if (MojingSDK_IsHDMWorking())
		{
			sensorOrigin = SENSOR_ORIGIN_EXTERNAL_DEVICE;
		}
		else
		{
			if (pProfile->GetSensorDataFromMJSDK())
			{
				if (pProfile->GetSensorDataFromJava())
					sensorOrigin = SENSOR_ORIGIN_LOCAL_JAVA;
				else
					sensorOrigin = SENSOR_ORIGIN_LOCAL_NATIVE;
			}
		}
	}
	return sensorOrigin;
}

bool   MojingSDK_SetSensorOrigin(int SensorOrigin)
{
	bool bRet = false;
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call MojingSDK_SetSensorOrigin befor Init! InitStatus = " << pStatus->GetInitStatus());
		return false;
	}
	if (SensorOrigin == SENSOR_ORIGIN_EXTERNAL_DEVICE)
	{
		MOJING_WARN(g_APIlogger, "Can not set External Device");
		return bRet;
	}

	UserSettingProfile* pProfile = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile();
	if (pProfile)
	{
		if (SensorOrigin == SENSOR_ORIGIN_EXTERNAL_SDK)
		{
			MOJING_TRACE(g_APIlogger, "Set sensor origin: not MojingSDK");
			pProfile->SetSensorDataFromMJSDK(false);
		}
		else
		{
			pProfile->SetSensorDataFromMJSDK(true);
			if (SensorOrigin == SENSOR_ORIGIN_LOCAL_JAVA)
			{
				MOJING_TRACE(g_APIlogger, "Set sensor origin: MojingSDK Java");
				pProfile->SetSensorDataFromJava(true);
			}
			else if (SENSOR_ORIGIN_LOCAL_NATIVE)
			{
				MOJING_TRACE(g_APIlogger, "Set sensor origin: MojingSDK Native");
				pProfile->SetSensorDataFromJava(false);
			}
		}
		pProfile->Save();
	}

	return bRet;
}


#ifdef USING_MINIDUMP
void MojingSDK_CheckCrashReport()
{
	char FindDir[256] = { 0 };
	//定义一个临时字符数组，存储目录
	strcpy(FindDir, MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath());
// 	if (FindDir[strlen(FindDir) - 1] != '/')
// 		strcat(FindDir , "/");

	DIR *dp;
	struct dirent *dirp;
	int iReportCount = 0;
	if ((dp = opendir(FindDir)) != NULL)
	{
		char FileName[256];

		while (iReportCount < 5 && (dirp = readdir(dp)) != NULL)
		{
			if (dirp->d_type == DT_REG)
			{
				char *pPos = strstr(dirp->d_name , ".zip");
				if (pPos)
				{
					strcpy(FileName, FindDir);
					strcat(FileName, "/");
					strcat(FileName, dirp->d_name);
					MOJING_TRACE(g_APIlogger, "Find .zip file , " << FileName);
					if (CrashReporter::GetCrashReporter(FileName)->DoReport())
						iReportCount++;
					else
						remove(FileName);
				}
				else
				{
					pPos = strstr(dirp->d_name, ".dmp");
					if (pPos)
					{
						strcpy(FileName, FindDir);
						strcat(FileName, "/");
						strcat(FileName, dirp->d_name);
						remove(FileName);
					}
				}
			}
		}
		closedir(dp);
	}
	
}
#endif

bool MojingSDK_GetInitSDK(void)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	return 	pStatus->IsMojingSDKEnbaled();
}

bool MojingSDK_GetStartTracker(void)
{
	ENTER_MINIDUMP_FUNCTION;
	MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
		return pStatus->GetTrackerStatus() == TRACKER_START;
	return false;
}

bool MojingSDK_GetInMojingWorld(void)
{
	ENTER_MINIDUMP_FUNCTION;
	//MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
		return MojingRenderBase::GetCurrentRender() != NULL;
	return false;
}

const char* MojingSDK_GetSDKVersion(void)
{
	ENTER_MINIDUMP_FUNCTION;
	//MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	return pStatus->GetSDKVersion();
}

const char* MojingSDK_GetGlasses(void)
{
	ENTER_MINIDUMP_FUNCTION;
	//MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{// 注意：这个GlassName是UUID格式
		return pStatus->GetGlassesName();
	}
	else
	{
		return "";
	}
}

bool MojingSDK_IsGlassesNeedDistortion(void)
{
	ENTER_MINIDUMP_FUNCTION;
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		size_t nLen = strlen(pStatus->GetGlassesName());
		if (nLen > 0)
		{
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Distortion* pDistortion = pManager->GetDistortion();
				return pDistortion->IsDistortionNeeded();
			}
		}
	}
	return false;
}

bool MojingSDK_IsGlassesNeedDistortionByName(const char * szGlassesName)
{
	bool bRet = false;
	ENTER_MINIDUMP_FUNCTION;
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
			if (pGlassesConfigProfile)
			{
				bRet = pGlassesConfigProfile->IsGlassesNeedDistortionByName(szGlassesName);
			}
		}
	}
	return bRet;
}
// 设置和获取瞳距，以米为单位
bool MojingSDK_DistortionSetLensSeparation(float fNewValue)
{
	ENTER_MINIDUMP_FUNCTION;
    MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
    {
        size_t nLen = strlen(pStatus->GetGlassesName());
        if (nLen > 0)
        {
            Manager* pManager = Manager::GetMojingManager();
            if (pManager)
            {
                Distortion* pDistortion = pManager->GetDistortion();
                if (pDistortion && pDistortion->IsDistortionNeeded())
                {
                    pDistortion->SetLensSeparation(fNewValue);
                    MojingRenderBase::SetModify();
                    return true;
                }
            }
        }
    }
    return false;
}

float MojingSDK_DistortionGetLensSeparation()
{
	ENTER_MINIDUMP_FUNCTION;
    MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
    {
        size_t nLen = strlen(pStatus->GetGlassesName());
        if (nLen > 0)
        {
            Manager* pManager = Manager::GetMojingManager();
            if (pManager)
            {
                Distortion* pDistortion = pManager->GetDistortion();
                if (pDistortion && pDistortion->IsDistortionNeeded())
                {
                    return pDistortion->GetLensSeparation();
                }
            }
        }
    }
    return 0;
}




unsigned int MojingSDK_GetEyeTexture(unsigned int eyeTextureType, unsigned int &iWidth,
	unsigned int &iHeight, unsigned int &format)
{
	ENTER_MINIDUMP_FUNCTION;
	//MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "Call GetEyeTexture befor Init! InitStatus = " << pStatus->GetInitStatus());
		return 0;
	}

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
		if (pRender == NULL)
		{
			MOJING_ERROR(g_APIlogger, "GetEyeTexture with out Mojing Word!!");
			return 0;
		}

		//unsigned iiWidth = iWidth;
		//unsigned iiHeight = iHeight;
		//GLenum iformat = format;
		unsigned id = pRender->GetEyeTextureId((EyeTextureType)eyeTextureType, iWidth, iHeight, format);
		if (id == 0)
		{
			MOJING_ERROR(g_APIlogger , " GetEyeTextureId = 0 ");
			id = pRender->GetEyeTextureId((EyeTextureType)eyeTextureType, iWidth, iHeight, format);
		}
		//iWidth = iiWidth;
		//iHeight = iiHeight;
		//format = iformat;
		return id;
	}
	return 0;
}

// bool MojingSDK_AddTextureLayout(unsigned int nLeftEyeTextureId, unsigned int nRightEyeTextureId, int order)
// {
// 	ENTER_MINIDUMP_FUNCTION;
// 	MojingRenderEffects* pRender = (MojingRenderEffects*)(MojingRenderBase::GetCurrentRender());
// 	if (pRender == NULL)
// 	{
// 		MOJING_ERROR(g_APIlogger, "AddTextureForBlend with out Mojing Word!!");
// 		return false;
// 	}
// 
// 	pRender->AddTextureLayout(nLeftEyeTextureId, nRightEyeTextureId, order);
// 	return true;
// }
// 
// bool MojingSDK_DeleteTextureLayout(int zorder)
// {
// 	ENTER_MINIDUMP_FUNCTION;
// 	MojingRenderEffects* pRender = (MojingRenderEffects*)(MojingRenderBase::GetCurrentRender());
// 	if (pRender == NULL)
// 	{
// 		MOJING_ERROR(g_APIlogger, "DeleteTextureLayout with out Mojing Word!!");
// 		return false;
// 	}
// 
// 	pRender->DeleteTextureLayout(zorder);
// 	return true;
// }

/************************************************************************/
/* 厂商-产品-镜片-APP管理                                               */
/************************************************************************/
String MojingSDK_GetManufacturerList(const char* strLanguageCodeByISO639)
{
	ENTER_MINIDUMP_FUNCTION;
	String strRet;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
			if (pGlassesConfigProfile)
			{
				strRet = pGlassesConfigProfile->GetManufacturerJson(strLanguageCodeByISO639);
			}
		}
	}
//	MOJING_TRACE(g_APIlogger, "MojingSDK_GetManufacturerList = " << strRet);
	return strRet;
}
String MojingSDK_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639)
{
	ENTER_MINIDUMP_FUNCTION;
	String strRet;

	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
			if (pGlassesConfigProfile)
			{
				strRet = pGlassesConfigProfile->GetProductJson(strManufacturerKey, strLanguageCodeByISO639);
			}
		}
	}
//	MOJING_TRACE(g_APIlogger, "MojingSDK_GetProductList = " << strRet);
	return strRet;
}
String MojingSDK_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639)
{
	ENTER_MINIDUMP_FUNCTION;
	String strRet;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
			if (pGlassesConfigProfile)
			{
				strRet = pGlassesConfigProfile->GetGlassJson(strProductKey, strLanguageCodeByISO639);
			}
		}
	}
//	MOJING_TRACE(g_APIlogger, "MojingSDK_GetGlassList = " << strRet);
	return strRet;
}
String MojingSDK_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639)
{
	ENTER_MINIDUMP_FUNCTION;
	String strRet;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
			if (pGlassesConfigProfile)
			{
				strRet = pGlassesConfigProfile->GetGlassInfoJson(strGlassKey, strLanguageCodeByISO639);
			}
		}
	}
//	MOJING_TRACE(g_APIlogger, "MojingSDK_GetGlassInfo = " << strRet);
	return strRet;
}


String MojingSDK_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode)
{
	ENTER_MINIDUMP_FUNCTION;
	String strRet;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		MojingProfileKey KeyA , KeyB;
		bool bAOK = KeyA.SetString(strProductQRCode);
		bool bBOK = KeyB.SetString(strGlassQRCode);
		if (bAOK || bBOK)
		{
			KeyA.UnionKey(KeyB);
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				GlassesConfigProfileV2* pGlassesConfigProfile = pManager->GetParameters()->GetGlassesConfigProfile();
				if (pGlassesConfigProfile)
				{
					pGlassesConfigProfile->MakeFinalKey(KeyA);
					JSON *pError = pGlassesConfigProfile->CheckIsFinalKey(KeyA);
					if (pError == NULL)
					{
						strRet = KeyA.GetString();
					}
					else
					{
						char * pErrorString = pError->PrintValue(0, false);
						strRet = pErrorString;
						MJ_FREE(pErrorString);
					}
				}
			}
		}
		else
		{
			strRet = "{\"ERROR\":\"INVALID KEY\"}";
		}
		
	}
	return strRet;
}

#define NEAR	1e-5
#define FAR		1e9
#define CHECK_CENTER(C , LT , BR) fmin(LT.x , BR.x) < C.x &&\
	fmax(LT.x, BR.x) > C.x &&\
	fmin(LT.y, BR.y) < C.y &&\
	fmax(LT.y, BR.y) > C.y

// 不适用于超大平面
int MojingSDK_Math_SelectRectByDirectional(Matrix4f Direction, int iRectCounts, float * pv3TopLeft, float * pv3BottomRight)
{
	// 1 初始化 摄像机投影
	static const Matrix4f m4VP = Matrix4f::LookAtLH(Vector3f(0, 0, 0), Vector3f(0, 0, -1), Vector3f(0, 1, 0)) * Matrix4f::PerspectiveLH(90, 1, NEAR, FAR);
	Matrix4f m4MVP = Direction * m4VP;
	Matrix4f m4MVP_T = m4MVP.Inverted();
	Matrix4f m4Direction_T = Direction.Inverted();

	for (int i = 0; i < iRectCounts; i++)
	{
		Vector3f v2TopLeftT = m4Direction_T.Transform(Vector3f(pv3TopLeft[3 * i], pv3TopLeft[3 * i + 1], pv3TopLeft[3 * i + 2]));
		Vector3f v2BottomRightT = m4Direction_T.Transform(Vector3f(pv3BottomRight[3 * i], pv3BottomRight[3 * i + 1], pv3BottomRight[3 * i + 2]));
		if (v2TopLeftT.z < 0 && v2BottomRightT.z < 0)
		{
			if (CHECK_CENTER(Vector2f(0, 0), v2TopLeftT, v2BottomRightT))
			{
				return i;
			}
		}
	}
	return -1;
}
// 计算射线到平面矩形(平行于XoY平面，暨Z恒定)的焦点坐标
Vector2f MojingSDK_Math_DirectionalRadiaInRect(Matrix4f Direction, Vector2f v2TopLeft, Vector2f v2BottomRight, float fZ)
{
	Vector2f vRet(-1, -1);
	// 在初始视角为Z轴负方向的情况下，要求Direction.M[2][2]的符号和fZ的符号不能不同
	if (Direction.M[2][2] * fZ < 0)
	{// 因为只有旋转，没有平移，而且只是转(0 , 0 , Z)，所以可以简写成这样
		Vector3f V(0, 0, fZ / Direction.M[2][2]);
		Vector3f V2 = Direction.Transform(V);
		if (CHECK_CENTER(V2, v2TopLeft, v2BottomRight))
		{// 得到交点位置
			vRet = Vector2f(fabs(V2.x - v2TopLeft.x), fabs(V2.y - v2TopLeft.y));
		}
	}
	return vRet;
}

void MojingSDK_Math_DirectionalInScreen(float* fArray)
{
	Quatf originView(-fArray[0], -fArray[1], -fArray[2], fArray[3]);
	//LOGE("originQuat %f, %f, %f, %f", originView.x, originView.y, originView.z, originView.w);
	Quatf currentView(fArray[4], fArray[5], fArray[6], fArray[7]);
	//LOGE("CurrentQuat %f, %f, %f, %f", currentView.x, currentView.y, currentView.z, currentView.w);

	float yaw, pitch, roll;
	Quatf viewCorrected = originView * currentView;
	//LOGE("viewCorrected: %f, %f, %f, %f", viewCorrected.x, viewCorrected.y, viewCorrected.z, viewCorrected.w);
	viewCorrected.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);
	//LOGE("yaw %f, pitch %f, roll %f", yaw, pitch, roll);
	fArray[8] = cos(yaw + PI / 3);
	fArray[9] = sin(pitch + PI / 6 * 5);
	// Near为近平面。近平面定义为长、宽为1的平面
	//float fNear = 0.5 / tan(fFOV / 2 * PI / 180);
	//fNear / Direction.M[2][2] = 原点沿射线到近平面的距离
	//Vector3f V3(0, 0, fNear / Direction.M[2][2]);
	//Vector3f V3_A = Direction.Transform(V3);
	//Vector2f Ret(V3_A.x + 0.5, V3_A.y + 0.5);

	//return Ret;
// 
// 	Matrix4f m;
// 	Quatf q(m);
// 	Vector3f v;
// 	Vector3f v2 = v * q;
}

const char* MojingSDK_GetCpuName()
{
	ENTER_MINIDUMP_FUNCTION;
	return MojingPlatformBase::GetPlatform()->GetCPUName();
}

const char* MojingSDK_GetGpuName()
{
	ENTER_MINIDUMP_FUNCTION;
	return MojingPlatformBase::GetPlatform()->GetGPUName();
}

#ifndef DO_NOT_USING_TEXTURE_BACKER
bool MojingSDK_StartBacker(int width, int height, int texID /* = 0表示由低层生成TextureID*/)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	if (NULL == g_pTexBacker)
	{
		g_pTexBacker = new TextureBacker();
	}
	else
	{
		g_pTexBacker->StopBacker();
	}
	g_pTexBacker->StartBacker(width, height, texID);
	return true;
}

bool MojingSDK_GetBackerTexID(int* pTexID)
{
	if (NULL == g_pTexBacker)
	{
		return false;
	}
	g_pTexBacker->GetTexID(pTexID);
	return true;
}

bool MojingSDK_StopBacker()
{
	if (NULL == g_pTexBacker)
	{
		return false;
	}
	g_pTexBacker->StopBacker();
    return true;
}
// #define  _DEBUG_BACKER
bool MojingSDK_BackerTexture(int texID, int x, int y, int width, int height, int desX, int desY)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	if (NULL == g_pTexBacker)
	{
		MOJING_TRACE(g_APIlogger, "MojingSDK_BackerTexture: return ");
		return false;
	}

	MOJING_TRACE(g_APIlogger, "MojingSDK_BackerTexture");
	static int framc = 1;
	//mt.drawTexToFBO(texID, mpv);
	g_pTexBacker->BackerTexture(texID, x, y, width, height, desX, desY);

#ifdef _DEBUG_BACKER
	char filepath[256];
	//if (framcount < 6)
	{
		framc++;
		sprintf(filepath, "/sdcard/MojingSDK/test%d.bmp", framc);
		pTexBacker->GetFbo()->SaveToBmp(filepath, 0, 0, pTexBacker->GetFbo()->GetTextureId(), pTexBacker->GetFbo()->GetWidth(), pTexBacker->GetFbo()->GetHeight(), 3);
		//pTexBacker->GetFbo()->SaveToBmp(filepath, 0, 0, texID, width, height, 3);
		//pTexBacker->GetFbo()->SaveToBmp(filepath, 0,0, )
		//mt.saveFBOToFile(filepath);
	}
#endif // _DEBUG

	return true;
}
#endif // DO_NOT_USING_TEXTURE_BACKER

bool MojingSDK_IsLowPower()
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			return pManager->GetParameters()->GetSensorParameters()->GetIsLowPower();
		}
	}
	return false;
}

void MojingSDK_SetHDMWorking(bool bHDMWorking)
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			pManager->GetParameters()->SetHDMWorking(bHDMWorking);
		}
	}
}

bool MojingSDK_IsHDMWorking()
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		Manager* pManager = Manager::GetMojingManager();
		if (pManager)
		{
			return pManager->GetParameters()->GetHDMWorking();
		}
	}
	return false;
}

#ifdef MJ_OS_ANDROID
int MojingSDK_GetSocketPort()
{
	int s = -1, port = 0;

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		MojingSensorParameters * pSensorParameters = pManager->GetParameters()->GetSensorParameters();
		if (pSensorParameters->GetSocket() == -1)
		{
			struct sockaddr_in si_me;
			socklen_t slen_me = sizeof(si_me);

			// create a UDP socket
			if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
			{
				__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "create socket failed");
			}
			else
			{
				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "create socket OK");

				// zero out the structure
				memset((char *) &si_me, 0, sizeof(si_me));

				si_me.sin_family = AF_INET;
				si_me.sin_port = htons(0);
				si_me.sin_addr.s_addr = htonl(INADDR_ANY);

				// bind socket to port
				if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
				{
					__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "bind socket failed");
				}
				else
				{
					getsockname(s, (struct sockaddr*)&si_me, &slen_me);

					__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "bind socket OK port=%d", ntohs(si_me.sin_port));

					// set timeout
					struct timeval tv;
					tv.tv_sec = 0;
					tv.tv_usec = 100000;
					if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
					{
						__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "setsockopt failed");
					}
					else
					{
						port = ntohs(si_me.sin_port);
					}
				}
			}

			if (s != -1 && port == 0)
			{
				close(s);
				s = -1;
			}

			pSensorParameters->SetSocket(s);
			pSensorParameters->SetPort(port);
		}
		else
		{
			__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "socket already init");
			port = pSensorParameters->GetPort();
		}
	}
	else
	{
		__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "Manager not init");
	}
	return port;
}
#endif
/*
bool MojingSDK_Device_StartTracker(int iID)
{
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		return pManager->GetControlTracker()->StartControlTracker(iID);
	}
	return false;
}

void MojingSDK_Device_StopTracker(int iID)
{
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		pManager->GetControlTracker()->StopControlTracker(iID);
	}
}
*/

#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
// 获取已经连接的设备上的按键列表的掩码，返回长度为32的int数组。分别表示按键状态码的第0位到第31位表示的键值。
int MojingSDK_Device_GetKeymask(int iID, int *pKeyMask)
{
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_Device_GetKeymask before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return 0;
	}
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		return pManager->GetControlTracker()->GetKeymask(iID, pKeyMask);
	}

	return 0;
}

// 获取设备的姿态信息和其他传感器信息,返回值为采样的时间。
float MojingSDK_Device_GetCurrentPoaseInfo(int iID/*设备ID*/,
	float *pQuart/*四元数表示的旋转，依次为XYZW*/,
	float *pAngularAccel/*角加速度，依次为XYZ*/,
	float *pLinearAccel/*线加速度，依次为XYZ*/,
	float *pPosition,/*设备的空间位置，以米为单位，默认是0,0,0。*/
	unsigned int *pKeystatus/*设备上的按键状态，默认是0表示没有按键被按下*/)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_Device_GetCurrentPoaseInfo before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return 0;
	}
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		if (pPosition)
		{
			pPosition[0] = pPosition[1] = pPosition[2] = 0;
		}
		if (pKeystatus)
		{
			*pKeystatus = 0;
		}
		return pManager->GetControlTracker()->GetControlCurrentPose(iID, pQuart, pAngularAccel, pLinearAccel);
	}

	return 0;
}

// 获取设备Reset的姿态信息和其他传感器信息,返回值为Reset的时间。
float MojingSDK_Device_GetFixPoaseInfo(int iID/*设备ID*/,
	float *pQuart/*四元数表示的旋转，依次为XYZW*/,
	float *pAngularAccel/*角加速度，依次为XYZ*/,
	float *pLinearAccel/*线加速度，依次为XYZ*/,
	float *pPosition/*设备的空间位置，以米为单位，默认是0,0,0。*/)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_Device_GetFixPoaseInfo before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return 0;
	}
	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		if (pPosition)
		{
			pPosition[0] = pPosition[1] = pPosition[2] = 0;
		}
		return pManager->GetControlTracker()->GetControlFixPose(iID, pQuart, pAngularAccel, pLinearAccel);
	}

	return 0;
}

// 获取设备Reset的姿态信息和其他传感器信息,返回值为Reset的时间。
float MojingSDK_Device_GetControlFixCurrentInfo(int iID/*设备ID*/,
    float *pQuart/*四元数表示的旋转，依次为XYZW*/,
    float *pAngularAccel/*角加速度，依次为XYZ*/,
    float *pLinearAccel/*线加速度，依次为XYZ*/,
    float *pPosition/*设备的空间位置，以米为单位，默认是0,0,0。*/,
    unsigned int *pKeystatus/*设备上的按键状态，默认是0表示没有按键被按下*/)
{
    //MOJING_FUNC_TRACE(g_APIlogger);
	mj_Initialize();
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (!pStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_Device_GetControlFixCurrentInfo before SDK init! InitStatus = " << pStatus->GetInitStatus());
		return 0;
	}
    Manager* pManager = Manager::GetMojingManager();
    if (pManager)
    {
		if (pPosition)
		{
			pPosition[0] = pPosition[1] = pPosition[2] = 0;
		}
		if (pKeystatus)
		{
			*pKeystatus = 0;
		}
        return pManager->GetControlTracker()->GetControlFixCurrentPose(iID, pQuart, pAngularAccel, pLinearAccel);
    }

    return 0;
}

void MojingSDK_Device_GetFixScore(int* pStatus, int* pScore)
{
	mj_Initialize();
	MojingSDKStatus *pSDKStatus = MojingSDKStatus::GetSDKStatus();
	if (!pSDKStatus->IsMojingSDKEnbaled())
	{
		MOJING_ERROR(g_APIlogger, "MojingSDK_Device_GetFixScore before SDK init! InitStatus = " << pSDKStatus->GetInitStatus());
		return;
	}
    Manager* pManager = Manager::GetMojingManager();
    if (pManager)
    {
        pManager->GetControlTracker()->GetFixScore(pStatus, pScore);
    }
    return;
}
#endif

#ifdef _DEBUG
/************************************************************************/
/* 以下为私有代码，给SDK用的代码放到前面去			                    */
/************************************************************************/
// 设置和获取K值，Get函数的返回值为畸变引导点的数量
bool MojingSDKPrivate_DistortionSetParamet(int iSegment, float *KR, float *KG, float *KB)
{
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		size_t nLen = strlen(pStatus->GetGlassesName());
		if (nLen > 0)
		{
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Distortion* pDistortion = pManager->GetDistortion();
				if (pDistortion && pDistortion->IsDistortionNeeded())
				{
					pDistortion->SetDistortionParamet(iSegment,false, KR, KG, KB);
					MojingRenderBase::SetModify();
					return true;
				}
			}
		}
	}
	return false;
}
int MojingSDKPrivate_DistortionGetParamet(float *KR, float *KG, float *KB)
{
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		size_t nLen = strlen(pStatus->GetGlassesName());
		if (nLen > 0)
		{
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Distortion* pDistortion = pManager->GetDistortion();
				if (pDistortion && pDistortion->IsDistortionNeeded())
				{
					return pDistortion->GetDistortionParamet(KR,KG,KB);
				}
			}
		}
	}
	return 0;
}

bool MojingSDKPrivate_DistortionSaveParamet()
{
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->IsMojingSDKEnbaled())
	{
		size_t nLen = strlen(pStatus->GetGlassesName());
		if (nLen > 0)
		{
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Distortion* pDistortion = pManager->GetDistortion();
				if (pDistortion && pDistortion->IsDistortionNeeded())
				{
					char szFilePathName[512];
					strcpy(szFilePathName , MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath());
					if (szFilePathName[strlen(szFilePathName) - 1] != '/')
						strcat(szFilePathName , "/");
					strcat(szFilePathName, "GlassesConfig.dat");
					return pDistortion->SaveToFile(szFilePathName);
				}
			}
		}
	}
	return false;

}
bool MojingSDKPrivate_DistortionResetParamet()
{
	ENTER_MINIDUMP_FUNCTION;
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
		if (pStatus->IsMojingSDKEnbaled())
		{
			size_t nLen = strlen(pStatus->GetGlassesName());
			if (nLen > 0)
			{
				Manager* pManager = Manager::GetMojingManager();
				if (pManager)
				{
					Distortion* pDistortion = pManager->GetDistortion();
					if (pDistortion && pDistortion->IsDistortionNeeded())
					{
						char szFilePathName[512];
						strcpy(szFilePathName, MojingPlatformBase::GetPlatform()->GetPacketProfilePath());
						if (szFilePathName[strlen(szFilePathName) - 1] != '/')
							strcat(szFilePathName, "/");
						strcat(szFilePathName, "GlassesConfig.dat");
						remove(szFilePathName);
						GlassesConfigProfile::UpdateFromProfile(MojingSDK_GetGlasses(), MojingPlatformBase::GetPlatform()->GetPacketProfilePath()); 
						MojingRenderBase::SetModify();
						//return pDistortion->Load();
					}
				}
			}
		}
		return false;
}

/************************************************************************/
/* 以上为私有代码，给SDK用的代码放到前面去			                    */
/************************************************************************/
#endif



