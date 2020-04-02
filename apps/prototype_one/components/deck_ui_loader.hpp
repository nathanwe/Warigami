#ifndef __DECK_UI_LOADER_HPP_
#define __DECK_UI_LOADER_HPP_


#include "deck_option.hpp"
#include "deck_ui.hpp"
#include <asset/component_loader.hpp>

namespace components
{
	class deck_ui_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(deck_ui::component_bitshift);
			auto& deck_ui = entity->get_component<components::deck_ui>();
						
			for (auto& child : asset_loader_node.children)
			{
				deck_ui.children[deck_ui.child_count++] = child.entity_resource.entity->id();
			}
		}

		component_bitset components_to_load() override
		{
			return deck_ui::archetype_bit;
		}
	};
}


#endif