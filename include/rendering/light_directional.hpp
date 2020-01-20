#ifndef WIZARDPEOPLE_RENDERING_LIGHT_DIRECTIONAL_HPP
#define WIZARDPEOPLE_RENDERING_LIGHT_DIRECTIONAL_HPP

#include "ecs/component.hpp"
#include "rendering/component_bits.hpp"

#include "glm/glm.hpp"

namespace rendering
{
	struct light_directional : ecs::component<light_directional>
	{
		float intensity;
		glm::vec3 color;

		bool is_shadow_caster;
		std::uint32_t shadow_texture;
		glm::mat4 shadow_view;
		glm::mat4 shadow_projection;
	};
}

template<> const component_shift ecs::component<rendering::light_directional>::component_bitshift = (component_shift)rendering::component_bits::light_directional;

#endif
