#include "GlStateSave.h"
#include "../../Base/GlUtils.h"

#ifdef _DEBUG
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif
//extern MojingLogger g_APIlogger;
//#define TRACE(_X_) MOJING_TRACE(g_APIlogger , _X_)
#endif



namespace Baofeng
{
	namespace Mojing
	{
		GLStateSave::GLStateSave()
		{

			glGetIntegerv(GL_BLEND, &Blend);
			glGetIntegerv(GL_BLEND_DST_ALPHA, &BlendDstAlpha);
			glGetIntegerv(GL_BLEND_DST_RGB, &BlendDstRGB);
			glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &BlendEquationAlpha);
			glGetIntegerv(GL_BLEND_EQUATION_RGB, &BlendEquationRgb);
			glGetIntegerv(GL_BLEND_SRC_ALPHA, &BlendSrcAlpha);
			glGetIntegerv(GL_BLEND_SRC_RGB, &BlendSrcRGB);

			// We update an array for debug graphs, so this needs to
			// be saved even though we use VAO, because GL_ARRAY_BUFFER_BINDING
			// isn't part of the VAO state.
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &ArrayBuffer);

			if (!exclusivelyVAO)
			{
				glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ElementArrayBuffer);
				for (int i = 0; i < MAX_ATTRIBS; i++)
				{
					glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &vertexAttribArrayEnabled[i]);
				}
			}

			glGetIntegerv(GL_SCISSOR_TEST, &ScissorTest);
			glGetIntegerv(GL_SCISSOR_BOX, ScissorBox);

			glGetIntegerv(GL_DEPTH_TEST, &DepthTest);
			glGetIntegerv(GL_DEPTH_FUNC, &DepthFunc);
			glGetIntegerv(GL_DEPTH_WRITEMASK, &DepthWriteMask);
			glGetIntegerv(GL_CULL_FACE, &CullFace);
		}

		GLStateSave::~GLStateSave()
		{
			glBlendEquationSeparate(BlendEquationRgb, BlendEquationAlpha);
			glBlendFuncSeparate(BlendSrcRGB, BlendDstRGB, BlendSrcAlpha, BlendDstAlpha);
			GL_Enable(GL_BLEND, Blend);

			glBindBuffer(GL_ARRAY_BUFFER, ArrayBuffer);

			if (exclusivelyVAO)
			{
#ifdef MJ_OS_ANDROID
				if (glBindVertexArrayOES_ != NULL)
					glBindVertexArrayOES_(0);
#else
				glBindVertexArray(0);
#endif
			}
			else
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementArrayBuffer);
				for (int i = 0; i < MAX_ATTRIBS; i++)
				{
					if (vertexAttribArrayEnabled[i])
					{
						glEnableVertexAttribArray(i);
					}
					else
					{
						glDisableVertexAttribArray(i);
					}
				}
			}

			GL_Enable(GL_SCISSOR_TEST, ScissorTest);
			glScissor(ScissorBox[0], ScissorBox[1], ScissorBox[2], ScissorBox[3]);

			GL_Enable(GL_DEPTH_TEST, DepthTest);
			glDepthFunc(DepthFunc);

			glDepthMask(DepthWriteMask);

			GL_Enable(GL_CULL_FACE, CullFace);
		}
	} // namespace Mojing
}	// namespace Baofeng




