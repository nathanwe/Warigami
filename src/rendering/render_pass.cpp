#include "rendering/render_pass.hpp"

#include "glbinding/gl/gl.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace gl;

namespace rendering
{
	void verify_shader_compilation(unsigned int shader_id, std::string const& filepath);
	void verify_shader_linkage(unsigned int render_program_id);

	render_pass::render_pass(description& desc) :
		_render_state(desc.state),
		m_render_target(desc.target)
	{
		unsigned int vs_id = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fs_id = glCreateShader(GL_FRAGMENT_SHADER);
		assert(vs_id != 0 && fs_id != 0);

		char const* vs_text_cstr = desc.proto_vertex.source.c_str();
		char const* fs_text_cstr = desc.proto_fragment.source.c_str();

		// Compile shaders
		glShaderSource(vs_id, 1, &vs_text_cstr, NULL);
		glCompileShader(vs_id);
#ifndef NDEBUG
		verify_shader_compilation(vs_id, desc.proto_vertex.path);
#endif
		glShaderSource(fs_id, 1, &fs_text_cstr, NULL);
		glCompileShader(fs_id);
#ifndef NDEBUG
		verify_shader_compilation(fs_id, desc.proto_fragment.path);
#endif

		// Connect shaders to render pass
		m_render_program_id = glCreateProgram();
		glAttachShader(m_render_program_id, vs_id);
		glAttachShader(m_render_program_id, fs_id);
		glLinkProgram(m_render_program_id);
#ifndef NDEBUG
		verify_shader_linkage(m_render_program_id);
#endif

		glDetachShader(m_render_program_id, vs_id);
		glDetachShader(m_render_program_id, fs_id);
		glDeleteShader(vs_id);
		glDeleteShader(fs_id);
	}

