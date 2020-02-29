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

    // Helper function for checking for legal attacks
    static bool check_attacks(glm::vec2 location, std::vector<glm::vec2> targets, float teammates, ecs::state &r_state)
    {
        bool ret = false;
        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece &game_piece) {
            for (auto &target : targets)
            {
                glm::vec2 attack_location = location + target;
                if (game_piece.board_location == attack_location && game_piece.team != teammates &&
                    game_piece.state != components::UNIT_STATE::DYING &&
                    game_piece.state != components::UNIT_STATE::DEAD)
                {
                    ret = true;
                }
            }
        });

        return ret;
    }

    // Helper function for attacking legal attacks
    static void attack_targets(
            glm::vec2 location,
            std::vector<glm::vec2> targets,
            float damage,
            float teammates,
            ecs::state &r_state)
    {
        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece &game_piece) {
            for (auto &target : targets)
            {
                if (game_piece.board_location == location + target &&
                    game_piece.state != components::UNIT_STATE::DYING && game_piece.team != teammates)
                {
                    game_piece.health -= damage;

                    if (game_piece.health <= 0.f)
                    {
                        game_piece.state = components::UNIT_STATE::DYING;
                    }
                }
            }
        });
    }

    // Helper function for checking for legal moves
    static glm::vec2 move_check(entity_id id, glm::vec2 start, glm::vec2 end, float teammates, ecs::state &r_state)
    {
        int start_loc = static_cast<int>(start.y);
        int end_loc = static_cast<int>(end.y);
        glm::vec2 ret = start;

        if (end_loc > start_loc)
        {
            for (int current_loc = start_loc; current_loc <= end_loc; current_loc++)
            {
                r_state.each_id<components::game_piece>(
                        [&](entity_id id_two, components::game_piece &game_piece) {
                            if (game_piece.board_location.x == ret.x && id != id_two)
                            {
                                if (current_loc == game_piece.board_location.y)
                                {
                                    return;
                                } else
                                {
                                    ret.y = current_loc;
                                }
                            } else
                            {
                                ret.y = current_loc;
                            }
                        });
            }
        } else
        {
            for (int current_loc = start_loc; current_loc >= end_loc; current_loc--)
            {
                r_state.each_id<components::game_piece>(
                        [&](entity_id id_two, components::game_piece &game_piece) {
                            if (game_piece.board_location.x == ret.x && id != id_two)
                            {
                                if (current_loc == game_piece.board_location.y)
                                {
                                    return;
                                } else
                                {
                                    ret.y = current_loc;
                                }
                            }
                        });
            }
        }

        return ret;
    }

    // Helper function for moving in board space
    static void move_unit(components::game_piece &game_piece, glm::vec2 location)
    {
        game_piece.board_location = location;
    }

    void update(ecs::state &r_state) override
    {
        float delta = m_timer.smoothed_delta_secs();

        // Do board combat every 1 second
        timer -= delta;
        if (timer <= 0.f)
        {
            timer = ROUND_TIME;

            // A unit can only do one thing per turn with the exception that a unit can both attack and die on the same turn

            std::cerr << "One: " << std::endl;

            // Update states to either attack or move depending on current board state
            r_state.each_id<components::game_piece, transforms::transform>(
                    [&](entity_id id, components::game_piece &game_piece, transforms::transform &transform) {
                        if (game_piece.state == components::UNIT_STATE::DYING ||
                            game_piece.state == components::UNIT_STATE::DEAD)
                        {
                            game_piece.state = components::UNIT_STATE::DEAD;
                        } else
                        {
                            if (check_attacks(game_piece.board_location, game_piece.attacks, game_piece.team, r_state))
                            {
                                game_piece.state = components::UNIT_STATE::ATTACK;
                            } else
                            {
                                game_piece.state = components::UNIT_STATE::MOVE;
                            }
                        }
                    });

            std::cerr << "Two: " << std::endl;

            // If a unit can attack, attack now
            r_state.each_id<components::game_piece, transforms::transform>(
                    [&](entity_id id, components::game_piece &game_piece, transforms::transform &transform) {
                        if (game_piece.state == components::UNIT_STATE::ATTACK ||
                            game_piece.state == components::UNIT_STATE::DYING)
                        {
                            // Attack animation here
                            attack_targets(game_piece.board_location, game_piece.attacks, game_piece.damage,
                                           game_piece.team, r_state);
                            std::cerr << "Unit: " << id << " attacking" << std::endl;
                        }
                    });


            std::cerr << "Three: " << std::endl;


            // If a unit is still alive after attacks and has a move state, check for legality and then move
            r_state.each_id<components::game_piece, transforms::transform>(
                    [&](entity_id id, components::game_piece &game_piece, transforms::transform &transform) {
                        if (game_piece.state == components::UNIT_STATE::MOVE)
                        {
                            glm::vec2 movement = move_check(id, game_piece.board_location,
                                                            game_piece.board_location + game_piece.move_board,
                                                            game_piece.team, r_state);
                            if (movement == game_piece.board_location)
                            {
                                game_piece.state = components::UNIT_STATE::WAIT;
                                std::cerr << "Unit: " << id << " waiting" << std::endl;
                            } else
                            {
                                move_unit(game_piece, movement);
                                std::cerr << "Unit: " << id << " moved to " << movement.x << ", " << movement.y
                                          << std::endl;
                                if (game_piece.board_location.y < 0.f || game_piece.board_location.y > 9.f)
                                {
                                    std::cerr << "Unit: " << id << " scored" << std::endl;
                                    game_piece.state = components::UNIT_STATE::DYING;
                                }
                            }
                        }
                    });
        }



        // If a unit made it through the last state update with a move state, move the unit until the next update
        r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, auto& board) {
            std::cerr << "Four: " << std::endl;

            r_state.each_id<transforms::transform, components::game_piece>([&](entity_id unit_id, auto& unit_t, auto& unit) {
                if (unit.state == components::UNIT_STATE::MOVE)
                {
                    // Walking animation here
                    walk_unit(unit, delta);
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

        std::cerr << "Five: " << std::endl;

    }

private:
    float timer = ROUND_TIME;
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;



    // Helper function for moving in board space
    static void walk_unit(components::game_piece& game_piece, float delta)
    {
        game_piece.board_location += delta * game_piece.speed * game_piece.move_board;
        if (game_piece.board_location.y < 0) game_piece.board_location.y = 0;
        if (game_piece.board_location.y > 9) game_piece.board_location.y = 9;
    }

    static void handle_unit_transform(
        transforms::transform& board_t,
        transforms::transform& unit_t,
        components::game_piece& game_piece,
        components::board& board,
        entity_id board_id)
    {
        unit_t.has_parent = true;
        unit_t.parent = board_id;
        unit_t.position = grid_to_world(game_piece.board_location, board, board_t);
        unit_t.is_matrix_dirty = true;
    }

    static glm::vec3 grid_to_world(glm::vec2& grid, components::board& board, transforms::transform& board_t)
    {
        auto width_scaled = board_t.scale.x * board.cell_width;
        auto height_scaled = board_t.scale.y * board.cell_height;

        auto half_w = board.rows * width_scaled / 2.f;
        auto half_h = board.columns * height_scaled / 2.f;

        return 
            glm::vec3(grid.x * width_scaled, 1, grid.y * height_scaled) 
            - glm::vec3(half_w, 0, half_h);
    }
};

#endif
