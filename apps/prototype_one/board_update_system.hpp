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
#include "combat.hpp"
#include "components/card_enum.hpp"
#include "components/terrain.hpp"

#include <algorithm>

struct to_spawn 
{
    to_spawn(int _x, int _y, int _team, components::card_enum _type) : x(_x), y(_y), team(_team), type(_type) {}
    int x;
    int y;
    int team;
    components::card_enum type;
};

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
            combats::combat_resolution& _resolver)
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

    void spawn_unit_in_place(int lane, int space, int team, ecs::state& r_state, components::card_enum type)
    {
        static const std::string CardPrototypes[(size_t)components::card_enum::TOTAL_CARDS] = {
            "assets/prototypes/scissorling.json",
            "assets/prototypes/scissorling.json",
            "assets/prototypes/scissorling_twin.json",
            "assets/prototypes/scissor_trooper.json",
            "assets/prototypes/scissorling_egg.json",
            "assets/prototypes/scissor_webber.json",
            "assets/prototypes/scissor_goliath.json",
            "assets/prototypes/scissor_queen.json",
            "assets/prototypes/scissor_titan.json"
        };

        size_t type_index = (size_t)type;
        ecs::entity& nerd = hydrater.add_from_prototype(CardPrototypes[type_index]);
        transforms::transform& nerdT = nerd.get_component<transforms::transform>();
        components::game_piece& nerdP = nerd.get_component<components::game_piece>();

        nerdP.team = team;
        nerdP.board_source.x = lane;
        nerdP.board_source.y = space;

        if (nerdP.team >= 0)
            nerdT.rotation.y = glm::pi<float>() - glm::quarter_pi<float>();
        else
            nerdT.rotation.y = glm::quarter_pi<float>();

        nerdP.continuous_board_location = nerdP.board_source;
        nerdP.board_destination = nerdP.board_source;
        nerdP.move_board = nerdP.move_board * nerdP.team;
        nerdP.move_world = nerdP.move_world * (float)nerdP.team;

        std::vector<glm::ivec2> new_attacks;
        for (int i = 0; i < nerdP.attacks.size(); i++)
        {
            new_attacks.push_back(nerdP.attacks[i] * nerdP.team);
        }
        nerdP.attacks = new_attacks;

        nerdT.scale = glm::vec3(0.5);

        r_state.each_id<components::board, transforms::transform>([&](entity_id id, components::board& board, transforms::transform& transform)
            {
                nerdT.has_parent = true;
                nerdT.parent = id;
            });
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
                {
                    piece.state = components::UNIT_STATE::DYING;
                    score_for_team(r_state, piece.team, piece.damage * 10.f);
                }
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

    static void reconcile_movement_intervals(ecs::state& r_state, components::board& board)
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
                    int speed_reduction = 1;
                    r_state.each<components::terrain>([&](auto& terrain) {
                        if (terrain.location == piece.board_source) { //TODO: i dont think board_source is the right thing to check against here
                            if (terrain.type == components::TERRAIN_ENUM::WEB) {
                                if (terrain.team != piece.team) {
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
    void attack_targets(
            components::game_piece& attacker,
            entity_id attacker_id,
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
                    bool duplicate = false;
                    for (auto& comb : resolver.Get_Combats())
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

    // Helper function for moving in board space
    static void move_unit(components::game_piece &game_piece, glm::ivec2 location)
    {
        game_piece.board_source = location;
    }
    //Helper function for getting a board_square
    static ecs::entity* get_square_at_location(ecs::state& r_state, glm::ivec2 loc) {
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
    // Helper function for attacking
    static void attack_unit(components::game_piece& game_piece, float ticker_t)
    {
        static std::pair<float, float> position_keyframes[] { // t -> y
            {std::numeric_limits<float>::min(), 0.f},
            {0.f, 0.f},            
            {0.4f, 0.0f},
            {0.6f, -0.1f},
            {0.7f, 0.2f},
            {1.f, 0.f},
            {std::numeric_limits<float>::max(), 0.f},
        };

        size_t index = 0;
        while (position_keyframes[index].first < ticker_t)
            index++;

        float source = position_keyframes[index - 1].second;
        float destination = position_keyframes[index].second;
        float t_first = position_keyframes[index-1].first;
        float t_second = position_keyframes[index].first;
        float t_range = t_second - t_first;
        float t = (ticker_t - t_first) / t_range;
        game_piece.continuous_board_location.y = game_piece.board_source.y + 
            (source + t * (destination - source)) * game_piece.team;
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

    void spiderling_egg_spawns(std::vector<to_spawn>& spawner, ecs::state& r_state, components::game_piece& piece)
    {
        for (auto& effect : piece.effects)
        {
            if (effect == combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_FOR_HEALTH)
            {
                bool open_space[3] = {true, true, true};
                r_state.each<components::game_piece>([&](components::game_piece& game_piece) 
                {
                    if (game_piece.board_source == piece.board_source + glm::ivec2(0, 1 * piece.team))
                    {
                        open_space[0] = false;
                    }

                    if (game_piece.board_source == piece.board_source + glm::ivec2(1, 0) || piece.board_source.x >= 6)
                    {
                        open_space[1] = false;
                    }

                    if (game_piece.board_source == piece.board_source + glm::ivec2(-1, 0) || piece.board_source.x <= 0)
                    {
                        open_space[2] = false;
                    }
                });

                if (open_space[0])
                {
                    to_spawn new_spawn(piece.board_source.x, piece.board_source.y + (1 * piece.team), piece.team, components::card_enum::SCISSORLING);
                    spawner.push_back(new_spawn);
                    piece.health -= 1;
                }
                else if (open_space[1])
                {
                    to_spawn new_spawn(piece.board_source.x + 1, piece.board_source.y, piece.team, components::card_enum::SCISSORLING);
                    spawner.push_back(new_spawn);
                    piece.health -= 1;
                }
                else if (open_space[2])
                {
                    to_spawn new_spawn(piece.board_source.x - 1, piece.board_source.y, piece.team, components::card_enum::SCISSORLING);
                    spawner.push_back(new_spawn);
                    piece.health -= 1;
                }
            }
        }
    }

    void update(ecs::state &r_state) override
    {
        r_state.each_id<transforms::transform, components::board>(
            [&](entity_id board_id, auto& board_t, auto& board) {
                handle_board(r_state, board_id, board_t, board);
            });
    }

private:
    float timer = ROUND_TIME;
    core::game_input_manager &m_input;
    core::frame_timer &m_timer;
    event::EventManager &event_manager;
    asset::scene_hydrater &hydrater;
    combats::combat_resolution& resolver;

    void handle_board(ecs::state r_state, entity_id board_id, transforms::transform& board_t, components::board& board)
    {
        float delta = m_timer.smoothed_delta_secs();

        // Do board combat every 1 second
        timer -= delta;

        float timer_t = (ROUND_TIME - timer) / ROUND_TIME;

        if (timer <= 0.f)
        {
            timer = ROUND_TIME;

            std::vector<to_spawn> spawner;

            // A unit can only do one thing per turn with the exception that a unit can both attack and die on the same turn
            // Update states to either attack or move depending on current board state
            r_state.each<components::game_piece>([&](components::game_piece& game_piece)
                {
                    game_piece.board_source = game_piece.board_destination;
                    spiderling_egg_spawns(spawner, r_state, game_piece);
                });

            r_state.each<components::game_piece>([&](components::game_piece& game_piece)
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


            r_state.each<components::game_piece>([&](components::game_piece& game_piece)
                {
                    if (game_piece.health <= 0)
                    {
                        game_piece.state = components::UNIT_STATE::DYING;
                        game_piece.give_points = 3;//back to 3 for testing //(int)game_piece.piece_type; // with the current order, big pieces give more points
                        for (auto& effect : game_piece.effects)
                        {
                            switch (effect)
                            {
                            case combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_ON_DEATH:
                            {
                                to_spawn newSpawn(game_piece.board_source.x, game_piece.board_source.y, game_piece.team, components::card_enum::SCISSORLING);
                                spawner.push_back(newSpawn);
                                break;
                            }
                            case combats::COMBAT_EFFECTS::SPAWN_SCISSOR_TROOPER_ON_DEATH:
                            {
                                to_spawn newSpawn2(game_piece.board_source.x, game_piece.board_source.y, game_piece.team, components::card_enum::SCISSOR_TROOPER);
                                spawner.push_back(newSpawn2);
                                break;
                            }
                            }
                        }
                    }
                });


            // If a unit can attack, attack now
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
                            game_piece.damage,
                            game_piece.team,
                            r_state);
                    }
                    resolver.Resolve_Combats();
                });

            generate_new_board_state(r_state);

            // If a unit is standing in FIRE, it takes damage; 
            r_state.each<components::game_piece, transforms::transform>([&](auto& game_piece, auto& transform) {
                r_state.each<components::terrain>([&](auto& terrain) {
                        if (terrain.location == game_piece.board_source) {
                            if (terrain.type == components::TERRAIN_ENUM::FIRE) {
                                if (terrain.team != game_piece.team) {
                                    game_piece.health -= terrain.damage;
                                    if (game_piece.health <= 0.f)
                                    {
                                        game_piece.state = components::UNIT_STATE::DYING;
                                    }
                                }
                            }
                        }
                    });
                });


            for (auto& spawns : spawner)
            {
                spawn_unit_in_place(spawns.x, spawns.y, spawns.team, r_state, spawns.type);
            }
            spawner.clear();

        }
        else
        {
            // If a unit made it through the last state update with a move state, move the unit until the next update

            r_state.each_id<transforms::transform, components::game_piece>(
                [&](entity_id unit_id, auto& unit_t, auto& unit) {
                    if (unit.state == components::UNIT_STATE::ATTACK)
                    {
                        // attacking animation here
                        attack_unit(unit, timer_t);
                        handle_unit_transform(board_t, unit_t, unit, board, board_id);
                    }
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


        }
    }

    static void score_for_team(ecs::state& state, float team, float damage)
    {
		state.each<components::player>([&](auto& p)
		{
			if (p.team != team)
			{
				p.health -= damage;
                if (p.health < 0.f)
                {
                    p.health = 0.f;
                }
			}
		});

        state.each<components::tug_of_war_meter>([&](auto& meter)
        {
            meter.value += -1.f * team * damage;
            std::clamp(meter.value, -100.f, 100.f);
        });
    }
};

#endif
