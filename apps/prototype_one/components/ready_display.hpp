#ifndef __READY_DISPLAY_HPP_
#define __READY_DISPLAY_HPP_

#include <ecs/component.hpp>
#include <ecs/ecs_types.hpp>

namespace components
{
	struct ready_display : public ecs::component<ready_display>
	{
		bool is_ready{ false };

		entity_id ready_entity;
		entity_id back_entity;

		float team{ 1.f };

		std::uint32_t child_count = 0;
		entity_id children[32];

		std::uint32_t p1_units_count = 0;
		entity_id p1_units[32];
		
		std::uint32_t p2_units_count = 0;
		entity_id p2_units[32];

		float button_x;
		float button_y{ -9.5f };
		float button_z{ -24.5f };
		
	};
}


#endif