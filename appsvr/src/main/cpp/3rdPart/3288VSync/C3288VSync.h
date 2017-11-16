#pragma once
#include "../../Base/MojingTypes.h"

namespace Baofeng
{
	namespace Mojing
	{
		typedef int(*FP_hwvsync_init)(void);
		typedef uint64_t(*FP_hwvsync)(void);
		typedef int(*FP_hwvsync_deinit)(void);

		class C3288VSync
		{
		public:
			C3288VSync();
			virtual ~C3288VSync();

			bool OpenVSync();
			bool CloseVSync();
			double WaitNextVSync();
		private:
			void * m_hDLL;
			FP_hwvsync_init		m_fpInit;
			FP_hwvsync_deinit	m_fpDeInit;
			FP_hwvsync			m_fpWaitVSync;
			bool m_bInitd;
		};
	}
}
