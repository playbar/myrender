#ifndef _H_GLPROGRAME_
#define _H_GLPROGRAME_

#include "../Base/MojingTypes.h"
namespace Baofeng
{
	namespace Mojing
	{
		class GlProgram
		{
		public:
			GlProgram();
			virtual ~GlProgram();
			void Release();
		public:
			unsigned	program;
			unsigned	vertexShader;
			unsigned	fragmentShader;

			// Uniforms that aren't found will have a -1 value
			int		uMvp;				// uniform Mvpm
			int		uModel;				// uniform Modelm
			int		uView;				// uniform Viewm
			int		uTexm;				// uniform Texm
			int		uTexClipX;		// 对于图像有效范围进行边界限定
			int		uTexture0;
			int		uLayerIndex;		// uniform multiview layer index
		};

		struct WarpProg
		{
			GlProgram	Prog;
		};
		extern const char *VertexShader;
		extern const char *VertexShaderLayout;
		extern const char *FragmentShader;
		extern const char *FragmentShaderNoDispersion; 
		extern const char *FragmentShaderLayout;
		extern const char *FragmentShaderLayoutNoDispersion;
		extern const char *VertexShaderDistortionRange;
		extern const char *FragmentShaderDistortionRange;
		extern const char *FragmentShaderDistortionRangeDark;
		// Multiview
		extern const char* VertexShaderMultiviewNoDispertion;
		extern const char* FragmentShaderMultiviewNoDispertion;
		extern const char* VertexShaderMultiviewHighResNoDispertion;
		extern const char* FragmentShaderMultiviewHighResNoDispertion;
	}
}
#endif