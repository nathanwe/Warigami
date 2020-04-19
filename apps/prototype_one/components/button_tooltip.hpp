#pragma once

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>

namespace components
{
	struct button_tooltip : ecs::component<button_tooltip>
	{
		float team{ 1.f };
		std::uint16_t hand_index{ 0 };
	};
}