#include "C3288Overlay.h"
#include <dlfcn.h>

C3288Overlay::C3288Overlay() :
m_hDLL(NULL),
m_bInit(false),
m_fpCreateOverlay(NULL),
m_fpMoveOverlay(NULL),
m_fpResizeOverlay(NULL),
m_fpSetOverlayVisible(NULL),
m_fpDestroyOverlay(NULL)
{
}
#define GET_DLL_FUNCION(DLL , FUNC)  m_fp##FUNC = (FP_##FUNC)dlsym(DLL , FN_##FUNC)

C3288Overlay::~C3288Overlay()
{
	Release();
}

bool C3288Overlay::Init()
{
	if (!m_bInit)
	{
		m_hDLL = dlopen("libOverlay.so", RTLD_NOW | RTLD_LOCAL);

		if (m_hDLL)
		{
			GET_DLL_FUNCION(m_hDLL, CreateOverlay);
			GET_DLL_FUNCION(m_hDLL, MoveOverlay);
			GET_DLL_FUNCION(m_hDLL, ResizeOverlay);
			GET_DLL_FUNCION(m_hDLL, SetOverlayVisible);
			GET_DLL_FUNCION(m_hDLL, DestroyOverlay);

			if (m_fpCreateOverlay != NULL &&
				m_fpMoveOverlay != NULL &&
				m_fpResizeOverlay != NULL &&
				m_fpSetOverlayVisible != NULL &&
				m_fpDestroyOverlay != NULL )
			{
				m_bInit = true;
			}
			else
			{// 有函数指针拿不到
				Release();
			}
		}
		else// m_hDLL == NULL
		{
			const char* err = dlerror();
			//MOJING_ERROR(g_APIlogger, "Can not load libary \"libOverlay.so\"  Error = " << (err ? err : "Unknown"));
		}
	}

	return m_bInit;
}
void C3288Overlay::Release()
{
	if (m_hDLL)
		dlclose(m_hDLL);
	m_hDLL = NULL;
	m_fpCreateOverlay = NULL;
	m_fpMoveOverlay = NULL;
	m_fpResizeOverlay = NULL;
	m_fpSetOverlayVisible = NULL;
	m_fpDestroyOverlay = NULL;
	
	m_bInit = (false);
}
OVERLAY_HANDLE	C3288Overlay::CreateOverlay(const char* szOverlayTag, unsigned int iWidth, unsigned int iHeight, unsigned int iLayer, EGLContext SharedContext, EGLint* pContexAttribute)
{
	if (m_fpCreateOverlay)
		return m_fpCreateOverlay(szOverlayTag, iWidth, iHeight, iLayer, SharedContext, pContexAttribute);
	return NULL;
}
unsigned int C3288Overlay::MoveOverlay(OVERLAY_HANDLE hHandle, float fX, float fY)
{
	if (m_fpMoveOverlay)
		return m_fpMoveOverlay(hHandle, fX, fY);
	return -1;
}
unsigned int C3288Overlay::ResizeOverlay(OVERLAY_HANDLE hHandle, unsigned int iWidth, unsigned int iHeight)
{
	if (m_fpResizeOverlay)
		return m_fpResizeOverlay(hHandle, iWidth, iHeight);
	return -1;
}
unsigned int C3288Overlay::SetOverlayVisible(OVERLAY_HANDLE hHandle, bool bVisible)
{
	if (m_fpSetOverlayVisible)
		return m_fpSetOverlayVisible(hHandle, bVisible);

	return -1;
}
void C3288Overlay::DestroyOverlay(OVERLAY_HANDLE hHandle)
{
	if (m_fpDestroyOverlay)
		m_fpDestroyOverlay(hHandle);
}
