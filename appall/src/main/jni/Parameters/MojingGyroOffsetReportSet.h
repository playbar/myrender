#pragma once
#include "MojingGyroOffsetReport.h"

namespace Baofeng
{
	namespace Mojing
	{
		class MojingGyroOffsetReportSet:public MojingJsonElement
		{
			MojingGyroOffsetReport** m_MojingGyroOffsetReport;
			void AllocMemory();
			void FreeMemory();
		public:
			MojingGyroOffsetReportSet(int iBins, int iSamples);
			virtual ~MojingGyroOffsetReportSet();

			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);
			MojingGyroOffsetReport* operator[](int index) const
			{
				MJ_ASSERT(index >= 0 && index < GetNumBins());
				if (index >= 0 && index < GetNumBins())
					return m_MojingGyroOffsetReport[index];
				return NULL;
			}

			CLASS_MEMBER_J(int, m_i, NumBins);
			CLASS_MEMBER_J(int, m_i, NumSamples);
		};

	}
}