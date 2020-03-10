#ifndef WIZARD_GAME_TUG_OF_WAR_METER_LOADER_HPP
#define WIZARD_GAME_TUG_OF_WAR_METER_LOADER_HPP

#include "tug_of_war_meter.hpp"

#include <asset/component_loader.hpp>

namespace components
{
	class tug_of_war_meter_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(tug_of_war_meter::component_bitshift);
			auto& meter = entity->get_component<tug_of_war_meter>();
			if (auto it = json.find("value"); it != json.end())
			{
				meter.value = it->get<float>();
			}
		}

		component_bitset components_to_load() override
		{
			return tug_of_war_meter::archetype_bit;
		}
	};
}

#endif