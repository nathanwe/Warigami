#ifndef WIZARDPEOPLE_RENDERING_MATERIAL_PBR_HPP
#define WIZARDPEOPLE_RENDERING_MATERIAL_PBR_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>

namespace rendering
{
	struct material_pbr
	{
		std::uint32_t texture_diffuse           = 0;
		std::uint32_t texture_metalness         = 0;
		std::uint32_t texture_roughness         = 0;
		std::uint32_t texture_normal            = 0;
		std::uint32_t texture_ambient_occlusion = 0;

		glm::vec2 texture_scale  = glm::vec2(1);
		glm::vec2 texture_offset = glm::vec2(0);

		glm::vec3 param_diffuse   = glm::vec3(0.96);
		glm::vec3 param_metalness = glm::vec3(0.04);
		float param_roughness     = 0.5f;
	};
}

#endif
