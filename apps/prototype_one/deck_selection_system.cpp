#include "deck_selection_system.hpp"
#include "game_util/player_specifics.hpp"
#include <transforms/transform.hpp>

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
		if (board.state == components::game_state::deck_selection) 
			do_update(state, board);
		else
			hide_elements(state, board);
	});
}

void deck_selection_system::do_update(ecs::state& state, components::board& board)
{
	auto& selection_component = _deck_selection->get_component<components::deck_selection>();
	auto& selection_transform = _deck_selection->get_component<transforms::transform>();
	selection_transform.position = glm::vec3(0);
	selection_transform.is_matrix_dirty = true;

	state.each<components::player>([&](components::player& player) {
		player_specific_data player_specifics(player, _input);
		
		for (size_t i = 0; i < 2; i++)
		{
			auto& p1_option = state.find_entity(selection_component.p1_deck_options[i]);
			auto& p2_option = state.find_entity(selection_component.p2_deck_options[i]);
			auto& p1_option_comp = p1_option.get_component<components::deck_option>();
			auto& p2_option_comp = p2_option.get_component<components::deck_option>();
			auto& p1_option_transform = p1_option.get_component<transforms::transform>();
			auto& p2_option_transform = p2_option.get_component<transforms::transform>();
			position_deck_option(i, p1_option_comp, p1_option_transform, selection_component);
			position_deck_option(i, p2_option_comp, p2_option_transform, selection_component);
		}
	});
}

void deck_selection_system::hide_elements(ecs::state& state, components::board& board)
{
	auto& transform = _deck_selection->get_component<transforms::transform>();
	transform.position = MovePosition;
	transform.is_matrix_dirty = true;
}

void deck_selection_system::position_deck_option(
	size_t deck_index, 
	components::deck_option& deck_option, 
	transforms::transform& deck_option_t, 
	components::deck_selection& deck_selection)
{
	float start_x = deck_option.team == 1
		? deck_selection.min_card_x
		: deck_selection.deck_gap;

	float x_increment = (deck_selection.max_card_x - deck_selection.deck_gap) /
		(components::player::MaxCards - 1.f);

	deck_option_t.position.x = start_x + x_increment * deck_index;
	deck_option_t.position.y = deck_selection.card_y;
	deck_option_t.position.z = -deck_selection.card_depth;
	deck_option_t.is_matrix_dirty = true;	
}