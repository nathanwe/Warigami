#ifndef SET_GAME_PIECE_STATES_SYSTEM_HPP
#define SET_GAME_PIECE_STATES_SYSTEM_HPP


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

class set_game_piece_states : public ecs::system_base, public event::Listener
{
public:

    set_game_piece_states(
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
                [&](entity_id board_id, transforms::transform& board_t, components::board&board) {                    
                if (!(board.state == components::game_state::gameplay || board.state == components::game_state::countdown)) return;

                    if (board.did_tick_elapse) 
                    {
                        death_check(r_state);
                        do_game_piece_actions(r_state);
                    }
                });
    }

private:
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;
    combats::combat_resolution &resolver;  

    void do_game_piece_actions(ecs::state& r_state)
    {
        r_state.each_id<components::game_piece, audio::audio_emitter>([&](entity_id id, components::game_piece& game_piece, audio::audio_emitter& emitter) {
            game_piece.board_source = game_piece.board_destination;
            if (game_piece.state == components::UNIT_STATE::MOVE || game_piece.state == components::UNIT_STATE::ATTACK
                || (game_piece.state == components::UNIT_STATE::STUN && game_piece.stun_duration == 0))
            {
                bool attacking = check_attacks(game_piece.board_destination, game_piece.attacks, game_piece.team, r_state);
                if (attacking) {
                    game_piece.state = components::UNIT_STATE::ATTACK;
                    emitter.set_sound_state(0, audio::sound_state::playback_requested);
                }
                else {
                    game_piece.state = components::UNIT_STATE::MOVE;
                }
            }

            });
    }
 

    void death_check(ecs::state& r_state) {
        r_state.each_id<components::game_piece, audio::audio_emitter>([&](entity_id id, components::game_piece& game_piece, audio::audio_emitter& emitter) {
            if (game_piece.health <= 0 && (game_piece.state == components::UNIT_STATE::MOVE || game_piece.state == components::UNIT_STATE::ATTACK || game_piece.state == components::UNIT_STATE::STUN))
            {
                game_piece.state = components::UNIT_STATE::DYING;
                emitter.set_sound_state(1, audio::sound_state::playback_requested);
            }

            });
    }

   
    // Helper function for checking for legal attacks
    static bool
    check_attacks(glm::ivec2 location, std::vector<glm::ivec2> targets, float teammates, ecs::state &r_state)
    {
        for (auto &t : targets) t += location;

        auto attacked = r_state.first<components::game_piece>([&](components::game_piece &game_piece) {
            for (auto &target : targets)
            {

                if (game_piece.board_destination == target && game_piece.team != teammates && 
                    (game_piece.state == components::UNIT_STATE::MOVE || game_piece.state == components::UNIT_STATE::ATTACK || game_piece.state == components::UNIT_STATE::STUN))
                {
                    return true;
                }
            }

            return false;
        });

        return attacked != nullptr;
    }

};

#endif
