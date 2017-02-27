#ifndef MJ_GLSTATESAVE_H
#define MJ_GLSTATESAVE_H

#include "../../Base/GlUtils.h"
#ifdef _DEBUG
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif
extern MojingLogger g_APIlogger;
#define TRACE(_X_) MOJING_TRACE(g_APIlogger , _X_) 
#endif

namespace Baofeng
{
	namespace Mojing
	{
		class GLStateSave
		{
		public:
			// If our code only uses VertexArrayObjects, we don't need to
			// save and restore all the buffer related state
			static const bool	exclusivelyVAO = true;

			GLStateSave();
			~GLStateSave();
			void GL_Enable(const GLenum feature, GLboolean enabled)
			{
				if (enabled)
				{
					glEnable(feature);
				}
				else
				{
					glDisable(feature);
				}
			}


			GLint	Blend;
			GLint	BlendDstAlpha;
			GLint	BlendDstRGB;
			GLint	BlendEquationAlpha;
			GLint	BlendEquationRgb;
			GLint	BlendSrcAlpha;
			GLint	BlendSrcRGB;

			GLint	ElementArrayBuffer;
			GLint	ArrayBuffer;
			static const int MAX_ATTRIBS = 4;
			GLint	vertexAttribArrayEnabled[MAX_ATTRIBS];

			GLint	ScissorTest;
			GLint	ScissorBox[4];

			GLint	DepthTest;
			GLint	DepthFunc;
			GLint	DepthWriteMask;

			GLint   CullFace;
		};
	} // namespace Mojing

}	// namespace Baofeng

#endif // MJ_GLSTATESAVE_H
