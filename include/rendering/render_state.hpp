#ifndef WIZARDPEOPLE_RENDERING_RENDER_STATE_HPP
#define WIZARDPEOPLE_RENDERING_RENDER_STATE_HPP

#include "glbinding/gl/gl.h"

namespace rendering
{
	struct render_state
	{
		gl::GLboolean uses_cull_face  = gl::GL_FALSE;
		gl::GLboolean uses_depth_test = gl::GL_TRUE;
		gl::GLboolean uses_blend      = gl::GL_FALSE;
		gl::GLboolean depth_mask      = gl::GL_TRUE;
		gl::GLenum culled_face        = gl::GL_BACK;
		gl::GLenum depth_func         = gl::GL_LESS;
		gl::GLenum blend_src          = gl::GL_ONE;
		gl::GLenum blend_dest         = gl::GL_ZERO;
		gl::GLenum polygon_mode       = gl::GL_FILL;
		unsigned int target = 0;
	};
}

#endif
