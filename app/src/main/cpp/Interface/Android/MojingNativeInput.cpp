#include "MojingNativeInput.h"
#include "../../Base/MojingTypes.h"
#ifdef MJ_OS_ANDROID
#include "../../Base/MojingString.h"
#include <map>
#include <vector>
#include <../../Base/MojingThreads.h>
#include "../../Profile/UnrealJoystickProfile.h"
#include <dlfcn.h>
#include <android/input.h>
#include <android/keycodes.h>
using namespace Baofeng::Mojing;

Mutex *g_pMojingInputDeviceLock = NULL;
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#ifndef GetAxesType
// 这是一个R19版NDK才有的函数
typedef float(*FP_GetAxesType)(const AInputEvent*, int32_t axis, size_t pointer_index);
FP_GetAxesType g_fpGetAxes = NULL;
#endif

static const int32_t g_AxisList[] =
{
	AMOTION_EVENT_AXIS_X,
	AMOTION_EVENT_AXIS_Y,
	AMOTION_EVENT_AXIS_Z,
	AMOTION_EVENT_AXIS_RX,
	AMOTION_EVENT_AXIS_RY,
	AMOTION_EVENT_AXIS_RZ,
	//These are DPAD analogs
	AMOTION_EVENT_AXIS_HAT_X,
	AMOTION_EVENT_AXIS_HAT_Y,
};





struct __tagDevice
{
	int m_iID;	// 设备ID
	String m_strDeviceName;	// 设备名称
	MOJING_INPUT_DEVICE m_DeviceType;	// 是否是
	bool m_bChecked;
	time_t m_tmLastCheckTime;
};
typedef std::map <int, __tagDevice> DeviceMap;
DeviceMap g_AllDeviceMap;
extern void mj_Initialize();

int g_Mojing2Number = -1;
int g_Mojing3Number = -1;
int g_Mojing4Number = -1;

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeSetMojing2Number(JNIEnv *env, jclass, jint iNumber)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	g_Mojing2Number = iNumber;
}


JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeSetMojing3Number(JNIEnv *env, jclass, jint iNumber)
{
    MOJING_FUNC_TRACE(g_APIlogger);
    g_Mojing3Number = iNumber;
}


JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeSetMojing4Number(JNIEnv *env, jclass, jint iNumber)
{
    MOJING_FUNC_TRACE(g_APIlogger);
    g_Mojing4Number = iNumber;
}



JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeCleanDeviceMap(JNIEnv *, jclass)
{

	if (g_pMojingInputDeviceLock == NULL)
		g_pMojingInputDeviceLock = new Mutex;
	// g_pMojingInputDeviceLock->DoLock();
	g_AllDeviceMap.clear();
	// g_pMojingInputDeviceLock->Unlock();
}
					   
JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeBeginUpdateDeviceMap(JNIEnv *, jclass)
{// 清除所有设备的更新状态
	mj_Initialize();
	// g_pMojingInputDeviceLock->DoLock();
	if (g_pMojingInputDeviceLock == NULL)
		g_pMojingInputDeviceLock = new Mutex;
	for (DeviceMap::iterator it = g_AllDeviceMap.begin(); it != g_AllDeviceMap.end(); it++)
	{
		it->second.m_bChecked = false;
	}
	// g_pMojingInputDeviceLock->Unlock();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeEndUpdateDeviceMap(JNIEnv *, jclass)
{// 清除所有未更新设备的记录
	mj_Initialize();
	MOJING_FUNC_TRACE(g_APIlogger); 
	if (g_pMojingInputDeviceLock == NULL)
		g_pMojingInputDeviceLock = new Mutex;
	// g_pMojingInputDeviceLock->DoLock();
	
	std::vector<int> EraseID;
	for (DeviceMap::iterator it = g_AllDeviceMap.begin(); it != g_AllDeviceMap.end(); it++)
	{
		if (!it->second.m_bChecked)
		{
			EraseID.push_back(it->first);
			MOJING_TRACE(g_APIlogger, "Remove Device , ID = " << it->first << " , Name = \"" << it->second.m_strDeviceName.ToCStr() << "\"");
		}

        if (MID_JOYSTICK_MJ_MAYBE == it->second.m_DeviceType)
        {
            if (g_Mojing4Number == 1)
            {
                it->second.m_DeviceType = MID_JOYSTICK_MJ4;
            }
            else if (g_Mojing3Number == 1)
            {
                it->second.m_DeviceType = MID_JOYSTICK_MJ3;
            }
            else if (g_Mojing2Number == 1)
            {
                it->second.m_DeviceType = MID_JOYSTICK_MJ2;
            }
        }
	}

	for (int i = 0; i < (int)EraseID.size(); i++)
	{
		g_AllDeviceMap.erase(EraseID[i]);
	}
	// g_pMojingInputDeviceLock->Unlock();
}
std::map<String, int> g_devmap;
JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeAddDeviceToMap(JNIEnv *env, jclass, jint jiDeviceID, jstring jsDeviceName)
{
	
	if (g_pMojingInputDeviceLock == NULL)
		g_pMojingInputDeviceLock = new Mutex;
	
	if (g_devmap.size() == 0)
	{
#if 1
        g_devmap.insert(std::map<String, int>::value_type("Mojing", MID_JOYSTICK_MJ2));
        g_devmap.insert(std::map<String, int>::value_type("Mojing3", MID_JOYSTICK_MJ3));
        g_devmap.insert(std::map<String, int>::value_type("Mojing4-A", MID_JOYSTICK_MJ4));
        g_devmap.insert(std::map<String, int>::value_type("mojing", MID_JOYSTICK_MJ4));
        g_devmap.insert(std::map<String, int>::value_type("mojing5_BL", MID_JOYSTICK_MJ5));
        g_devmap.insert(std::map<String, int>::value_type("mojing-motion", MID_JOYSTICK_MJ_Motion));
#endif
        UnrealJoystickProfile joystickprofile;
        joystickprofile.getDeviceMap(g_devmap);

#if 0
        std::map<String, int>::iterator it = g_devmap.begin();
        for (; it != g_devmap.end(); ++it)
        {
            MOJING_TRACE(g_APIlogger, "DeviceMap item:" << it->first << "  value:" << it->second);
        }
        MOJING_TRACE(g_APIlogger, "getDeviceMap map size:" << g_devmap.size());
#endif

	}
	
	const char * szDeviceName = env->GetStringUTFChars(jsDeviceName, 0);
	g_AllDeviceMap[jiDeviceID].m_iID = jiDeviceID;
	g_AllDeviceMap[jiDeviceID].m_strDeviceName = szDeviceName;
	g_AllDeviceMap[jiDeviceID].m_bChecked = true;
	g_AllDeviceMap[jiDeviceID].m_tmLastCheckTime = time(NULL);
	
    if (g_devmap.size() == 0)
	{
	
        if (0 == strcmp("Mojing", szDeviceName))
        {
            g_AllDeviceMap[jiDeviceID].m_DeviceType = MID_JOYSTICK_MJ2;
        }
        else if (0 == strcmp("Mojing3", szDeviceName))
        {
            g_AllDeviceMap[jiDeviceID].m_DeviceType = MID_JOYSTICK_MJ3;
        }
        else if (0 == strcmp("Mojing4-A", szDeviceName) || 0 == strcmp("mojing", szDeviceName))
        {
            g_AllDeviceMap[jiDeviceID].m_DeviceType = MID_JOYSTICK_MJ4;
        }	
        else if (0 == strcmp("mojing5_BL", szDeviceName))
        {
            g_AllDeviceMap[jiDeviceID].m_DeviceType = MID_JOYSTICK_MJ5;
        }
        else
        {
            g_AllDeviceMap[jiDeviceID].m_DeviceType = MID_NONE;
        }
    }
    else
    {
        if (g_devmap.end() == g_devmap.find(szDeviceName))
		{
            g_AllDeviceMap[jiDeviceID].m_DeviceType = MID_NONE;
		}
        else
        {
            g_AllDeviceMap[jiDeviceID].m_DeviceType = (MOJING_INPUT_DEVICE)g_devmap[szDeviceName];
        }
        
    }

    if (0 == strcmp("Broadcom Bluetooth HID", szDeviceName) || 0 == strcmp("MTK BT HID", szDeviceName))
    {
        g_AllDeviceMap[jiDeviceID].m_DeviceType = MID_JOYSTICK_MJ_MAYBE;
    }
	env->ReleaseStringUTFChars(jsDeviceName, szDeviceName);

	// g_pMojingInputDeviceLock->Unlock();
}

typedef void(*FP_OnMojingKeyEventCB)(int32_t deviceId, int32_t buttonId, bool buttonDown);
typedef void(*FP_OnMojingAxisEventCB)(int32_t deviceId, int32_t axisId, float axisValue);
FP_OnMojingKeyEventCB g_fpOnMojingKeyEventCB = NULL;
FP_OnMojingAxisEventCB g_fpOnMojingAxisEventCB = NULL;

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_OnKeyEvent(JNIEnv *env, jclass, jstring jsDeviceName, jint jiButtonId, jboolean jbButtonDown)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	if (NULL != g_fpOnMojingKeyEventCB)
	{
		g_fpOnMojingKeyEventCB(32768, jiButtonId, (bool)(jbButtonDown == JNI_TRUE));
	}
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_OnAxisEvent(JNIEnv *env, jclass, jstring jsDeviceName, jint jiAxisID, jfloat jfValue)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	if (NULL != g_fpOnMojingAxisEventCB)
	{
		g_fpOnMojingAxisEventCB(32768, jiAxisID, jfValue);
	}
}

