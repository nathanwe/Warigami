#ifndef GAME_PIECE_LOADER_HPP
#define GAME_PIECE_LOADER_HPP

#include <asset/component_loader.hpp>
//#include <components/include/game_piece.hpp>


namespace components
{
	class game_piece_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif