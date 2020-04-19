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
                                        r_state,
                                        board,
                                        board_t);
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
            ecs::state &r_state,
        components::board& board,
        transforms::transform& board_t)
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
                        if (attacker.projectile_type != components::PROJECTILE::NONE) {
                            spawn_projectile(attacker, attacker.board_source, game_piece.board_source, board, board_t);
                        }
                    }

                }
            }
        });
    }

    void spawn_projectile(components::game_piece& attacker, glm::vec2 position, glm::vec2 destination, components::board& board, transforms::transform& board_t) {
        std::string projectile_type;
        if (attacker.projectile_type == components::PROJECTILE::PENCIL) {
            projectile_type = "assets/prototypes/projectile_pencil.json";
        }
        else if (attacker.projectile_type == components::PROJECTILE::FIRE) {
            projectile_type = "assets/prototypes/projectile_fire.json";
        }
        else if (attacker.projectile_type == components::PROJECTILE::WEB) {
            projectile_type = "assets/prototypes/projectile_web.json";
        }
        ecs::entity& projectile = hydrater.add_from_prototype(projectile_type);
        transforms::transform& projectileT = projectile.get_component<transforms::transform>();
        projectileT.has_parent = true;
        projectileT.parent = 69; // Game board
        projectileT.position = board.grid_to_board(position, board_t);
        projectileT.position.y += 0.5;
        projectileT.rotation = glm::vec3(0, 90, -48);
        projectileT.is_matrix_dirty = true;
        attacker.projectiles.push_back(std::make_pair(projectile, board.grid_to_board(destination, board_t) + glm::vec3(0,0.5,0)));
    }
};

#endif
