#ifndef GAME_COMPONENTS_BOARD_SQUARE_LOADER_HPP
#define GAME_COMPONENTS_BOARD_SQUARE_LOADER_HPP

#include <asset/component_loader.hpp>
#include "board_square.hpp"


namespace components
{
	class board_square_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif