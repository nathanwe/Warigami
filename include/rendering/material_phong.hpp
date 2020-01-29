#ifndef WIZARDPEOPLE_RENDERING_MATERIAL_PHONG_HPP
#define WIZARDPEOPLE_RENDERING_MATERIAL_PHONG_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>

namespace rendering
{
	struct material_phong
	{
		std::uint32_t texture    = 0;
		glm::vec2 texture_scale  = glm::vec2(1);
		glm::vec2 texture_offset = glm::vec2(0);
		glm::vec3 color          = glm::vec3(1);
	};
}

#endif
