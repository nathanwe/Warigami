#ifndef WIZARDPEOPLE_RENDERING_LIGHT_POINT_HPP
#define WIZARDPEOPLE_RENDERING_LIGHT_POINT_HPP

#include "ecs/component.hpp"
#include "rendering/component_bits.hpp"

#include "glm/vec3.hpp"

namespace rendering
{
	struct light_point : ecs::component<light_point>
	{
		float radius;
		float intensity;
		glm::vec3 color;
	};

}

template<> const component_shift ecs::component<rendering::light_point>::component_bitshift = (component_shift)rendering::component_bits::light_point;

#endif
