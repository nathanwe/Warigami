#ifndef __FLASH_STEP_SYSTEM_HPP_
#define __FLASH_STEP_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <algorithm>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "combat.hpp"
#include "components/game_piece.hpp"


class flash_step_system : public ecs::system_base
{
public:
	flash_step_system()
	{}

	void update(ecs::state& state)
	{
		// Skip system when paused
		auto e = state.first<components::pause>([](auto& pause)
			{
				return true;
			});
		if (e && e->get_component<components::pause>().is_game_paused)
		{
			return;
		}
		state.each_id<transforms::transform, components::board>(
			[&](entity_id board_id, transforms::transform& board_t, components::board& board) {


				if (board.did_tick_elapse)
				{
					state.each<components::game_piece, transforms::transform>([&]
					(components::game_piece& piece, transforms::transform& transform) {
							if (piece.flash_step_flag) {
								piece.flash_step_flag = false;

								if (piece.board_source.x > 0 &&
									piece.board_destination.x > 0 &&
									board.board_state[piece.board_source.x][piece.board_source.y] == 0) 
								{
									piece.board_source.x--;
									piece.board_destination.x--;
									piece.continuous_board_location.x--;
									transform.position = board.grid_to_board(piece.continuous_board_location, board_t);					
									transform.is_matrix_dirty = true;
								}
							}
						});
				}



			});

	}

private:

};

#endif