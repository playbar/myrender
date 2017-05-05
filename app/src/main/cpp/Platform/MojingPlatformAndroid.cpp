#include "MojingPlatformAndroid.h"
#ifdef MJ_OS_ANDROID
#include "../Parameters/MojingDisplayParameters.h"
#include "../Parameters/MojingDeviceParameters.h"
#include "../3rdPart/minizip/unzip.h"
#include "Android/MojingAndroidDeviceInfo.h"

namespace Baofeng
{
	namespace Mojing
	{
		MojingPlatformAndroid::MojingPlatformAndroid(JNIEnv *pEnv, jobject DisplayMetrics)
		{
			SetDisplayMetrics(DisplayMetrics);
			SetEnv(pEnv);
			
			char StrOS[64] = "";
			String OSType = "Android ";
			AndroidProperties::GetAndroidVersion(StrOS);
			if (*StrOS)
				SetOSString(OSType + StrOS);
			else
				SetOSString(OSType + "Unknown");
			
		}


		MojingPlatformAndroid::~MojingPlatformAndroid()
		{
		}

		MojingDisplayParameters * MojingPlatformAndroid::CreateDisplayParameters()
		{
			MojingDisplayParameters * pRet = new MojingDisplayParameters;

			JNIEnv* env_ = GetEnv();
			jclass classDisplayMetrics = env_->FindClass("android/util/DisplayMetrics");
			jfieldID idDisplayMetrics_widthPixels = env_->GetFieldID(classDisplayMetrics, "widthPixels", "I");
			jfieldID idDisplayMetrics_heightPixels = env_->GetFieldID(classDisplayMetrics, "heightPixels", "I");
			jfieldID idDisplayMetrics_density = env_->GetFieldID(classDisplayMetrics, "density", "F");
			jfieldID idDisplayMetrics_densityDpi = env_->GetFieldID(classDisplayMetrics, "densityDpi", "I");
			jfieldID idDisplayMetrics_scaledDensity = env_->GetFieldID(classDisplayMetrics, "scaledDensity", "F");
			jfieldID idDisplayMetrics_xdpi = env_->GetFieldID(classDisplayMetrics, "xdpi", "F");
			jfieldID idDisplayMetrics_ydpi = env_->GetFieldID(classDisplayMetrics, "ydpi", "F");

			jobject displayMetrics = GetDisplayMetrics();
			int iWidth = env_->GetIntField(displayMetrics, idDisplayMetrics_widthPixels);
			int iHeight = env_->GetIntField(displayMetrics, idDisplayMetrics_heightPixels);

			if (idDisplayMetrics_widthPixels)
				pRet->SetScreenWidth(iWidth);
			if (idDisplayMetrics_heightPixels)
				pRet->SetScreenHeight(iHeight);
// 			if (idDisplayMetrics_density)
// 				pRet->SetDensity(env_->GetFloatField(displayMetrics, idDisplayMetrics_density));
// 			if (idDisplayMetrics_densityDpi)
// 				pRet->SetDensityDPI(env_->GetIntField(displayMetrics, idDisplayMetrics_densityDpi));
// 			if (idDisplayMetrics_scaledDensity)
// 				pRet->SetScaledDensity(env_->GetFloatField(displayMetrics, idDisplayMetrics_scaledDensity));
			if (idDisplayMetrics_xdpi)
				pRet->SetXdpi(env_->GetFloatField(displayMetrics, idDisplayMetrics_xdpi));
			if (idDisplayMetrics_ydpi)
				pRet->SetYdpi(env_->GetFloatField(displayMetrics, idDisplayMetrics_ydpi));

			pRet->SetPPI(0);
			//pRet->SetScreenMeters();
			classDisplayMetrics = 0;
			return pRet;
		}

		MojingDeviceParameters * MojingPlatformAndroid::CreateDeviceParameters()
		{
			MojingDeviceParameters *pRet = new MojingDeviceParameters;

			JNIEnv* env = GetEnv();
			jclass	BuildClass;
			BuildClass = env->FindClass("android/os/Build");
			char * pValue;
			pRet->SetBrand(pValue = GetBuildFieldString(env, BuildClass, "BRAND"));			delete pValue;
			pRet->SetDevice(pValue = GetBuildFieldString(env, BuildClass, "DEVICE"));       delete pValue;
			pRet->SetDisplay(pValue = GetBuildFieldString(env, BuildClass, "DISPLAY"));      delete pValue;
			pRet->SetFingerprint(pValue = GetBuildFieldString(env, BuildClass, "FINGERPRINT"));  delete pValue;
			pRet->SetHardware(pValue = GetBuildFieldString(env, BuildClass, "HARDWARE"));     delete pValue;
			pRet->SetHost(pValue = GetBuildFieldString(env, BuildClass, "HOST"));         delete pValue;
			pRet->SetID(pValue = GetBuildFieldString(env, BuildClass, "ID"));           delete pValue;
			pRet->SetModel(pValue = GetBuildFieldString(env, BuildClass, "MODEL"));        delete pValue;
			pRet->SetProduct(pValue = GetBuildFieldString(env, BuildClass, "PRODUCT"));    delete pValue;
			pRet->SetSerial(pValue = GetBuildFieldString(env, BuildClass, "SERIAL"));       delete pValue;
			pRet->SetTags(pValue = GetBuildFieldString(env, BuildClass, "TAGS"));         delete pValue;
			pRet->SetType(pValue = GetBuildFieldString(env, BuildClass, "TYPE"));         delete pValue;
			BuildClass = 0;
			return pRet;
		}

		char * MojingPlatformAndroid::GetBuildFieldString(JNIEnv *env, jclass	BuildClass, const char * name)
		{
			jfieldID field = env->GetStaticFieldID(BuildClass, name, "Ljava/lang/String;");

			// get reference to the string
			jstring jstr = (jstring)env->GetStaticObjectField(BuildClass, field);

			jboolean isCopy;
			const char * cstr = env->GetStringUTFChars(jstr, &isCopy);

			char* szRet = new char[strlen(cstr) + 1];
			strcpy(szRet, cstr);

			env->ReleaseStringUTFChars(jstr, cstr);


			return szRet;
		}

#if 0
		String MojingPlatformAndroid::LoadProfileString(String szProfilePath, String szProfileName)
		{
			String strRet;
			unzFile packageZipFile = unzOpen(szProfilePath);
			if (packageZipFile)
			{
				if (unzGoToFirstFile(packageZipFile) == UNZ_OK)
				{
				//	LOG("FilesInPackage", "Files in package:");
					do
					{
						unz_file_info fileInfo;
						char fileName[512];
						if (unzGetCurrentFileInfo(packageZipFile, &fileInfo, fileName, sizeof(fileName), NULL, 0, NULL, 0) == UNZ_OK)
						{
				//			LOG("FilesInPackage", "%s", fileName);

				//			if (fileInfo.flag)
						}
					} while (unzGoToNextFile(packageZipFile) == UNZ_OK);
				}
			}
		}
#endif
	};// end of namespace mojing
};// end of namespace baofeng
#endif