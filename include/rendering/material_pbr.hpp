#ifndef WIZARDPEOPLE_RENDERING_MATERIAL_PBR_HPP
#define WIZARDPEOPLE_RENDERING_MATERIAL_PBR_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>

namespace rendering
{
	struct material_pbr
	{
		std::uint32_t texture_diffuse;
		std::uint32_t texture_metalness;
		std::uint32_t texture_roughness;
		std::uint32_t texture_normal;
		std::uint32_t texture_ambient_occlusion;

		glm::vec2 texture_scale;
		glm::vec2 texture_offset;

		glm::vec3 param_diffuse;
		glm::vec3 param_metalness;
		float param_roughness;
	};
}

#endif
