#ifndef GAME_COMPONENTS_CARD_LOADER_HPP
#define GAME_COMPONENTS_CARD_LOADER_HPP

#include <asset/component_loader.hpp>
#include "card.hpp"


namespace components
{
	class card_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif