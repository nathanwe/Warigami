#include "deck_selection_system.hpp"
#include "game_util/player_specifics.hpp"

deck_selection_system::deck_selection_system(
	asset::scene_hydrater& hydrater, 
	core::game_input_manager& input)
	: _hydrater(hydrater)
	, _input(input)
{
}

void deck_selection_system::initialize(ecs::state& state)
{
	_board = state.first<components::board>();
	_deck_selection = state.first<components::deck_selection>();

	_player1 = state.first<components::player>([&](components::player& p) {
		return p.team == 1.f;
	});

	_player2 = state.first<components::player>([&](components::player& p) {
		return p.team == -1.f;
	});

	assert(_board != nullptr);
	assert(_deck_selection != nullptr);
	assert(_player1 != nullptr);
	assert(_player2 != nullptr);
}

void deck_selection_system::update(ecs::state& state)
{
	state.each<components::board>([&](components::board& board) {
		if (board.state != components::game_state::deck_selection) return;

		do_update(state, board);
	});
}

void deck_selection_system::do_update(ecs::state& state, components::board& board)
{
	state.each<components::player>([&](components::player& player) {
		player_specific_data player_specifics(player, _input);

		
	});
}
