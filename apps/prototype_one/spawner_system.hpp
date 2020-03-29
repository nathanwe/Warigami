#ifndef __SPAWNER_SYSTEM_HPP
#define __SPAWNER_SYSTEM_HPP


#include <asset/scene_hydrater.hpp>
#include <asset/scene_hydrater.hpp>
#include <ecs/state.hpp>
#include <ecs/system_base.hpp>
#include <transforms/transform.hpp>

#include "components/game_piece.hpp"
#include "components/board.hpp"
#include "components/card_enum.hpp"



class spawner_system : public ecs::system_base
{
public:
    spawner_system(asset::scene_hydrater& hydrater) : _hydrater(hydrater) {}

	void update(ecs::state& state)
	{
		state.each_id<transforms::transform, components::board>(
			[&](entity_id board_id, transforms::transform& board_t, components::board& board) {
                for (auto& spawns : board.spawner)
                {
                    spawn_unit_in_place(spawns.x, spawns.y, spawns.team, state, spawns.type);
                }
                board.spawner.clear();                
			});
	}

private:
    asset::scene_hydrater& _hydrater;

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
            "assets/prototypes/scissor_titan.json",
            "assets/prototypes/light_tank_soldier.json",
            "assets/prototypes/light_range_soldier.json",
            "assets/prototypes/light_fast_soldier.json",
            "assets/prototypes/medium_tank_soldier.json",
            "assets/prototypes/medium_range_soldier.json",
            "assets/prototypes/medium_fast_soldier.json",
            "assets/prototypes/heavy_tank_soldier.json",
            "assets/prototypes/heavy_range_soldier.json",
            "assets/prototypes/heavy_fast_soldier.json"
        };

        size_t type_index = (size_t)type;
        ecs::entity& nerd = _hydrater.add_from_prototype(CardPrototypes[type_index]);
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

        nerdT.scale = glm::vec3(0.45f); //I would like changing the scale in the json to do things.

        r_state.each_id<components::board, transforms::transform>(
            [&](entity_id id, components::board& board, transforms::transform& transform)
            {
                nerdT.has_parent = true;
                nerdT.parent = id;
            });
    }

};


#endif