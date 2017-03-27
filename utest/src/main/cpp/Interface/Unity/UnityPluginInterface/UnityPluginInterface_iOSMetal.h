#pragma once

#include "UnityPluginInterface_iOSGL.h"

#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
#include <TargetConditionals.h>
#include "../../../Render/Metal/MojingRenderMetal.h"
#include "../MetalForwardDecls.h" // UnityRenderTex

#include "../MojingMetalView.h"

namespace Baofeng
{
	namespace Mojing
	{
		class UnityPluginInterface_iOSMetal :
			public UnityPluginInterface_iOSGL
		{
            friend class UnityPluginInterfaceBase;
        protected:
			UnityPluginInterface_iOSMetal();
			virtual ~UnityPluginInterface_iOSMetal();
        public:
			// 1、OnEnable中，先调用Unity_SetMojingWorld改变GlassName和TimeWarpStatus，然后触发
			virtual void OnEvent_EnterMojingWorld();

			// 2、WaitForEndOfFrame 后调用SetTextureID，随后触发本事件
			virtual void OnEvent_SetTextureID();

			// 3、SetTextureID 后立即调用SetTextureID
			virtual void OnEvent_DistortFrame();

			// 4、OnDisable的时候调用
			virtual void OnEvent_LeaveMojingWorld();

			// 5、用户修改镜片的时候调用
			virtual void OnEvent_ChangeMojingWorld();

		};
		

	}
}
#endif