#ifndef INTERPOLATION_SYSTEM_HPP
#define INTERPOLATION_SYSTEM_HPP


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

class interpolation_system : public ecs::system_base, public event::Listener
{
public:

    interpolation_system(
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
            [&](entity_id board_id, transforms::transform& board_t, components::board& board) {
                if (board.state != components::game_state::gameplay) return;

                if (!board.did_tick_elapse) 
                {
                    handle_tick_progress(r_state, board_id, board_t, board);
                }
                });
    }

private:
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;
    combats::combat_resolution &resolver;  

    void handle_tick_progress(
        ecs::state &r_state, 
        entity_id board_id, 
        transforms::transform &board_t,
        components::board &board)
    {
        r_state.each_id<transforms::transform, components::game_piece>(
                [&](entity_id unit_id, auto &unit_t, auto &unit) {
                    switch (unit.state)
                    {
                        case components::UNIT_STATE::ATTACK:
                        {
                            attack_unit(unit, board.timer_t);
                            handle_unit_transform(board_t, unit_t, unit, board, board_id);
                            break;
                        }
                        case components::UNIT_STATE::MOVE:
                        {
                            walk_unit(unit, board.timer_t);
                            handle_unit_transform(board_t, unit_t, unit, board, board_id);
                            break;
                        }
                        case components::UNIT_STATE::DYING:
                        {
                            fall_unit(unit, board.timer_t);
                            handle_unit_transform(board_t, unit_t, unit, board, board_id);
                            break;
                        }
                        case components::UNIT_STATE::DANCING:
                        {
                            dance_unit(unit, board.timer_t);
                            handle_unit_transform(board_t, unit_t, unit, board, board_id);
                            break;
                        }
                    }
                });
    }

    // Helper function for moving in board space
    static void walk_unit(components::game_piece &game_piece, float ticker_t)
    {
        glm::vec2 to_dst = game_piece.board_destination - game_piece.board_source;
        glm::vec2 overstep = glm::vec2(game_piece.board_destination) + (to_dst) * 0.2f;

        std::pair<float, glm::vec2> position_keyframes[]{ // t -> y
                {std::numeric_limits<float>::min(), game_piece.board_source},
                {0.f,                               game_piece.board_source},
                {0.6f,                              game_piece.board_source},
                {0.9f,                              overstep},
                {1.f,                               game_piece.board_destination},
                {std::numeric_limits<float>::max(), game_piece.board_destination} };

        size_t index = 0;
        while (position_keyframes[index].first < ticker_t)
            index++;

        auto source = position_keyframes[index - 1].second;
        auto destination = position_keyframes[index].second;
        float t_first = position_keyframes[index - 1].first;
        float t_second = position_keyframes[index].first;
        float t_range = t_second - t_first;
        float t = (ticker_t - t_first) / t_range;

        glm::vec2 interpolated = glm::vec2(game_piece.board_source) + t * to_dst;
        game_piece.continuous_board_location = source + t * (destination - source);
    }
    // Helper function for dieing in board space
    static void fall_unit(components::game_piece& game_piece, float ticker_t)
    {       
        game_piece.continuous_board_rotation.z = ticker_t;
    }
    // Helper function for danceing in board space
    static void dance_unit(components::game_piece& game_piece, float ticker_t)
    {
        game_piece.continuous_board_rotation.x = .5 * sinf(ticker_t * 10);
    }

    // Helper function for attacking
    static void attack_unit(components::game_piece &game_piece, float ticker_t)
    {
        static std::pair<float, float> position_keyframes[]{ // t -> y
                {std::numeric_limits<float>::min(), 0.f},
                {0.f,                               0.f},
                {0.4f,                              0.0f},
                {0.6f,                              -0.1f},
                {0.7f,                              0.2f},
                {1.f,                               0.f},
                {std::numeric_limits<float>::max(), 0.f},
        };

        size_t index = 0;
        while (position_keyframes[index].first < ticker_t)
            index++;

        float source = position_keyframes[index - 1].second;
        float destination = position_keyframes[index].second;
        float t_first = position_keyframes[index - 1].first;
        float t_second = position_keyframes[index].first;
        float t_range = t_second - t_first;
        float t = (ticker_t - t_first) / t_range;
        game_piece.continuous_board_location.y = game_piece.board_source.y +
                                                 (source + t * (destination - source)) * game_piece.team;

    }

    static void handle_unit_transform(
            transforms::transform &board_t,
            transforms::transform &unit_t,
            components::game_piece &game_piece,
            components::board &board,
            entity_id board_id)
    {
        auto new_position = board.grid_to_board(game_piece.continuous_board_location, board_t);
        unit_t.position = new_position;
        unit_t.rotation = game_piece.continuous_board_rotation;
        //unit_t.scale = glm::vec3(0.2f); //I would like changig the scale in the json to do somthing
        unit_t.is_matrix_dirty = true;

        // Change transform of health spheres
        int i = 0;
        for (ecs::entity e : game_piece.health_points) {
            transforms::transform& health_t = e.get_component<transforms::transform>();
            health_t.position = glm::vec3(0, 0, 0);
            health_t.position.x += (i - game_piece.max_health/2)*0.5;
            health_t.is_matrix_dirty = true;
            i++;
        }

        // Move projectiles towards their end destination
        // Projectiles are stores as entity-destination pairs
        for (std::pair<ecs::entity, glm::vec3> p : game_piece.projectiles) {
            transforms::transform& projectile_t = p.first.get_component<transforms::transform>();
            if (distance_squared(projectile_t.position, p.second) > 0.3) {
                projectile_t.position = moveTowards(projectile_t.position, p.second, 0.2);
                projectile_t.is_matrix_dirty = true;
            }
            else {
                // Hide until deletion (deleted with unit)
                projectile_t.position.z = 500;
                projectile_t.is_matrix_dirty = true;
            }
        }
    }

    static float distance_squared(glm::vec3 v1, glm::vec3 v2) {
        return powf(v1.x-v2.x, 2) + powf(v1.y - v2.y, 2) + powf(v1.z - v2.z, 2);
    }

    static glm::vec3 moveTowards(glm::vec3 from, glm::vec3 to, float speed) {
        
        glm::vec3 new_pos = from;
        if (from.x > to.x) {
            new_pos.x -= speed;
        }
        else if (from.x < to.x) {
            new_pos.x += speed;
        }

        if (from.y > to.y) {
            new_pos.y -= speed;
        }
        else if (from.y < to.y) {
            new_pos.y += speed;
        }

        if (from.z > to.z) {
            new_pos.z -= speed;
        }
        else if (from.z < to.z) {
            new_pos.z += speed;
        }

        return new_pos;
    }
};

#endif