bool PreDispatchKeyEvent_Mojing2(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown);
bool PreDispatchKeyEvent_Mojing4(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown);
bool PreDispatchKeyEvent_Mojing5(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown);
bool PreDispatchKeyEvent_Mojing_motion(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown);
bool  Unreal_NativePreDispatchKeyEvent(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown)
{
	
	bool bRet = false;
	int EventType = AInputEvent_getType(event);
	int EventDevice = AInputEvent_getDeviceId(event);
	if (EventType == AINPUT_EVENT_TYPE_KEY)
	{
		// g_pMojingInputDeviceLock->DoLock();
		DeviceMap::iterator it = g_AllDeviceMap.find(EventDevice);
		if (g_AllDeviceMap.size() > 0 && it != g_AllDeviceMap.end())
		{
			switch (it->second.m_DeviceType)
			{
				case MID_JOYSTICK_MJ2:
				case MID_JOYSTICK_MJ3:
				{
					bRet = PreDispatchKeyEvent_Mojing2(event, iDevice, iKeyCode, bDown);
					break;
				}
                case MID_JOYSTICK_MJ4:
                {
                    bRet = PreDispatchKeyEvent_Mojing4(event, iDevice, iKeyCode, bDown);
                    break;
                }
                case MID_JOYSTICK_MJ5:
                {
                    bRet = PreDispatchKeyEvent_Mojing5(event, iDevice, iKeyCode, bDown);
                    break;
                }
                case MID_JOYSTICK_MJ_MAYBE:
                case MID_NONE:
                case MID_ERROR:
                    break;

			}
		}
		// g_pMojingInputDeviceLock->Unlock();
	}
	return bRet;
}


bool PreDispatchKeyEvent_Mojing4(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown)
{
    /*
    deviceState.mKeys.put(102, MojingKeyCode.KEYCODE_ENTER);
    deviceState.mKeys.put(103, MojingKeyCode.KEYCODE_BACK);
    deviceState.mKeys.put(104, MojingKeyCode.KEYCODE_MENU);
    */
    static std::map<int, int> g_KeyMapOfMojing4;
    if (g_KeyMapOfMojing4.size() == 0)
    {
        g_KeyMapOfMojing4[96] = AKEYCODE_ENTER;
        g_KeyMapOfMojing4[97] = AKEYCODE_BACK;
        g_KeyMapOfMojing4[100] = AKEYCODE_MENU;
    }
    int keyCode = AKeyEvent_getKeyCode(event);
    if (g_KeyMapOfMojing4.find(keyCode) != g_KeyMapOfMojing4.end())
    {
        iDevice = AInputEvent_getDeviceId(event);
        iKeyCode = g_KeyMapOfMojing4[keyCode];
        bDown = AKeyEvent_getAction(event) != AKEY_EVENT_ACTION_UP;

        //MOJING_TRACE(g_APIlogger, "KeyEvent_Mojing2 , ID = " << iDevice << " , KeyCode = " << iKeyCode << ", Down = " << bDown);
        return true;
    }
    return false;
}

