#ifndef GAME_BOARD_UPDATE_SYSTEM_HPP
#define GAME_BOARD_UPDATE_SYSTEM_HPP

#define ROUND_TIME 1.0f

#include "ecs/state.hpp"
#include <transforms/transform.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>
#include "components/game_piece.hpp"
#include "components/board.hpp"
#include "components/board_square.hpp"

class unit_death_event : public event::Event
{
public:
    explicit unit_death_event(entity_id _id) : event::Event(event::EVENT_TYPE::UNIT_DEATH), id(_id)
    {}

    entity_id id;
};

class board_update : public ecs::system_base, public event::Listener
{
public:

    board_update(
            core::game_input_manager &input,
            core::frame_timer &timer,
            event::EventManager &_event_manager,
            asset::scene_hydrater &_hydrater)
            : m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater)
    {
        event_manager.Subscribe(this, event::EVENT_TYPE::UNIT_DEATH);
    }

    void HandleEvent(event::Event &event) override
    {
        if (event.mType == event::EVENT_TYPE::UNIT_DEATH)
        {
            hydrater.remove_entity(((unit_death_event &) event).id);
        }
    }

    void generate_new_board_state(ecs::state& r_state)
    {
        r_state.each<components::board>([&](components::board& board) {            
            board.clear_state();

            r_state.each<components::game_piece>([&](components::game_piece& piece)
            {
                auto out_of_bounds = 
                    (piece.board_source.y <= 0 && piece.team == -1) ||
                    (piece.board_source.y >= 8 && piece.team == 1);

                if (out_of_bounds) 
                    piece.state = components::UNIT_STATE::DYING;
            });
            
            r_state.each<components::game_piece>([&](components::game_piece& piece) 
            {
                board.board_state[piece.board_source.x][piece.board_source.y] = piece.team;
                piece.remaining_speed = piece.state == components::UNIT_STATE::MOVE ? piece.speed : 0;
                piece.board_destination = piece.board_source;
            });
           
            reconcile_movement_intervals(r_state, board);            
            board.print();
        });        
    }

    void reconcile_movement_intervals(ecs::state& r_state, components::board& board)
    {
        bool changed = true;
        while (changed)
        {
            changed = false;
            r_state.each<components::game_piece>([&](components::game_piece& piece)
            {
                if (piece.remaining_speed <= 0) return;

                glm::ivec2 next_pos = piece.board_destination + piece.move_board;
                if (next_pos.y < 0)
                {
                    piece.remaining_speed = 0;
                    next_pos.y = 0;
                    piece.board_destination = next_pos;
                }
                else if (next_pos.y > 8)
                {
                    piece.remaining_speed = 0;
                    next_pos.y = 8;
                    piece.board_destination = next_pos;
                }
                else if (board.board_state[next_pos.x][next_pos.y] == 0)
                {
                    board.board_state[piece.board_destination.x][piece.board_destination.y] = 0;
                    board.board_state[next_pos.x][next_pos.y] = piece.team;
                    piece.board_destination = next_pos;
                    piece.remaining_speed -= 1;
                    changed = true;
                }
            });
        }
    }

    // Helper function for checking for legal attacks
    static bool check_attacks(glm::ivec2 location, std::vector<glm::ivec2> targets, float teammates, ecs::state &r_state)
    {
        for (auto& t : targets) t += location;

        auto attacked = r_state.first<components::game_piece>([&](components::game_piece& game_piece) {
            for (auto& target : targets)
            {
                if (game_piece.board_source == target && game_piece.team != teammates)
                    return true;
            }

            return false;
        });

        return attacked != nullptr;
    }

    // Helper function for attacking legal attacks
    static void attack_targets(
            glm::ivec2 location,
            std::vector<glm::ivec2> targets,
            int damage,
            int teammates,
            ecs::state &r_state)
    {
        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece &game_piece) {
            for (auto &target : targets)
            {
                if (game_piece.board_source == location + target && game_piece.team != teammates)
                {
                    game_piece.health -= damage;
                }
            }
        });
    }

    // Helper function for moving in board space
    static void move_unit(components::game_piece &game_piece, glm::ivec2 location)
    {
        game_piece.board_source = location;
    }
    //Helper function for getting a board_square
    static ecs::entity* get_square_id_with_loaction(ecs::state& r_state, glm::ivec2 loc) {
        return r_state.first<components::board_square>([&](components::board_square& board_square) {
            return board_square.x == loc.x && board_square.y == loc.y;                 
        });
    }

    // Helper function for moving in board space
    static void walk_unit(components::game_piece& game_piece, float ticker_t)
    {
        glm::vec2 to_dst = game_piece.board_destination - game_piece.board_source;
        glm::vec2 interpolated = glm::vec2(game_piece.board_source) + ticker_t * to_dst;
        game_piece.continuous_board_location = interpolated;
    }

    static void handle_unit_transform(
        transforms::transform& board_t,
        transforms::transform& unit_t,
        components::game_piece& game_piece,
        components::board& board,
        entity_id board_id)
    {
        unit_t.position = board.grid_to_board(game_piece.continuous_board_location, board_t);
        unit_t.scale = glm::vec3(0.2f);
        unit_t.is_matrix_dirty = true;
    }

    void update(ecs::state &r_state) override
    {
        float delta = m_timer.smoothed_delta_secs();

        // Do board combat every 1 second
        timer -= delta;

        float timer_t = (ROUND_TIME - timer) / ROUND_TIME;

        if (timer <= 0.f)
        {
            timer = ROUND_TIME;

            // A unit can only do one thing per turn with the exception that a unit can both attack and die on the same turn
            // Update states to either attack or move depending on current board state
            r_state.each<components::game_piece>([&](components::game_piece &game_piece) 
            {
                game_piece.board_source = game_piece.board_destination;
            });

            r_state.each<components::game_piece>([&](components::game_piece &game_piece) 
            {
                if (check_attacks(game_piece.board_source, game_piece.attacks, game_piece.team, r_state))
                {
                    game_piece.state = components::UNIT_STATE::ATTACK;
                }
                else
                {
                    game_piece.state = components::UNIT_STATE::MOVE;
                }
            });


            // If a unit can attack, attack now
            r_state.each<components::game_piece>([&](components::game_piece& game_piece) 
            {
                if (game_piece.state == components::UNIT_STATE::ATTACK)
                {
                    // Attack animation here
                    attack_targets(
                        game_piece.board_source,
                        game_piece.attacks,
                        game_piece.damage,
                        game_piece.team,
                        r_state);
                }
            });

            generate_new_board_state(r_state);

            // If a unit is standing in fire, it takes damage; (SHOULD GO IN TILE_EFFECT_SYSTEM)
            r_state.each<components::game_piece, transforms::transform>([&](auto& game_piece, auto& transform) {
                ecs::entity* square_e = get_square_id_with_loaction(r_state, game_piece.board_source);
                if (square_e) {
                    auto& square = square_e->get_component<components::board_square>();
                    if (square.terrain_type == terrain::fire) {
                        game_piece.health -= 1; //hardcoded damage, fix                        
                        if (game_piece.health <= 0.f)
                        {
                            game_piece.state = components::UNIT_STATE::DYING;
                        }
                    }
                }
            });

            r_state.each<components::game_piece>([&](components::game_piece& game_piece)
            {                
                if (game_piece.health <= 0)
                    game_piece.state = components::UNIT_STATE::DYING;                
            });
            
        } else
        {
            // If a unit made it through the last state update with a move state, move the unit until the next update
            r_state.each_id<transforms::transform, components::board>(
                [&](entity_id board_id, auto &board_t, auto &board) {
                    r_state.each_id<transforms::transform, components::game_piece>(
                        [&](entity_id unit_id, auto &unit_t, auto &unit) {
                            if (unit.state == components::UNIT_STATE::MOVE)
                            {
                                // Walking animation here
                                walk_unit(unit, timer_t);
                                handle_unit_transform(board_t, unit_t, unit, board, board_id);
                            }
                            if (unit.state == components::UNIT_STATE::DYING)
                            {
                                // Death animation here
                                unit_death_event new_event(unit_id);
                                event_manager.BroadcastEvent(new_event);
                            }
                        });
                    });

        }
    }

private:
    float timer = ROUND_TIME;
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;
};

#endif
