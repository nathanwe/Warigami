#pragma once

#include "button_tooltip.hpp"

#include <asset/component_loader.hpp>

namespace components
{
	class button_tooltip_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override
		{
			auto entity = asset_loader_node.entity_resource.entity;
			auto entity_data = asset_loader_node.entity_resource.entity_data;
			auto& json = entity_data->component_data(button_tooltip::component_bitshift);
			auto& tt = entity->get_component<button_tooltip>();
			tt.team = json.value("team", 1.f);
		}

		component_bitset components_to_load() override
		{
			return button_tooltip::archetype_bit;
		}
	};
}
