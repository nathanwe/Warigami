#ifndef ADD_COMBATS_TO_RESOLVER_SYSTEM_HPP
#define ADD_COMBATS_TO_RESOLVER_SYSTEM_HPP


#include "ecs/state.hpp"
#include <transforms/transform.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>
#include "components/game_piece.hpp"
#include "components/board.hpp"
#include "components/board_square.hpp"
#include "combat.hpp"
#include "components/card_enum.hpp"
#include "components/terrain.hpp"
#include "components/to_spawn.hpp"
#include <algorithm>
#include <math.h>

class add_combats_to_resolver : public ecs::system_base, public event::Listener
{
public:

    add_combats_to_resolver(
            core::game_input_manager &input,
            core::frame_timer &timer,
            event::EventManager &_event_manager,
            asset::scene_hydrater &_hydrater,
            combats::combat_resolution &_resolver)
            : m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater), resolver(_resolver)
    {
        
    }

    void HandleEvent(event::Event &event) override
    {
        
    }

    void update(ecs::state &r_state) override
    {
        r_state.each_id<transforms::transform, components::board>(
                [&](entity_id board_id, transforms::transform& board_t, components::board&board) 
                {                    
                    if (board.state != components::game_state::gameplay) return;

                    if (board.did_tick_elapse) 
                    {
                        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece& game_piece) 
                            {

                                if (game_piece.state == components::UNIT_STATE::ATTACK)
                                {
                                    // Attack animation here
                                    attack_targets(
                                        game_piece,
                                        id,
                                        game_piece.board_source,
                                        game_piece.attacks,
                                        game_piece.team,
                                        r_state);
                                }
                            });
                    }
                });
    }

private:
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;
    combats::combat_resolution &resolver;  
    

    // Helper function for attacking legal attacks
    void attack_targets(
            components::game_piece &attacker,
            entity_id attacker_id,
            glm::ivec2 location,
            std::vector<glm::ivec2> targets,
            int teammates,
            ecs::state &r_state)
    {
        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece &game_piece) {
            for (auto &target : targets)
            {
                if (game_piece.board_source == location + target && game_piece.team != teammates)
                {
                    bool duplicate = false;
                    for (auto &comb : resolver.Get_Combats())
                    {
                        if (comb.id_one == id && comb.id_two == attacker_id)
                        {
                            comb.retaliation = true;
                            duplicate = true;
                        }
                    }
                    if (!duplicate)
                    {
                        resolver.Add_Combat(attacker, attacker_id, game_piece, id);
                    }
                }
            }
        });
    }
};

#endif
