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

		size_t num_howto_pages{ 5 };
		entity_id how_to_play_images[5]{ 104, 105, 106, 107, 108 };
	};
}