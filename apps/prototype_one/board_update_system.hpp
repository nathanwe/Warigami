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
            asset::scene_hydrater &_hydrater,
            combats::combat_resolution &_resolver)
            : m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater), resolver(_resolver)
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

    void update(ecs::state &r_state) override
    {
        r_state.each_id<transforms::transform, components::board>(
                [&](entity_id board_id, auto &board_t, auto &board) {
                    if (board.did_tick_elapse)
                        handle_tick_end(r_state, board_id, board_t, board);
                    else
                        handle_tick_progress(r_state, board_id, board_t, board);
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
        do_game_piece_actions(r_state);
        resolver.Resolve_Combats();
        generate_new_board_state(r_state);

        // If a unit is standing in fire, it takes damage;
        r_state.each<components::game_piece, transforms::transform>([&](auto &game_piece, auto &transform) {
            r_state.each<components::terrain>([&](auto &terrain) {
                if (terrain.location == game_piece.board_source)
                {
                    if (terrain.type == components::TERRAIN_ENUM::FIRE)
                    {
                        if (terrain.team != game_piece.team)
                        {
                            game_piece.health -= terrain.damage;
                            if (game_piece.health <= 0.f) // no. please refactor
                            {
                                game_piece.state = components::UNIT_STATE::DYING;
                            }
                        }
                    }
                }
            });
        });
    }

    void handle_tick_progress(ecs::state &r_state, entity_id board_id, transforms::transform &board_t,
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
                            unit_death_event new_event(unit_id);
                            event_manager.BroadcastEvent(new_event);
                            break;
                        }
                    }
                });
    }

    void do_game_piece_actions(ecs::state &r_state)
    {
        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece &game_piece) {
            game_piece.board_source = game_piece.board_destination;

            game_piece.state = check_attacks(game_piece.board_source, game_piece.attacks, game_piece.team, r_state)
                               ? components::UNIT_STATE::ATTACK
                               : components::UNIT_STATE::MOVE;

        });

        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece &game_piece) {

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

        r_state.each_id<components::game_piece>([&](entity_id id, components::game_piece &game_piece) {
            if (game_piece.health <= 0)
            {
                game_piece.state = components::UNIT_STATE::DYING;
            }

        });
    }

    static void score_for_team(ecs::state &state, float team, float damage)
    {
        state.each<components::player>([&](auto &p) {
            if (p.team != team)
            {
                p.health -= damage;
                if (p.health < 0.f)
                {
                    p.health = 0.f;
                }
            }
        });

        state.each<components::tug_of_war_meter>([&](auto &meter) {
            meter.value += -1.f * team * damage;
            std::clamp(meter.value, -100.f, 100.f);
        });
    }

    // Helper function for moving in board space
    static void move_unit(components::game_piece &game_piece, glm::ivec2 location)
    {
        game_piece.board_source = location;
    }

    //Helper function for getting a board_square
    static ecs::entity *get_square_at_location(ecs::state &r_state, glm::ivec2 loc)
    {
        return r_state.first<components::board_square>([&](components::board_square &board_square) {
            return board_square.x == loc.x && board_square.y == loc.y;
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
        unit_t.position = board.grid_to_board(game_piece.continuous_board_location, board_t);
        unit_t.scale = glm::vec3(0.2f);
        unit_t.is_matrix_dirty = true;
    }

    void generate_new_board_state(ecs::state &r_state)
    {
        r_state.each<components::board>([&](components::board &board) {
            board.clear_state();

            r_state.each<components::game_piece>([&](components::game_piece &piece) {
                auto out_of_bounds =
                        (piece.board_source.y <= 1 && piece.team == -1) ||
                        (piece.board_source.y >= 7 && piece.team == 1);

                if (out_of_bounds)
                {
                    piece.state = components::UNIT_STATE::DYING;
                    score_for_team(r_state, piece.team, piece.damage * 10.f);
                }
            });

            r_state.each<components::game_piece>([&](components::game_piece &piece) {
                board.board_state[piece.board_source.x][piece.board_source.y] = piece.team;
                piece.remaining_speed = piece.state == components::UNIT_STATE::MOVE ? piece.speed : 0;
                piece.board_destination = piece.board_source;
            });

            reconcile_movement_intervals(r_state, board);
            claim_territory(r_state, board);
            board.print();
        });
    }

    static void reconcile_movement_intervals(ecs::state &r_state, components::board &board)
    {
        bool changed = true;
        while (changed)
        {
            changed = false;
            r_state.each<components::game_piece>([&](components::game_piece &piece) {
                if (piece.remaining_speed <= 0) return;

                glm::ivec2 next_pos = piece.board_destination + piece.move_board;
                if (next_pos.y < 0)
                {
                    piece.remaining_speed = 0;
                    next_pos.y = 0;
                    piece.board_destination = next_pos;
                } else if (next_pos.y > 8)
                {
                    piece.remaining_speed = 0;
                    next_pos.y = 8;
                    piece.board_destination = next_pos;
                } else if (board.board_state[next_pos.x][next_pos.y] == 0)
                {
                    board.board_state[piece.board_destination.x][piece.board_destination.y] = 0;
                    board.board_state[next_pos.x][next_pos.y] = piece.team;
                    piece.board_destination = next_pos;
                    int speed_reduction = 1;
                    r_state.each<components::terrain>([&](auto &terrain) {
                        if (terrain.location == piece.board_source)
                        { //TODO: i dont think board_source is the right thing to check against here
                            if (terrain.type == components::TERRAIN_ENUM::WEB)
                            {
                                if (terrain.team != piece.team)
                                {
                                    ++speed_reduction;
                                }
                            }
                        }
                    });

                    piece.remaining_speed -= speed_reduction;
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
    // Helper function for checking for legal attacks
    static bool
    check_attacks(glm::ivec2 location, std::vector<glm::ivec2> targets, float teammates, ecs::state &r_state)
    {
        for (auto &t : targets) t += location;

        auto attacked = r_state.first<components::game_piece>([&](components::game_piece &game_piece) {
            for (auto &target : targets)
            {
                if (game_piece.board_source == target && game_piece.team != teammates)
                    return true;
            }

            return false;
        });

        return attacked != nullptr;
    }

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
