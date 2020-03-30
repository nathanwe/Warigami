#include "deck_selection_controller.hpp"
#include "game_util/player_specifics.hpp"
#include <transforms/transform.hpp>
#include <rendering/renderable_model_static.hpp>
#include <util/sign.hpp>
#include "components/countdown.hpp"


deck_selection_controller::deck_selection_controller(
	asset::scene_hydrater& hydrater, 
	core::game_input_manager& input,
	core::frame_timer& timer)
	: _hydrater(hydrater)
	, _input(input)
	, _timer(timer)
{
}

void deck_selection_controller::initialize(ecs::state& state)
{
	_board = state.first<components::board>();
	_deck_selection = state.first<components::deck_selection>();

	_players[0] = state.first<components::player>([&](components::player& p) {
		return p.team == 1.f;
	});

	_players[1] = state.first<components::player>([&](components::player& p) {
		return p.team == -1.f;
	});

	assert(_board != nullptr);
	assert(_deck_selection != nullptr);
	assert(_players[0] != nullptr);
	assert(_players[0] != nullptr);
}

void deck_selection_controller::update(ecs::state& state)
{
	state.each<components::board>([&](components::board& board) {
		if (board.state == components::game_state::deck_selection) 
			do_update(state, board);
		else
			hide_elements(state, board);
	});
}

void deck_selection_controller::do_update(ecs::state& state, components::board& board)
{
	auto& selection_component = _deck_selection->get_component<components::deck_selection>();
	auto& selection_transform = _deck_selection->get_component<transforms::transform>();
	selection_transform.position = glm::vec3(0);
	selection_transform.is_matrix_dirty = true;

	auto& player1_comp = _players[0]->get_component<components::player>();
	auto& player2_comp = _players[1]->get_component<components::player>();

	player_specific_data player1_specifics(player1_comp, _input);
	player_specific_data player2_specifics(player2_comp, _input);

	handle_player_selection(player1_comp, player1_specifics, selection_component);
	handle_player_selection(player2_comp, player2_specifics, selection_component);

	for (components::deck_index i = 0; i < 2; i++)
	{
		auto& p1_option = state.find_entity(selection_component.p1_deck_options[i]);
		auto& p2_option = state.find_entity(selection_component.p2_deck_options[i]);			
		position_deck_option(i, p1_option, *_players[0], selection_component);
		position_deck_option(i, p2_option, *_players[1], selection_component);
	}	

	check_players_ready(state);
}

void deck_selection_controller::hide_elements(ecs::state& state, components::board& board)
{
	auto& transform = _deck_selection->get_component<transforms::transform>();
	transform.position = MovePosition;
	transform.is_matrix_dirty = true;
}

void deck_selection_controller::position_deck_option(
	components::deck_index deck_index, 
	ecs::entity& option, 
	ecs::entity& player,
	components::deck_selection& deck_selection)
{
	auto& option_comp = option.get_component<components::deck_option>();	
	auto& option_transform = option.get_component<transforms::transform>();	
	auto& player_comp = player.get_component<components::player>();

	auto is_selected = player_comp.deck_selection == deck_index;

	auto start_x = option_comp.team == 1
		? deck_selection.player_1_options_x
		: deck_selection.player_2_options_x;

	auto x_increment = deck_selection.option_width + deck_selection.padding;

	option_transform.position.x = start_x + x_increment * deck_index;
	option_transform.position.y = is_selected ? deck_selection.card_y + 0.05f : deck_selection.card_y;
	option_transform.position.z = -deck_selection.card_depth;	
	option_transform.scale.y = is_selected ? 3.1f : 3.f;	
	option_transform.is_matrix_dirty = true;
}

void deck_selection_controller::handle_player_selection(
	components::player& player,
	player_specific_data& player_specifics,
	components::deck_selection& deck_selection)
{
	auto left = player_specifics.values.left;

	if (player.select_delay > 0.f)
	{
		player.select_delay -= _timer.smoothed_delta_secs();
	}
	else
	{
		player.select_delay = 0.1f;
		auto horizontal_input_active = std::abs(left) > .4f;
		auto dir_h = util::sign(left);
		auto under_limit_h = dir_h < 0 
			? player.deck_selection > 0
			: player.deck_selection < deck_selection.num_decks-1;

		if (horizontal_input_active && under_limit_h)
		{
			player.select_delay = 0.1f;
			player.deck_selection += dir_h;
		}
	}
}

void deck_selection_controller::check_players_ready(ecs::state& state)
{
	bool all_ready = true;
	state.each<components::player>([&](components::player& player) {
		player_specific_data player_specifics(player, _input);

		auto pressed_ready = _input.is_input_ended(player_specifics.controls.card1);
		auto pressed_cancel = _input.is_input_ended(player_specifics.controls.card2);

		if (pressed_ready)
			player.is_ready = true;

		if (pressed_cancel)
			player.is_ready = false;		

		all_ready = all_ready && player.is_ready;
	});

	if (all_ready) on_start(state);
}

void deck_selection_controller::on_start(ecs::state& state)
{
	auto& board_component = _board->get_component<components::board>();
	board_component.state = components::game_state::countdown;

	state.each<components::player>([&](components::player& player) {
		player.deck = components::decks[player.deck_selection];
	});
}
