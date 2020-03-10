#ifndef GAME_COMPONENTS_BOARD_LOADER_HPP
#define GAME_COMPONENTS_BOARD_LOADER_HPP

#include <asset/component_loader.hpp>
#include "board.hpp"
#include "board_square.hpp"


namespace components
{
	class board_loader : public asset::component_loader
	{
		void find_ids(asset::asset_loader_node& asset_loader_node, std::vector<ecs::entity*> entities);
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif