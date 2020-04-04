#pragma once

#include <ecs/component.hpp>
#include <ecs/ecs_types.hpp>

namespace components
{
	struct main_menu : public ecs::component<main_menu>
	{
		static const int NoPage = -1;

		size_t selection{ 0 };
		int how_to_page{ NoPage };

		entity_id how_to_play_images[4]{ 104, 105, 106, 107 };
	};
}