#ifndef WIZARDPEOPLE_RENDERING_COMPONENT_BITS_HPP
#define WIZARDPEOPLE_RENDERING_COMPONENT_BITS_HPP

#include "ecs/ecs_types.hpp"

namespace rendering
{
	enum class component_bits : component_shift
	{
		camera = 1,
		light_directional = 2,
		light_point = 3,
		renderable_mesh_static = 4,
		renderable_model_static = 5,
		renderable_mesh_rigged = 6,
		renderable_text = 7
	};
}

#endif
