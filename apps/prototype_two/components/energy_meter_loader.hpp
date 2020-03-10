#ifndef WIZARD_GAME_ENERGY_METER_LOADER_HPP
#define WIZARD_GAME_ENERGY_METER_LOADER_HPP

#include "energy_meter.hpp"

#include <asset/component_loader.hpp>

namespace components
{
	class energy_meter_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(energy_meter::component_bitshift);
			auto& meter = entity->get_component<energy_meter>();

			if (auto it = json.find("team"); it != json.end())
			{
				meter.team = it->get<float>();
			}
		}

		component_bitset components_to_load() override
		{
			return energy_meter::archetype_bit;
		}
	};
}

#endif