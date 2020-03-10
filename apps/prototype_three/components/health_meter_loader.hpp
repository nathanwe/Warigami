#ifndef WIZARD_GAME_HEALTH_METER_LOADER_HPP
#define WIZARD_GAME_HEALTH_METER_LOADER_HPP

#include "health_meter.hpp"

#include <asset/component_loader.hpp>

namespace components
{
	class health_meter_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(health_meter::component_bitshift);
			auto& meter = entity->get_component<health_meter>();

			if (auto it = json.find("team"); it != json.end())
			{
				meter.team = it->get<float>();
			}
		}

		component_bitset components_to_load() override
		{
			return health_meter::archetype_bit;
		}
	};
}

#endif