#ifndef __DECK_OPTION_LOADER_HPP_
#define __DECK_OPTION_LOADER_HPP_


#include "deck_option.hpp"
#include "deck_selection.hpp"
#include <asset/component_loader.hpp>

namespace components
{
	class deck_option_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(deck_option::component_bitshift);
			auto& option = entity->get_component<components::deck_option>();

			option.deck = json.value("deck", deck_type::spider);
			option.team = json.value("team", 1.f);			
		}

		component_bitset components_to_load() override
		{
			return deck_option::archetype_bit;
		}
	};
}


#endif