bool PreDispatchKeyEvent_Mojing2(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown)
{
	/*
		deviceState.mKeys.put(102, MojingKeyCode.KEYCODE_ENTER);
		deviceState.mKeys.put(103, MojingKeyCode.KEYCODE_BACK);
		deviceState.mKeys.put(104, MojingKeyCode.KEYCODE_MENU);
	*/
	static std::map<int, int> g_KeyMapOfMojing2;
	if (g_KeyMapOfMojing2.size() == 0)
	{
		g_KeyMapOfMojing2[102] = AKEYCODE_ENTER;
		g_KeyMapOfMojing2[103] = AKEYCODE_BACK;
		g_KeyMapOfMojing2[104] = AKEYCODE_MENU;
	}
	int keyCode = AKeyEvent_getKeyCode(event);
	if (g_KeyMapOfMojing2.find(keyCode) != g_KeyMapOfMojing2.end())
	{
		iDevice = AInputEvent_getDeviceId(event);
		iKeyCode = g_KeyMapOfMojing2[keyCode]; 
		bDown = AKeyEvent_getAction(event) != AKEY_EVENT_ACTION_UP;
		
		//MOJING_TRACE(g_APIlogger, "KeyEvent_Mojing2 , ID = " << iDevice << " , KeyCode = " << iKeyCode << ", Down = " << bDown);
		return true;
	}
	return false;
}

bool PreDispatchKeyEvent_Mojing5(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown)
{
    static std::map<int, int> g_KeyMapOfMojing5;
    if (g_KeyMapOfMojing5.size() == 0)
    {
        g_KeyMapOfMojing5[104] = AKEYCODE_ENTER;
        g_KeyMapOfMojing5[102] = AKEYCODE_BACK;
        g_KeyMapOfMojing5[100] = AKEYCODE_MENU;
    }
    int keyCode = AKeyEvent_getKeyCode(event);
    if (g_KeyMapOfMojing5.find(keyCode) != g_KeyMapOfMojing5.end())
    {
        iDevice = AInputEvent_getDeviceId(event);
        iKeyCode = g_KeyMapOfMojing5[keyCode];
        bDown = AKeyEvent_getAction(event) != AKEY_EVENT_ACTION_UP;

        //MOJING_TRACE(g_APIlogger, "KeyEvent_Mojing2 , ID = " << iDevice << " , KeyCode = " << iKeyCode << ", Down = " << bDown);
        return true;
    }
    return false;
}


bool PreDispatchKeyEvent_Mojing_motion(AInputEvent* event, int& iDevice, int& iKeyCode, bool &bDown)
{
    static std::map<int, int> g_KeyMapOfMojingMotion;
    if (g_KeyMapOfMojingMotion.size() == 0)
    {
        g_KeyMapOfMojingMotion[66] = AKEYCODE_ENTER;
        g_KeyMapOfMojingMotion[23] = AKEYCODE_ENTER;
        g_KeyMapOfMojingMotion[97] = AKEYCODE_BACK;
    }
    int keyCode = AKeyEvent_getKeyCode(event);
    if (g_KeyMapOfMojingMotion.find(keyCode) != g_KeyMapOfMojingMotion.end())
    {
        iDevice = AInputEvent_getDeviceId(event);
        iKeyCode = g_KeyMapOfMojingMotion[keyCode];
        bDown = AKeyEvent_getAction(event) != AKEY_EVENT_ACTION_UP;

        //MOJING_TRACE(g_APIlogger, "KeyEvent_Mojing2 , ID = " << iDevice << " , KeyCode = " << iKeyCode << ", Down = " << bDown);
        return true;
    }
    return false;
}

int PreDispatchMotionEvent_Mojing2(AInputEvent* event, int iMaxAxisCount, int& iDevice, int* iAxis, float* fValue);
int PreDispatchMotionEvent_Mojing4(AInputEvent* event, int iMaxAxisCount, int& iDevice, int* iAxis, float* fValue);
int Unreal_NativePreDispatchMotionEvent(AInputEvent* event, int iMaxAxisCount, int& iDevice, int* iAxis, float* fValue)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	// 按照Unreal的接口说法，这个函数指针在R19版的NDK之后被支持
	if (g_fpGetAxes == NULL)
	{
		void* Lib = dlopen("libandroid.so", 0);
		if (Lib != NULL)
		{
			g_fpGetAxes = (FP_GetAxesType)dlsym(Lib, "AMotionEvent_getAxisValue");
			if (g_fpGetAxes == NULL)
			{
				MOJING_ERROR(g_APIlogger, "MotionEvent_SDK : Can not get function AMotionEvent_getAxisValue");
			}
		}
	}

	int iRet = 0;
	int EventType = AInputEvent_getType(event);
	int EventDevice = AInputEvent_getDeviceId(event);
	if (g_fpGetAxes != NULL &&
		EventType == AINPUT_EVENT_TYPE_MOTION)
	{
		// g_pMojingInputDeviceLock->DoLock();
		DeviceMap::iterator it = g_AllDeviceMap.find(EventDevice);
		if (g_AllDeviceMap.size() > 0 && it != g_AllDeviceMap.end())
		{
			switch (it->second.m_DeviceType)
			{
                case MID_JOYSTICK_MJ3:
				case MID_JOYSTICK_MJ2:
				{
					//MOJING_TRACE(g_APIlogger, "Call MotionEvent_Mojing2");
					iRet = PreDispatchMotionEvent_Mojing2(event, iMaxAxisCount , iDevice, iAxis, fValue);
					break;
				}
                case MID_JOYSTICK_MJ4:
                case MID_JOYSTICK_MJ5:
                    iRet = PreDispatchMotionEvent_Mojing4(event, iMaxAxisCount, iDevice, iAxis, fValue);
                    break;

                case MID_NONE:
                case MID_JOYSTICK_MJ_MAYBE:
                case MID_ERROR:
                
                    break;
			}
		}
		// g_pMojingInputDeviceLock->Unlock();
	}
	return iRet;
}

