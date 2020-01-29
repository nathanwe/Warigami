#ifndef WIZARDPEOPLE_RENDERING_LIGHT_DIRECTIONAL_HPP
#define WIZARDPEOPLE_RENDERING_LIGHT_DIRECTIONAL_HPP

#include "ecs/component.hpp"
#include "rendering/component_bits.hpp"

#include "glm/glm.hpp"

namespace rendering
{
	struct light_directional : ecs::component<light_directional>
	{
		float intensity = 1;
		glm::vec3 color = glm::vec3(1);

		bool is_shadow_caster        = false;
		std::uint32_t shadow_texture = 0;
		glm::mat4 shadow_view        = glm::mat4(1);
		glm::mat4 shadow_projection  = glm::mat4(1);
	};
}

#endif
