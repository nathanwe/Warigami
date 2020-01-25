#include "rendering/render_pass.hpp"

#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace gl;

namespace rendering
{
	void verify_shader_compilation(unsigned int shader_id, char const* filepath);
	void verify_shader_linkage(unsigned int render_program_id);

	render_pass::render_pass(char const* vertex_filepath, char const* fragment_filepath)
	{
		unsigned int vsId = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fsId = glCreateShader(GL_FRAGMENT_SHADER);

		// if path to glsl, else path to precompiled binary
		if (1)
		{
			// Load shaders as text
			std::ifstream vsFile, fsFile;
			std::stringstream vsStringstream, fsStringstream;
#ifdef _DEBUG
			vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
#endif
				//std::ifstream vsFile(vertex_filepath);
				//std::ifstream fsFile(fragment_filepath);
				vsFile.open(vertex_filepath);
				fsFile.open(fragment_filepath);
				vsStringstream << vsFile.rdbuf();
				fsStringstream << fsFile.rdbuf();
				vsFile.close();
				fsFile.close();
#ifdef _DEBUG
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "Error: Failed to read shader from source file. Log: " << e.what() << std::endl;
			}
#endif

			std::string vsText, fsText;
			vsText = vsStringstream.str();
			fsText = fsStringstream.str();
			char const* vsTextCstr = vsText.c_str();
			char const* fsTextCstr = fsText.c_str();

			// Compile shaders
			glShaderSource(vsId, 1, &vsTextCstr, NULL);
			glCompileShader(vsId);
#ifdef _DEBUG
			verify_shader_compilation(vsId, vertex_filepath);
#endif
			glShaderSource(fsId, 1, &fsTextCstr, NULL);
			glCompileShader(fsId);
#ifdef _DEBUG
			verify_shader_compilation(fsId, fragment_filepath);
#endif
			// write compiled binary to file
			// write binary format to metadata file
		}
		else
		{
			// Load precompiled shader binaries
			//
			// Get binary format from metadata
			//
			//glShaderBinary(1, vsId, , , );
		}

		// Connect shaders to render pass
		m_render_program_id = glCreateProgram();
		glAttachShader(m_render_program_id, vsId);
		glAttachShader(m_render_program_id, fsId);
		glLinkProgram(m_render_program_id);
#ifdef _DEBUG
		verify_shader_linkage(m_render_program_id);
#endif

		glDetachShader(m_render_program_id, vsId);
		glDetachShader(m_render_program_id, fsId);
		glDeleteShader(vsId);
		glDeleteShader(fsId);
	}

	void verify_shader_compilation(unsigned int shader_id, char const* filepath)
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

	void verify_shader_linkage(unsigned int renderPassId)
	{
		GLint result;
		glGetProgramiv(renderPassId, GL_LINK_STATUS, &result);
		if (!result)
		{
			GLchar infoLog[1024];
			glGetProgramInfoLog(renderPassId, 1024, NULL, infoLog);
			std::cout << "Error: Render pass failed to link shaders. OpenGL logs:\n" << infoLog << std::endl;
		}
	}

	int render_pass::get_parameter_location(std::string const& p_r_name) const
	{
		return glGetUniformLocation(m_render_program_id, p_r_name.c_str());
	}

	void render_pass::bind() const
	{
		glUseProgram(m_render_program_id);
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