int PreDispatchMotionEvent_Mojing4(AInputEvent* event, int iMaxAxisCount, int& iDevice, int* iAxis, float* fValue)
{
    //MOJING_FUNC_TRACE(g_APIlogger);
    //int EventSource = AInputEvent_getSource(event);

    if (g_fpGetAxes)
    {
        if (iMaxAxisCount >= 2)
        {

            {
                
                for (int iAxis = 0; iAxis < 2; iAxis++)
                {
                    fValue[iAxis] = g_fpGetAxes(event, g_AxisList[iAxis], 0);
                    //MOJING_TRACE(g_APIlogger, "MotionEvent_Mojing2 : " << iAxis << ">> , Axis : " << g_AxisList[iAxis] << " , Value = " << iValues[g_AxisList[iAxis]]);
                }

                iAxis[0] = g_AxisList[0];
                iAxis[1] = g_AxisList[1];
                iDevice = AInputEvent_getDeviceId(event);
                    //MOJING_TRACE(g_APIlogger, "MotionEvent_Mojing2 , X = " << fValue[0] << " , Y = " << fValue[1]);
                return 2;
               
            }
        }
        else
        {
            MOJING_ERROR(g_APIlogger, "MotionEvent_Mojing2 , iMaxAxisCount  = " << iMaxAxisCount << " < 2");
        }

    }
    else
    {
        MOJING_ERROR(g_APIlogger, "MotionEvent_Mojing2 , fpGetAxes = NULL");
    }
    return 0;
}

unsigned char num_data_d(unsigned char x, unsigned char y)
{
	unsigned char i, j, k;
	j = 0;
	k = x;
	for (i = 1; i <= 8; i++)
	{
		if (((i % 2) != 0) && ((k & 0x01) == y))
		{
			j++;
		}
		k = k >> 1;
	}
	return j;
}

