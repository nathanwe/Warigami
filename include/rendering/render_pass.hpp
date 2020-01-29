#ifndef WIZARDPEOPLE_RENDERING_RENDER_PASS_HPP
#define WIZARDPEOPLE_RENDERING_RENDER_PASS_HPP

#include "glm/glm.hpp"

#include <string>

namespace rendering
{
	class render_pass
	{
	public:
		render_pass(char const* vertex_filepath, char const* fragment_filepath);
		~render_pass();

	public:
		void bind() const;
		int get_parameter_location(std::string const& name) const;
		void set_bool(int const location, bool const value) const;
		void set_int(int const location, int const value) const;
		void set_float(int const location, float const value) const;
		void set_float2(int const location, float const x, float const y) const;
		void set_float2(int const location, glm::vec2 const& value) const;
		void set_float3(int const location, float const x, float const y, float const z) const;
		void set_float3(int const location, glm::vec3 const& value) const;
		void set_float4(int const location, float const x, float const y, float const z, float const w) const;
		void set_float4(int const location, glm::vec4 const& value) const;
		void set_mat2(int const location, glm::mat2 const& value) const;
		void set_mat3(int const location, glm::mat3 const& value) const;
		void set_mat4(int const location, glm::mat4 const& value) const;
		void set_texture(int const binding, int value) const;
		void set_cubemap(int const binding, int value) const;
		void set_ubo(unsigned int const binding, std::string const& name) const;

	private:
		unsigned int m_render_program_id = 0;
	};
}

#endif
