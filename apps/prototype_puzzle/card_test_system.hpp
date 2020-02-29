#ifndef CARD_TEST_SYSTEM_HPP
#define CARD_TEST_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <rendering/camera.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <collisions/collision_manager.hpp>
#include "components/game_piece.hpp"
#include "components/hit_points.hpp"
#include "components/player.hpp"
#include <glm/glm.hpp>
#include <stdio.h>


class card_test_system : public ecs::system_base
{
public:
	card_test_system(core::frame_timer& timer) : m_timer(timer) {}

	virtual void update(ecs::state& r_state) override
	{
		components::card_enum card_drawn;
		r_state.each_id<components::player>([&](
			entity_id id,
			components::player& player)
			{
				
				card_drawn = player.draw();
				if (card_drawn == components::card_enum::NO_CARD) {
					player.regrow_deck();
					player.shuffle();
				}
				std::cout << (int)card_drawn << std::endl;
			});

	}

private:
	core::frame_timer& m_timer;
};

#endif
