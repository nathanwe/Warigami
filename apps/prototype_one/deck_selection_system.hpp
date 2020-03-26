#ifndef __DECK_SELECTION_SYSTEM_HPP_
#define __DECK_SELECTION_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <asset/scene_hydrater.hpp>
#include <core/game_input_manager.hpp>
#include <components/deck_selection.hpp>

#include "components/board.hpp"

class deck_selection_system : public ecs::system_base
{
public:
	deck_selection_system(
		asset::scene_hydrater& hydrater,
		core::game_input_manager& input);

	void initialize(ecs::state& state);
	void update(ecs::state& state);

private:
	asset::scene_hydrater& _hydrater;
	core::game_input_manager& _input;

	ecs::entity* _board;
	ecs::entity* _player1;
	ecs::entity* _player2;
	ecs::entity* _deck_selection;

	void do_update(ecs::state& state, components::board& board);
};



#endif