	void verify_shader_compilation(unsigned int shader_id, std::string const& filepath)
	{
		GLint result;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shader_id, 1024, NULL, infoLog);
			std::cout << "Error: Shader failed to compile. File: << " << filepath << std::endl << "OpenGL logs:\n" << infoLog << std::endl;
		}
	}

	void verify_shader_linkage(unsigned int renderPass_id)
	{
		GLint result;
		glGetProgramiv(renderPass_id, GL_LINK_STATUS, &result);
		if (!result)
		{
			GLchar infoLog[1024];
			glGetProgramInfoLog(renderPass_id, 1024, NULL, infoLog);
			std::cout << "Error: Render pass failed to link shaders. OpenGL logs:\n" << infoLog << std::endl;
		}
	}

	render_pass::~render_pass()
	{
		glDeleteProgram(m_render_program_id);
	}

	int render_pass::get_parameter_location(std::string const& p_r_name) const
	{
		return glGetUniformLocation(m_render_program_id, p_r_name.c_str());
	}

	void render_pass::bind(render_state& current_state) const
	{
		glUseProgram(m_render_program_id);
		
		// Render target
		if (_render_state.target != current_state.target)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _render_state.target);
			current_state.target = _render_state.target;
		}

		// Polygon mode
		if (_render_state.polygon_mode != current_state.polygon_mode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, _render_state.polygon_mode);
			current_state.polygon_mode = _render_state.polygon_mode;
		}

		// Face culling
		if (_render_state.uses_cull_face)
		{
			if (_render_state.uses_cull_face != current_state.uses_cull_face)
			{
				glEnable(GL_CULL_FACE);
				current_state.uses_cull_face = _render_state.uses_cull_face;
			}
			if (_render_state.culled_face != current_state.culled_face)
			{
				auto is_cull_face = [](GLenum val)
				{
					return is_enum_among<GL_FRONT, GL_BACK, GL_FRONT_AND_BACK>(val);
				};
				assert(is_cull_face(_render_state.culled_face));
				glCullFace(_render_state.culled_face);
				current_state.culled_face = _render_state.culled_face;
			}
		}
		else
		{
			if (_render_state.uses_cull_face != current_state.uses_cull_face)
			{
				glDisable(GL_CULL_FACE);
				current_state.uses_cull_face = _render_state.uses_cull_face;
			}
		}

		// Depth testing
		if (_render_state.uses_depth_test)
		{
			if (_render_state.uses_depth_test != current_state.uses_depth_test)
			{
				glEnable(GL_DEPTH_TEST);
				current_state.uses_depth_test = _render_state.uses_depth_test;
			}
			if (_render_state.depth_mask != current_state.depth_mask)
			{
				glDepthMask(_render_state.depth_mask);
				current_state.depth_mask = _render_state.depth_mask;
			}
			if (_render_state.depth_func != current_state.depth_func)
			{
				auto is_depth_func = [](GLenum val)
				{
					return is_enum_among<
						GL_NEVER, 
						GL_LESS, 
						GL_EQUAL, 
						GL_LEQUAL, 
						GL_GREATER, 
						GL_NOTEQUAL,
						GL_GEQUAL, 
						GL_ALWAYS>(val);
				};
				assert(is_depth_func(_render_state.depth_func));
				glDepthFunc(_render_state.depth_func);
				current_state.depth_func = _render_state.depth_func;
			}
		}
		else
		{
			if (_render_state.uses_depth_test != current_state.uses_depth_test)
			{
				glDisable(GL_DEPTH_TEST);
				current_state.uses_depth_test = _render_state.uses_depth_test;
			}
		}

		// Blending
		if (_render_state.uses_blend)
		{
			if (_render_state.uses_blend != current_state.uses_blend)
			{
				glEnable(GL_BLEND);
				current_state.uses_blend = _render_state.uses_blend;
			}
			if (_render_state.blend_src != current_state.blend_src || _render_state.blend_dest != current_state.blend_dest)
			{
				auto is_blend_func = [&](GLenum val)
				{
					return is_enum_among<
						GL_ZERO,
						GL_ONE,
						GL_SRC_COLOR,
						GL_ONE_MINUS_SRC_COLOR,
						GL_DST_COLOR,
						GL_ONE_MINUS_DST_COLOR,
						GL_SRC_ALPHA,
						GL_ONE_MINUS_SRC_ALPHA,
						GL_DST_ALPHA,
						GL_ONE_MINUS_DST_ALPHA,
						GL_CONSTANT_COLOR,
						GL_ONE_MINUS_CONSTANT_COLOR,
						GL_CONSTANT_ALPHA,
						GL_ONE_MINUS_CONSTANT_ALPHA>(val);
				};
				assert(is_blend_func(_render_state.blend_src) && is_blend_func(_render_state.blend_dest));
				glBlendFunc(_render_state.blend_src, _render_state.blend_dest);
				current_state.blend_src = _render_state.blend_src;
				current_state.blend_dest = _render_state.blend_dest;
			}
		}
		else
		{
			if (_render_state.uses_blend != current_state.uses_blend)
			{
				glDisable(GL_BLEND);
				current_state.uses_blend = _render_state.uses_blend;
			}
		}
	}

	void render_pass::set_bool(int const p_location, bool const p_value) const
	{
		glUniform1i(p_location, (int)p_value);
	}

	void render_pass::set_int(int const p_location, int const p_value) const
	{
		glUniform1i(p_location, p_value);
	}

	void render_pass::set_float(int const p_location, float const p_value) const
	{
		glUniform1f(p_location, p_value);
	}

	void render_pass::set_float2(int const p_location, float const p_x, float const p_y) const
	{
		glUniform2f(p_location, p_x, p_y);
	}

	void render_pass::set_float2(int const p_location, glm::vec2 const& p_value) const
	{
		glUniform2fv(p_location, 1, &p_value[0]);
	}

	void render_pass::set_float3(int const p_location, float const p_x, float const p_y, float const p_z) const
	{
		glUniform3f(p_location, p_x, p_y, p_z);
	}

	void render_pass::set_float3(int const p_location, glm::vec3 const& p_value) const
	{
		glUniform3fv(p_location, 1, &p_value[0]);
	}

	void render_pass::set_float4(int const p_location, float const p_x, float const p_y, float const p_z, float const p_w) const
	{
		glUniform4f(p_location, p_x, p_y, p_z, p_w);
	}

	void render_pass::set_float4(int const p_location, glm::vec4 const& p_value) const
	{
		glUniform4fv(p_location, 1, &p_value[0]);
	}

	void render_pass::set_mat2(int const p_location, glm::mat2 const& p_value) const
	{
		glUniformMatrix2fv(p_location, 1, GL_FALSE, &p_value[0][0]);
	}

	void render_pass::set_mat3(int const p_location, glm::mat3 const& p_value) const
	{
		glUniformMatrix3fv(p_location, 1, GL_FALSE, &p_value[0][0]);
	}

	void render_pass::set_mat4(int const p_location, glm::mat4 const& p_value) const
	{
		glUniformMatrix4fv(p_location, 1, GL_FALSE, &p_value[0][0]);
	}

	void render_pass::set_texture(int const binding, int value) const
	{
		glActiveTexture(GL_TEXTURE0 + binding);
		glBindTexture(GL_TEXTURE_2D, value);
	}

	void render_pass::set_cubemap(int const binding, int value) const
	{
		glActiveTexture(GL_TEXTURE0 + binding);
		glBindTexture(GL_TEXTURE_CUBE_MAP, value);
	}

	void render_pass::set_ubo(unsigned int const binding, std::string const& r_name) const
	{
		glUniformBlockBinding(m_render_program_id, glGetUniformBlockIndex(m_render_program_id, r_name.c_str()), binding);
	}
}
