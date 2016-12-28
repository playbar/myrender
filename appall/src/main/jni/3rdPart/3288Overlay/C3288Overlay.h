#pragma once
#include "../../Base/GlUtils.h"

typedef struct __tagOverlayHandle{
	int m_iErrorCode;
	int m_iWidth;
	int m_iHeight;
	int m_iLayer;
	char m_szTag[32];
	EGLSurface m_Surface;
	EGLContext m_Context;
}*OVERLAY_HANDLE;


typedef OVERLAY_HANDLE(*FP_CreateOverlay)(const char* szOverlayTag, unsigned int iWidth, unsigned int iHeight, unsigned int iLayer, EGLContext SharedContext, EGLint* pContexAttribute);
typedef unsigned int	(*FP_MoveOverlay		)(OVERLAY_HANDLE hHandle, float fX, float fY);
typedef unsigned int	(*FP_ResizeOverlay		)(OVERLAY_HANDLE hHandle, unsigned int iWidth, unsigned int iHeight);
typedef unsigned int	(*FP_SetOverlayVisible	)(OVERLAY_HANDLE hHandle, bool bVisible);
typedef void			(*FP_DestroyOverlay		)(OVERLAY_HANDLE hHandle);

// #define FN_CreateOverlay		"_ZN7android13CreateOverlayEPKcjjj"
#define FN_CreateOverlay		"_ZN7android13CreateOverlayEPKcjjjPvPi"
#define FN_MoveOverlay			"_ZN7android11MoveOverlayEPNS_18__tagOverlayHandleEff"
#define FN_ResizeOverlay		"_ZN7android13ResizeOverlayEPNS_18__tagOverlayHandleEjj"
#define FN_SetOverlayVisible	"_ZN7android17SetOverlayVisibleEPNS_18__tagOverlayHandleEb"
#define FN_DestroyOverlay		"_ZN7android14DestroyOverlayEPNS_18__tagOverlayHandleE"

class C3288Overlay
{
public:
	C3288Overlay();
	virtual ~C3288Overlay();
	OVERLAY_HANDLE	CreateOverlay(const char* szOverlayTag, unsigned int iWidth, unsigned int iHeight, unsigned int iLayer , EGLContext SharedContext , EGLint* pContexAttribute);
	unsigned int	MoveOverlay(OVERLAY_HANDLE hHandle, float fX, float fY);
	unsigned int	ResizeOverlay(OVERLAY_HANDLE hHandle, unsigned int iWidth, unsigned int iHeight);
	unsigned int	SetOverlayVisible(OVERLAY_HANDLE hHandle, bool bVisible);
	void			DestroyOverlay(OVERLAY_HANDLE hHandle);
	bool			Init();
	bool			GetInit()const{ return m_bInit; };
	void			Release();
private:
	void * m_hDLL;
	bool m_bInit;
	FP_CreateOverlay			m_fpCreateOverlay;
	FP_MoveOverlay				m_fpMoveOverlay;
	FP_ResizeOverlay			m_fpResizeOverlay;
	FP_SetOverlayVisible		m_fpSetOverlayVisible;
	FP_DestroyOverlay			m_fpDestroyOverlay;
};

