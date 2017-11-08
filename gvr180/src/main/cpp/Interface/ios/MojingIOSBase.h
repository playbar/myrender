//
//  MojingIOSBase.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/7/21.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_MojingIOSBase_h
#define MojingSDK_MojingIOSBase_h

enum Enum_AppDir_Type
{
    APP_DIR_HOME,
    APP_DIR_APP,
    APP_DIR_DOCUMENTS,
    APP_DIR_LIBRARY,
    APP_DIR_CACHES,
    APP_DIR_TMP
};

const char* GetUserID();
const char* GetOpenUDID();
const char* GetVerdorUUID();
const char* GetUUID();
const char* GetMobileBrand();
const char* GetCPUArchName();
const char* GetMoblieMAC();
const char* GetMobileSerialNumber();
const char* GetMobileSystemVersion();
const char* GetMobileModelName();
const char* GetMobileSimpleModelName();
const char* GetMobileInnerModelName();

bool GetMobileScreenSize(int &nWidth, int &nHeight, float &xDpi, float &yDpi, float& scale, bool bIsUnity = false);

const char* GetCurPackageName();
const char* GetCurAppName();
const char* GetCurAppVersion();

const char* GetIOSAppPrivateDir(int nType);
void CreateIOSDir(const char* path);
const char* GetAppAssetsFilePath(const char* filename);
const char* GetSDKAssetsFilePath(const char* filename);
const char* GetSDKAssetsDirPath();

const char* GetGLVersion();
const char* GetGLESVersion();

#endif
