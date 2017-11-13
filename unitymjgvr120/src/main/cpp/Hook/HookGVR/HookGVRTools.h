#pragma once
#include "../../Base/MojingTypes.h"
#ifdef MJ_OS_ANDROID
// from gvrtyprs.h
typedef struct gvr_context_ gvr_context;
struct gvr_frame;
struct gvr_buffer_viewport_list;

typedef struct gvr_clock_time_point 
{
	int64_t monotonic_system_time_nanos;
} gvr_clock_time_point;
// end gvrtyprs.h

typedef struct gvr_mat4f
{
	float m[4][4];
} gvr_mat4f;

typedef struct gvr_version_ {
    int32_t major;
    int32_t minor;
    int32_t patch;
} gvr_version;

typedef struct gvr_sizei {
	int32_t width;
	int32_t height;
} gvr_sizei;

#define FN_gvr_get_head_space_from_start_space_rotation "gvr_get_head_space_from_start_space_rotation"
typedef gvr_mat4f (*FP_gvr_get_head_space_from_start_space_rotation)(const gvr_context *gvr, const gvr_clock_time_point time);

#define FN_gvr_reset_tracking "gvr_reset_tracking"// Deprecated
typedef void (*FP_gvr_reset_tracking)(gvr_context *gvr);// Deprecated

#define FN_gvr_recenter_tracking "gvr_recenter_tracking"
typedef void(*FP_gvr_recenter_tracking)( gvr_context *gvr);


#define FN_gvr_get_viewer_model "gvr_get_viewer_model"
typedef const char *(*FP_gvr_get_viewer_model)(const gvr_context *gvr);

#define FN_gvr_get_viewer_vendor "gvr_get_viewer_vendor"
typedef const char *(*FP_gvr_get_viewer_vendor)(const gvr_context *gvr);

#define FN_gvr_get_version_string "gvr_get_version_string"
typedef const char * (*FP_gvr_get_version_string)();

#define FN_gvr_on_surface_created_reprojection_thread "gvr_on_surface_created_reprojection_thread"
typedef int (*FP_gvr_on_surface_created_reprojection_thread)(const gvr_context *gvr);

#define FN_gvr_render_reprojection_thread "gvr_render_reprojection_thread"
typedef int (*FP_gvr_render_reprojection_thread)(const gvr_context *gvr);

#define FN_gvr_get_version "gvr_get_version"
typedef gvr_version (*FP_gvr_get_version)();

#define FN_gvr_initialize_gl "gvr_initialize_gl"
typedef void (*FP_gvr_initialize_gl)(gvr_context* gvr);

#define FN_gvr_frame_bind_buffer "gvr_frame_bind_buffer"
typedef void (*FP_gvr_frame_bind_buffer)(gvr_frame* frame, int32_t index);

#define FN_gvr_frame_unbind "gvr_frame_unbind"
typedef void (*FP_gvr_frame_unbind)(gvr_frame* frame);

#define FN_gvr_is_feature_supported "gvr_is_feature_supported"
typedef bool (*FP_gvr_is_feature_supported)(const gvr_context* gvr, int32_t feature);

#define FN_gvr_frame_submit "gvr_frame_submit"
typedef void(*FP_gvr_frame_submit)(gvr_frame **frame,	const gvr_buffer_viewport_list *list,	gvr_mat4f head_space_from_start_space);

#define FN_gvr_get_maximum_effective_render_target_size "gvr_get_maximum_effective_render_target_size"
typedef gvr_sizei (*FP_gvr_get_maximum_effective_render_target_size)(const gvr_context* gvr);

#define  USING_SVR_SENSOR 0
class HookGVRTools
{
public:
	HookGVRTools();
	virtual ~HookGVRTools();
	static bool Init();

//	static gvr_version HOOK_gvr_get_version();
private:
#if USING_SVR_SENSOR
	static class CSVRApi m_SVRApi;
	
#else
	static bool	m_bSVREnable;
#endif

//	static double m_dRotateSpeed;
	
	static bool LoadGVR();
	static gvr_mat4f HOOK_gvr_get_head_space_from_start_space_rotation(const gvr_context *gvr, const gvr_clock_time_point time);
	static void HOOK_gvr_frame_submit(gvr_frame **frame, const gvr_buffer_viewport_list *list, gvr_mat4f head_space_from_start_space);
//	static int HOOK_gvr_on_surface_created_reprojection_thread(const gvr_context *gvr);
	static int HOOK_gvr_render_reprojection_thread(const gvr_context *gvr);
	static void HOOK_gvr_initialize_gl(gvr_context* gvr);
	static bool HOOK_gvr_is_feature_supported(const gvr_context* gvr, int32_t feature);
    static void HOOK_gvr_frame_unbind(gvr_frame* frame);

	static void HOOK_gvr_reset_tracking(gvr_context *gvr); // Deprecated
	static void HOOK_gvr_recenter_tracking(gvr_context *gvr);
	static void * m_hGVR;
	static FP_gvr_get_head_space_from_start_space_rotation m_fp_gvr_get_head_space_from_start_space_rotation;
	static FP_gvr_frame_submit m_fp_gvr_frame_submit;
	static FP_gvr_reset_tracking m_fp_gvr_reset_tracking;// Deprecated
	static FP_gvr_recenter_tracking m_fp_gvr_recenter_tracking;

	static FP_gvr_get_viewer_model m_fp_gvr_get_viewer_model;
	static FP_gvr_get_viewer_vendor m_fp_gvr_get_viewer_vendor;
	static FP_gvr_get_version_string m_fp_gvr_get_version_string;
//	static FP_gvr_on_surface_created_reprojection_thread m_fp_gvr_on_surface_created_reprojection_thread;
	static FP_gvr_render_reprojection_thread m_fp_gvr_render_reprojection_thread;
	static FP_gvr_get_version m_fp_gvr_get_version;
	static FP_gvr_initialize_gl m_fp_gvr_initialize_gl;
	static FP_gvr_frame_bind_buffer m_fp_gvr_frame_bind_buffer;
	static FP_gvr_frame_unbind m_fp_gvr_frame_unbind;
	static FP_gvr_is_feature_supported m_fp_gvr_is_feature_supported;
	static FP_gvr_get_maximum_effective_render_target_size m_fp_gvr_get_maximum_effective_render_target_size;

};
extern bool g_bEnableDDTracker;
extern float g_fDDHeaderView[16];
#endif