int PreDispatchMotionEvent_Mojing2(AInputEvent* event, int iMaxAxisCount, int& iDevice, int* iAxis, float* fValue)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	//int EventSource = AInputEvent_getSource(event);
	
	if (g_fpGetAxes)
	{
		if (iMaxAxisCount >= 2)
		{
			float fCheckValue[2];
			//if ((EventSource & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_MOVE)
			{
				int iValues[2] = { 0, 0 };
				for (int iAxis = 0; iAxis < 2; iAxis++)
				{
					iValues[iAxis] = 4096 * g_fpGetAxes(event, g_AxisList[iAxis], 0);
					//MOJING_TRACE(g_APIlogger, "MotionEvent_Mojing2 : " << iAxis << ">> , Axis : " << g_AxisList[iAxis] << " , Value = " << iValues[g_AxisList[iAxis]]);
				}

				int i;
				int xData = iValues[0];
				int yData = iValues[1];
				int AdcX, AdcY, sumX;
				if (xData == 0 && yData == 0) {
					AdcX = 0x80;
					AdcY = 0x80;
					sumX = 0;

				}
				else {
					// bits of data: BA98 7654 3210
					// real position 873210A9
					// Check Sum: 654 (xdata) 654 (ydata)
					i = xData;
					AdcX = (i >> 9) & 0x03;			//bit2-bit1
					AdcX |= ((i << 2) & 0x3c);		//bit6-bit3
					AdcX |= ((i >> 1) & 0xc0);		//bit8-bit7
					sumX = ((i >> 1) & 0x38);	//CHECK楂樹笁浣�

					i = yData;
					AdcY = (i >> 9) & 0x03;			//bit2-bit1
					AdcY |= ((i << 2) & 0x3c);		//bit6-bit3
					AdcY |= ((i >> 1) & 0xc0);		//bit8-bit7
					sumX |= ((i >> 4) & 0x7);	//CHECK浣庝笁浣�
				}
				//////////////////////////////////////////////////
				// int getCheckSum()
				//////////////////////////////////////////////////
				int iCheckSum = 0;
				{
					iCheckSum = num_data_d(AdcX, 0);
					iCheckSum += num_data_d(((AdcY >> 1) & 0x7f), 1);
					unsigned char t = AdcX >> 1;
					t = (t^AdcY) & 0x55;
					t = (num_data_d(t, 1) << 1) & 0xfe;
					iCheckSum += t;
					if (iCheckSum >= 16)
						iCheckSum = 0;
				}// end of check sum
				if (iCheckSum == sumX)
				{
					iAxis[0] = g_AxisList[0];
					iAxis[1] = g_AxisList[1];
                    fValue[0] = (AdcX - 128) / 127.0f;
                    fValue[1] = (AdcY - 128) / 127.0f;
					iDevice = AInputEvent_getDeviceId(event);
					//MOJING_TRACE(g_APIlogger, "MotionEvent_Mojing2 , X = " << fValue[0] << " , Y = " << fValue[1]);
					return 2;
				}
				else
				{
					//char szBuf[1024];
					//*szBuf = 0;
					//sprintf(szBuf + strlen(szBuf), "xData = %d(0x%X) , ", xData, xData);
					//sprintf(szBuf + strlen(szBuf), "yData = %d(0x%X) , ", yData, yData);

					//sprintf(szBuf + strlen(szBuf), "AdcX = %d(0x%X) , ", AdcX, AdcX);
					//sprintf(szBuf + strlen(szBuf), "AdcY = %d(0x%X) , ", AdcY, AdcY);
					//
					//sprintf(szBuf + strlen(szBuf), "sumX = %d(0x%X) , ", sumX, sumX);
					//sprintf(szBuf + strlen(szBuf), "iCheckSum = %d(0x%X)", iCheckSum, iCheckSum);

					//MOJING_ERROR(g_APIlogger, "CHECK SUM ERROR , " << szBuf);

					iAxis[0] = g_AxisList[0];
					iAxis[1] = g_AxisList[1];
					fValue[0] = 0;
					fValue[1] = 0;
					iDevice = AInputEvent_getDeviceId(event);
					//MOJING_TRACE(g_APIlogger, "MotionEvent_Mojing2 , X = " << fValue[0] << " , Y = " << fValue[1]);
					return 2;
				}
			}
// 			else
// 			{
// 				MOJING_ERROR(g_APIlogger, "MotionEvent_Mojing2 , EventSource = " << EventSource << "Unmask with AMOTION_EVENT_ACTION_MOVE (" << AMOTION_EVENT_ACTION_MOVE << ")");
// 			}
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "MotionEvent_Mojing2 , iMaxAxisCount  = " << iMaxAxisCount << " < 2");
		}

	}
	else
	{
		MOJING_ERROR(g_APIlogger, "MotionEvent_Mojing2 , fpGetAxes = NULL");
	}
	return 0;
}

typedef void(*FP_OnTouchPadStatusChangeCB)(int32_t deviceId, bool buttonDown);
typedef void(*FP_OnTouchPadPosCB)(int32_t deviceId, float x, float y);
FP_OnTouchPadStatusChangeCB g_OnTouchPadStatusChangeCB = NULL;
FP_OnTouchPadPosCB g_fpOnTouchPadPosCB = NULL;

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_OnTouchPadStatusChange(JNIEnv *env, jclass, jint jiDeviceID, bool bisTouched)
{
    if (g_OnTouchPadStatusChangeCB)
    {
        g_OnTouchPadStatusChangeCB(jiDeviceID, bisTouched);
    }
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_OnTouchPadPos(JNIEnv *env, jclass, jint jiDeviceID, float x, float y)
{
    if (g_fpOnTouchPadPosCB)
    {
        g_fpOnTouchPadPosCB(jiDeviceID, x, y);
    }
}

#endif
