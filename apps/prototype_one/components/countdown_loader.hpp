#ifndef WIZARD_GAME_COUNTDOWN_LOADER_HPP
#define WIZARD_GAME_COUNTDOWN_LOADER_HPP

#include "countdown.hpp"

#include <asset/component_loader.hpp>

namespace components
{
	class countdown_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(countdown::component_bitshift);
			auto& count = entity->get_component<countdown>();

			if (auto it = json.find("duration"); it != json.end())
			{
				count.count_duration = it->get<float>() *  1000000000; // To nanoseconds
			}
		}

		component_bitset components_to_load() override
		{
			return countdown::archetype_bit;
		}
	};
}

#endif