#pragma once

#include <asset/scene_hydrater.hpp>
#include <ecs/entity.hpp>
#include "../components/card_enum.hpp"

class card_spawner
{
public:
	card_spawner(asset::scene_hydrater& hydrater) : _hydrater(hydrater) {}

	ecs::entity& spawn(components::card_enum type)
	{
        switch (type)
        {
        case components::card_enum::SCISSORLING_TWIN:
            return _hydrater.add_from_prototype("assets/prototypes/card_tandem.json");
        case components::card_enum::SCISSOR_TROOPER:
            return _hydrater.add_from_prototype("assets/prototypes/card_trooper.json");
        case components::card_enum::SCISSORLING_EGG:
            return _hydrater.add_from_prototype("assets/prototypes/card_egg.json");
        case components::card_enum::SCISSORLING:
            return _hydrater.add_from_prototype("assets/prototypes/card_scissorling.json");
        case components::card_enum::SCISSOR_WEBBER:
            return _hydrater.add_from_prototype("assets/prototypes/card_webber.json");
        case components::card_enum::SCISSOR_GOLIATH:
            return _hydrater.add_from_prototype("assets/prototypes/card_goliath.json");
        case components::card_enum::SCISSOR_TITAN:
            return _hydrater.add_from_prototype("assets/prototypes/card_dragon.json");
        case components::card_enum::SCISSOR_QUEEN:
            return _hydrater.add_from_prototype("assets/prototypes/card_queen.json");

        case components::card_enum::LIGHT_TANK_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_light_tank_soldier.json");
        case components::card_enum::LIGHT_RANGE_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_light_range_soldier.json");
        case components::card_enum::LIGHT_FAST_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_light_fast_soldier.json");
        case components::card_enum::MEDIUM_TANK_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_medium_tank_soldier.json");
        case components::card_enum::MEDIUM_RANGE_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_medium_range_soldier.json");
        case components::card_enum::MEDIUM_FAST_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_medium_fast_soldier.json");
        case components::card_enum::HEAVY_TANK_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_heavy_tank_soldier.json");
        case components::card_enum::HEAVY_RANGE_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_heavy_range_soldier.json");
        case components::card_enum::HEAVY_FAST_SOLDIER:
            return _hydrater.add_from_prototype("assets/prototypes/card_heavy_fast_soldier.json");

        case components::card_enum::ERASER:
            return _hydrater.add_from_prototype("assets/prototypes/card_eraser.json");
        case components::card_enum::TROLL:
            return _hydrater.add_from_prototype("assets/prototypes/card_troll.json");
        case components::card_enum::WIZARD:
            return _hydrater.add_from_prototype("assets/prototypes/card_wizard.json");
        case components::card_enum::ARCHER:
            return _hydrater.add_from_prototype("assets/prototypes/card_archer.json");
        case components::card_enum::UNICORN:
            return _hydrater.add_from_prototype("assets/prototypes/card_unicorn.json");
        case components::card_enum::KNIGHT:
            return _hydrater.add_from_prototype("assets/prototypes/card_knight.json");

        default:
            return _hydrater.add_from_prototype("assets/prototypes/card_dragon.json");
        }
	}

    void remove(entity_id id)
    {
        _hydrater.remove_entity(id);
    }

private:
	asset::scene_hydrater& _hydrater;
};