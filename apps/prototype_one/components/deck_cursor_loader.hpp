#ifndef __DECK_CURSOR_LOADER_HPP_
#define __DECK_CURSOR_LOADER_HPP_

#include "deck_cursor.hpp"
#include <asset/component_loader.hpp>

namespace components
{
	class deck_cursor_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(deck_cursor::component_bitshift);
			auto& cursor = entity->get_component<deck_cursor>();

			cursor.team = json.value("team", 1.f);
		}

		component_bitset components_to_load() override
		{
			return deck_cursor::archetype_bit;
		}
	};
}

#endif