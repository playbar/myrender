#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "ReporterTools.h"

namespace Baofeng
{
	namespace Mojing
	{
		class GyroTempCalibrationReporter:public ReporterTools
		{
			GyroTempCalibrationReporter();
			static void InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
		public:
			virtual ~GyroTempCalibrationReporter();
			static GyroTempCalibrationReporter* GetGyroTempCalibrationRepoter();
			CLASS_MEMBER_STR(String, m_str, ClassName); 
			CLASS_MEMBER_STR(String, m_str, File);
			CLASS_MEMBER_STR(String, m_str, App);
			CLASS_MEMBER_STR(String, m_str, Brand);
			CLASS_MEMBER_STR(String, m_str, Mobile);
			CLASS_MEMBER_STR(String, m_str, Serial);
			CLASS_MEMBER_STR(String, m_str, Display);
			CLASS_MEMBER_STR(String, m_str, Sensor);
			CLASS_MEMBER_STR(String, m_str, OS);
			CLASS_MEMBER_STR(String, m_str, GLES);
			CLASS_MEMBER_STR(String, m_str, EGL);
			CLASS_MEMBER_STR(String, m_str, GlassesName);
			CLASS_MEMBER_STR(String, m_str, GlassesReportName);
			CLASS_MEMBER_STR(String, m_str, MojingSN);
			CLASS_MEMBER_STR(String, m_str, GyroOffsetReportSet);
			CLASS_MEMBER(bool , m_b , EnablePostRepote);
			void Post();
			void UpdateConfig();
		};
	}
}
