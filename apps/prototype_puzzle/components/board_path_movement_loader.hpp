#ifndef GAME_COMPONENTS_BOARD_PATH_MOVEMENT_LOADER_HPP
#define GAME_COMPONENTS_BOARD_PATH_MOVEMENT_LOADER_HPP

#include <asset/component_loader.hpp>
#include "board_path_movement.hpp"


namespace components
{
	class board_path_movement_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif