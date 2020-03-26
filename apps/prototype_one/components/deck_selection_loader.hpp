#ifndef __DECK_SELECTION_LOADER_HPP_
#define __DECK_SELECTION_LOADER_HPP_

#include "deck_option.hpp"
#include "deck_selection.hpp"
#include <asset/component_loader.hpp>

namespace components
{
	class deck_selection_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(deck_selection::component_bitshift);
			auto& deck_selection = entity->get_component<components::deck_selection>();

			size_t deck_1_count = 0;
			size_t deck_2_count = 0;

			for (auto& child : asset_loader_node.children)
			{
				auto& child_entity = child.entity_resource.entity;
				auto option_component = child_entity->get_component_opt<components::deck_option>();

				if (!option_component) continue;

				if (option_component->team == 1)
					deck_selection.p1_deck_options[deck_1_count++] = child_entity->id();
				else
					deck_selection.p2_deck_options[deck_2_count++] = child_entity->id();
			}
		}

		component_bitset components_to_load() override
		{
			return deck_selection::archetype_bit;
		}
	};
}

#endif