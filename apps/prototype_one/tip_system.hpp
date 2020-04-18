#ifndef TIP_SYSTEM_HPP
#define TIP_SYSTEM_HPP


#include "ecs/state.hpp"
#include <transforms/transform.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>
#include "components/game_piece.hpp"
#include "components/board.hpp"
#include "components/board_square.hpp"
#include "components/tip.hpp"
#include "combat.hpp"
#include "components/card_enum.hpp"
#include "components/terrain.hpp"
#include "components/to_spawn.hpp"
#include <algorithm>
#include <math.h>

class tip_system : public ecs::system_base
{
public:

    tip_system(
        core::frame_timer& timer
      )
        : m_timer(timer)
    {
    }


    void update(ecs::state& r_state) override
    {
        r_state.each_id<transforms::transform, components::board>(
            [&](entity_id board_id, transforms::transform& board_t, components::board& board) {
                if (board.state != components::game_state::gameplay) return;

                r_state.each_id<transforms::transform, components::tip>(
                    [&](entity_id tip_id, transforms::transform& tip_t, components::tip& tip) {
                        r_state.each_id<transforms::transform, components::board_square, components::terrain>(
                            [&](entity_id board_square_id, transforms::transform& square_t, components::board_square& square, components::terrain& terrain) {
                                if ((terrain.growth_stage == 3 || terrain.growth_stage == 2) 
                                    && (tip.team == square.team)) 
                                {
                                    tip_t.position = board.grid_to_board(glm::vec2(square.x, square.y), board_t);
                                    tip_t.is_matrix_dirty = true;
                                }

                            });

                    });
                
            });
    }

private:
    core::frame_timer& m_timer;
};

#endif