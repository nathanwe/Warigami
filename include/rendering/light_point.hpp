#ifndef WIZARDPEOPLE_RENDERING_LIGHT_POINT_HPP
#define WIZARDPEOPLE_RENDERING_LIGHT_POINT_HPP

#include "ecs/component.hpp"
#include "rendering/component_bits.hpp"

#include "glm/vec3.hpp"

namespace rendering
{
	struct light_point : ecs::component<light_point>
	{
		float radius    = 1;
		float intensity = 1;
		glm::vec3 color = glm::vec3(1);
	};

}

#endif
