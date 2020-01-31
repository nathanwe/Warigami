#ifndef WIZARDPEOPLE_RENDERING_RENDER_STATE_HPP
#define WIZARDPEOPLE_RENDERING_RENDER_STATE_HPP

#include "glbinding/gl/gl.h"

namespace rendering
{
	struct render_state
	{
		gl::GLboolean uses_cull_face;
		gl::GLboolean uses_depth_test;
		gl::GLboolean uses_blend;
		gl::GLboolean depth_mask;
		gl::GLenum culled_face;
		gl::GLenum depth_func;
		gl::GLenum blend_src;
		gl::GLenum blend_dest;
		unsigned int target = 0;
	};
}

#endif
