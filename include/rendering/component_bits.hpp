#ifndef WIZARDPEOPLE_RENDERING_COMPONENT_BITS_HPP
#define WIZARDPEOPLE_RENDERING_COMPONENT_BITS_HPP

#include "ecs/ecs_types.hpp"

namespace rendering
{
	enum class component_bits : component_shift
	{
		light_directional,
		light_point,
		renderable_mesh_static,
		camera
	};
}

#endif