#ifndef GAME_BOARD_UPDATE_SYSTEM_HPP
#define GAME_BOARD_UPDATE_SYSTEM_HPP


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


class board_update : public ecs::system_base, public event::Listener
{
public:

    board_update(
            core::game_input_manager &input,
            core::frame_timer &timer,
            event::EventManager &_event_manager,
            asset::scene_hydrater &_hydrater,
            combats::combat_resolution &_resolver)
            : m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater), resolver(_resolver)
    {
        
    }

    void HandleEvent(event::Event& event) override
    {
    }

    void update(ecs::state &r_state) override
    {
        r_state.each_id<transforms::transform, components::board>(
                [&](entity_id board_id, transforms::transform& board_t, components::board&board) {                    
                if (!(board.state == components::game_state::gameplay || board.state == components::game_state::countdown)) return;

                    if (board.did_tick_elapse)
                        handle_tick_end(r_state, board_id, board_t, board);
                });
    }

private:
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;
    combats::combat_resolution &resolver;


    void
    handle_tick_end(ecs::state &r_state, entity_id board_id, transforms::transform &board_t, components::board &board)
    {
   
        resolver.Resolve_Combats();
        generate_new_board_state(r_state);
    }    

     

    //Helper function for getting a board_square
    static ecs::entity *get_square_at_location(ecs::state &r_state, glm::ivec2 loc)
    {
        return r_state.first<components::board_square>([&](components::board_square &board_square) {
            return board_square.x == loc.x && board_square.y == loc.y;
        });
    }
  

    void generate_new_board_state(ecs::state &r_state)
    {
        r_state.each<components::board>([&](components::board &board) {
            board.clear_state();

            

            r_state.each<components::game_piece>([&](components::game_piece &piece) {
                out_of_bounds_effects(r_state, piece);
            });

            r_state.each<components::game_piece>([&](components::game_piece &piece) {
                board.board_state[piece.board_source.x][piece.board_source.y] = piece.team;
                piece.remaining_speed = piece.state == components::UNIT_STATE::MOVE ? piece.speed : 0;
                piece.board_destination = piece.board_source;
            });

            reconcile_movement_intervals(r_state, board);
            //claim_territory(r_state, board);

            //board.print();
        });
    }

    void out_of_bounds_effects(ecs::state& r_state, components::game_piece& piece) {
        r_state.each<components::player>([&](auto& p) {
            if (p.team != piece.team)
            {
                if ((piece.board_source.y >= p.score_column && p.team == -1) ||
                    (piece.board_source.y <= p.score_column && p.team == 1)) {

                    piece.state = components::UNIT_STATE::DANCING;
                    p.health -= piece.damage * 10.f;
                    if (p.health < 0.f)
                    {
                        p.health = 0.f;
                    }
                }
            }
            });
    }

    static void reconcile_movement_intervals(ecs::state &r_state, components::board &board)
    {
        bool changed = true;
        while (changed)
        {
            changed = false;
            r_state.each<components::game_piece>([&](components::game_piece &piece) {
                ecs::entity* foo = get_square_at_location(r_state, piece.board_destination);
                foo->get_component<components::board_square>().next_team = piece.team;
                
                if (piece.remaining_speed <= 0) return;

                glm::ivec2 next_pos = piece.board_destination + piece.move_board;
                if (next_pos.y < 0)
                {
                    piece.remaining_speed = 0;
                    next_pos.y = 0;
                    piece.board_destination = next_pos;
                } else if (next_pos.y > board.rows -1)
                {
                    piece.remaining_speed = 0;
                    next_pos.y = board.rows -1;
                    piece.board_destination = next_pos;
                } else if (board.board_state[next_pos.x][next_pos.y] == 0)
                {
                    board.board_state[piece.board_destination.x][piece.board_destination.y] = 0;
                    board.board_state[next_pos.x][next_pos.y] = piece.team;
                    piece.board_destination = next_pos;                                    
                    piece.remaining_speed--;
                    changed = true;
                }
            });
        }
    }
    static void claim_territory(ecs::state& r_state, components::board& board) {
        r_state.each<components::board_square>([&](auto& square) {
            float foo = board.board_state[square.x][square.y];
            if (foo != 0) {
                square.team = foo;
            }
            });
    }
    
};

#endif
