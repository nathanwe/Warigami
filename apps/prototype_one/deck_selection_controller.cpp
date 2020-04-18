#include <algorithm>

#include "deck_selection_controller.hpp"
#include "game_util/player_specifics.hpp"
#include <audio/audio_emitter.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <rendering/renderable_model_static.hpp>
#include <rendering/renderable_text.hpp>
#include <rendering/texture.hpp>
#include <util/sign.hpp>
#include <audio/audio_emitter.hpp>
#include "components/countdown.hpp"
#include "components/deck_cursor.hpp"
#include "components/game_piece.hpp"
#include "components/player.hpp"
#include "components/pause.hpp"
#include "components/card.hpp"
#include <rendering/renderable_text.hpp>
#include "components/terrain.hpp"

#include <string>

deck_selection_controller::deck_selection_controller(
	asset::scene_hydrater& hydrater,
	card_spawner& spawner,
	core::game_input_manager& input,
	core::frame_timer& timer,
	rendering::asset_cache& render_assets)
	: _hydrater(hydrater)
	, _card_spawner(spawner)
	, _input(input)
	, _timer(timer)
	, _render_assets(render_assets)
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

	_deck_cursors[0] = state.first<components::deck_cursor>([&](components::deck_cursor& p) {
		return p.team == 1.f;
	});

	_deck_cursors[1] = state.first<components::deck_cursor>([&](components::deck_cursor& p) {
		return p.team == -1.f;
	});
	
	if (can_run())
	{
		build_deck_set(0, components::decks[0]);
		build_deck_set(1, components::decks[1]);

		spawn_preview_units(state);

		auto& deck = _decks[0];
		auto& deck_selection_c = _deck_selection->get_component<components::deck_selection>();
		deck_selection_c.card_iterators[0] = deck.begin();
		deck_selection_c.card_iterators[1] = deck.begin();
		deck_selection_c.current_position_keyframes[0] = deck_selection_c.position_keyframes;
		deck_selection_c.current_position_keyframes[1] = deck_selection_c.position_keyframes;
	}
}

void deck_selection_controller::update(ecs::state& state)
{
	state.each<components::board>([&](components::board& board) {
		if (!can_run())					
			return;

		if (board.state == components::game_state::deck_selection) 
			do_update(state, board);
		else
			hide_elements(state, board);
	});

	if (can_run())
	{
		auto& selection_component = _deck_selection->get_component<components::deck_selection>();
		for (size_t i = 0; i < selection_component.child_count; ++i)
		{
			auto& e = state.find_entity(selection_component.children[i]);
			auto& t = e.get_component<transforms::transform>();
			t.is_matrix_dirty = true;
		}

		handle_animation(state, selection_component);
		position_cursors(state, selection_component);
	}
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

	handle_player_selection(state, player1_comp, player1_specifics, selection_component);
	handle_player_selection(state, player2_comp, player2_specifics, selection_component);

	for (components::deck_index i = 0; i < 2; i++)
	{
		auto& p1_option = state.find_entity(selection_component.p1_deck_options[i]);
		auto& p2_option = state.find_entity(selection_component.p2_deck_options[i]);			
		position_deck_option(i, p1_option, *_players[0], selection_component);
		position_deck_option(i, p2_option, *_players[1], selection_component);
	}	

	check_players_ready(state);
		
	auto& board_t = _board->get_component<transforms::transform>();
	auto board_id = _board->id();
		
	state.each_id<components::game_piece, transforms::transform>([&](
		entity_id id, components::game_piece& unit, transforms::transform& unit_t) {
			handle_unit_transform(board_t, unit_t, unit, board, board_id);
	});
}

void deck_selection_controller::hide_elements(ecs::state& state, components::board& board)
{
	auto& transform = _deck_selection->get_component<transforms::transform>();
	transform.position = MovePosition;
	transform.is_matrix_dirty = true;
	auto& selection = _deck_selection->get_component<components::deck_selection>();

	for (size_t i = 0; i < selection.child_count; ++i)
	{
		auto& child = state.find_entity(selection.children[i]);
		auto& t = child.get_component<transforms::transform>();
		t.position = MovePosition;
		t.is_matrix_dirty = true;

		auto text_opt = child.get_component_opt<rendering::renderable_text>();
		if (text_opt)
		{
			text_opt->position.x = 9999;
		}
	}
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
	option_transform.position.y = is_selected 
		? deck_selection.card_y + 0.05f 
		: deck_selection.card_y;

	option_transform.position.z = -deck_selection.card_depth;	
	option_transform.scale.y = is_selected ? 3.2f : 3.f;	
	option_transform.is_matrix_dirty = true;
}

void deck_selection_controller::position_card(
	components::deck_index deck_index,
	ecs::entity& card,
	ecs::entity& player,
	components::deck_selection& deck_selection, 
	glm::vec3 offset)
{
	auto& card_comp = card.get_component<components::card>();
	auto& card_transform = card.get_component<transforms::transform>();
	auto& player_comp = player.get_component<components::player>();

	auto is_selected = player_comp.deck_selection == deck_index;

	auto start_x = player_comp.team == 1.f
		? deck_selection.player_1_options_x
		: deck_selection.player_2_options_x;

	auto x_increment = deck_selection.option_width + deck_selection.padding;

	card_transform.position.x = start_x + (x_increment * deck_index);
	card_transform.position.y = deck_selection.card_y;
	card_transform.position.z = -deck_selection.card_depth;
	card_transform.position += offset;
	card_transform.is_matrix_dirty = true;
		
	card_transform.has_parent = true;
	card_transform.parent = _deck_selection->id();
}

void deck_selection_controller::handle_player_selection(
	ecs::state& state,
	components::player& player,
	player_specific_data& player_specifics,
	components::deck_selection& deck_selection)
{
	if (player.is_ready)
		return;

	auto player_index = player.team == 1.f ? 0 : 1;
	auto left = player_specifics.values.left;
	auto vert = player_specifics.values.forward;

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

			auto& emitter = _deck_selection->get_component<audio::audio_emitter>();
			emitter.set_sound_state(player.deck_selection, audio::sound_state::playback_requested);

			auto& deck = _decks[player.deck_selection];
			deck_selection.card_iterators[player_index] = deck.begin();
			deck_selection.preview_state[player_index] = components::preview_card_state::changing_forward;
			spawn_preview_units(state);
			interrupt_animation(state, deck_selection, player_index);
		}

		auto vertical_input_active = std::abs(vert) > .4f;
		auto dir_v = util::sign(vert);
		auto can_cycle = deck_selection.preview_state[player_index] == components::preview_card_state::displaying;

		if (vertical_input_active && can_cycle)
		{
			// hacky to get sound from pause menu, but we know it's there
			auto e = state.first<components::pause>();
			auto& sound_emitter = e->get_component<audio::audio_emitter>();
			sound_emitter.set_sound_state(0, audio::sound_state::playback_requested);

			auto& deck = _decks[player.deck_selection];

			if (dir_v < 0 && deck_selection.card_iterators[player_index] == deck.begin())
				deck_selection.card_iterators[player_index] = --deck.end();
			else if (dir_v > 0 && deck_selection.card_iterators[player_index] == --deck.end())
				deck_selection.card_iterators[player_index] = deck.begin();
			else if (dir_v < 0)
				deck_selection.card_iterators[player_index]--;
			else
				deck_selection.card_iterators[player_index]++;

			deck_selection.preview_state[player_index] = dir_v > 0
				? components::preview_card_state::changing_forward
				: components::preview_card_state::changing_back;						

			state.each<transforms::transform, components::game_piece>(
				[&](transforms::transform& t, components::game_piece& gp) 
				{
					if (gp.team == player.team && gp.piece_type == *deck_selection.card_iterators[player_index])
					{
						t.scale = glm::vec3(0.75f);
					}
					else if (gp.team == player.team)
					{
						t.scale = glm::vec3(0.45f);
					}

					t.is_matrix_dirty = true;			
				});
		}
	}
}

void deck_selection_controller::check_players_ready(ecs::state& state)
{
	bool all_ready = true;
	state.each<components::player>([&](components::player& player) {
		player_specific_data player_specifics(player, _input);

		auto pressed_ready = _input.is_input_started(player_specifics.controls.card1);
		auto pressed_cancel = _input.is_input_started(player_specifics.controls.card2);

		if (pressed_ready)
			player.is_ready = true;

		if (pressed_cancel)
			player.is_ready = false;		

		all_ready = all_ready && player.is_ready;
	});

	if (all_ready) on_start(state);
}

void deck_selection_controller::select_player_sprite(ecs::state& state)
{
	using namespace std::string_literals;
	
	static const auto spider_queen = _render_assets.get<rendering::texture>("assets/textures/spider/queen.png"s).id;
	static const auto fantasy_king = _render_assets.get<rendering::texture>("assets/textures/fantasy/archmage.png"s).id;

	state.each<components::player>(
		[&](components::player& player)
		{
			state.each<components::selection_arrow, rendering::renderable_mesh_static>(
				[&](components::selection_arrow& selector, rendering::renderable_mesh_static& sprite)
				{
					if (player.team == selector.team)
					{
						if (player.deck_selection == components::player::spider_deck)
						{
							sprite.material.texture_diffuse = spider_queen;
						}
						else if (player.deck_selection == components::player::fantasy_deck)
						{
							sprite.material.texture_diffuse = fantasy_king;
						}
						else
						{
							std::cout << "Can't set player controller sprite, unexpected deck index." << std::endl;
							assert(false);
						}
						position_energy_orbs(player, selector);
					}
				});
		});
}

void deck_selection_controller::position_energy_orbs(components::player& player, components::selection_arrow& selector)
{
	static constexpr int num_orbs = 10;
	static constexpr std::array<glm::vec3, num_orbs> right_spider_orb_positions
	{{
		{0.f, 0.f, 0.f}, {0.f, 0.2f, 0.f}, {-0.2f, 0.2f, 0.f}, {0.2f, 0.f, 0.f}, {-0.2f, 0.f, 0.f},
		{0.f, -0.2f, 0.f}, {0.2f, -0.2f, 0.f}, {-0.2f, -0.2f, 0.f}, {0.2f, 0.2f, 0.f}, {-0.4f, 0.f, 0.f}
	}};
	static constexpr std::array<glm::vec3, num_orbs> right_fantasy_orb_positions
	{{
		{-0.2f, 0.02f, 0.f}, {-0.15f, -0.15f, 0.f}, {-0.05f, -0.32f, 0.f}, {0.12f, -0.4f, 0.f}, {0.33f, -0.39f, 0.f},
		{0.52f, -0.35f, 0.f}, {0.69f, -0.25f, 0.f}, {0.8f, -0.07f, 0.f}, {0.85f, 0.13f, 0.f}, {0.8f, 0.3f, 0.f}
	}};

	for (int i = 0; i < num_orbs; ++i)
	{
		auto& orb_transform = selector.energy_orbs[i].get_component<transforms::transform>();
		switch (player.deck_selection)
		{
		case components::player::spider_deck:
			orb_transform.position = right_spider_orb_positions[i];
			orb_transform.position.x *= -player.team;
			break;
		case components::player::fantasy_deck:
			orb_transform.position = right_fantasy_orb_positions[i];
			orb_transform.position.x *= -player.team;
			break;
		}
		orb_transform.is_matrix_dirty = true;
	}
}

void deck_selection_controller::on_start(ecs::state& state)
{
	auto& board_component = _board->get_component<components::board>();
	board_component.state = components::game_state::countdown;

	auto& selection = _deck_selection->get_component<components::deck_selection>();	

	state.each<components::player>([&](components::player& player) {
		
		auto& option_id = player.team == 1.f 
			? selection.p1_deck_options[player.deck_selection]
			: selection.p2_deck_options[player.deck_selection];

		auto& option = state.find_entity(option_id);
		auto& option_c = option.get_component<components::deck_option>();
		auto deck_index = option_c.deck;

		player.deck = components::decks[(size_t)deck_index];
		player.start_deck = components::decks[(size_t)deck_index];
		player.shuffle();
		player.redraw();
	});

	state.each_id<components::game_piece>([&](entity_id id, components::game_piece& unit) {
		for (auto& hp : unit.health_points)
		{
			_card_spawner.remove(hp.id());
		}
		_card_spawner.remove(id);
	});

	state.each<components::pause>([&](auto& pause) {
		pause.is_game_started = true;
	});

	select_player_sprite(state);
}

void deck_selection_controller::build_deck_set(components::deck_index index, const std::vector<components::card_enum>& deck)
{
	auto& board = _board->get_component<components::board>();
	auto cols = board.rows / 2 - 1; // don't ask
	
	if (index >= _decks.size())
	{
		_decks.resize(index + 1);
		_preview_positions.resize(index + 1);
	}

	_preview_positions[index].clear();

	for (size_t i = 0; i < deck.size(); ++i)
	{
		int row = i / 2;
		int col = 2+i % 2;
		auto& card = deck.at(i);
		_decks[index].insert(card);
		_preview_positions[index].emplace_back(row, col);
	}
}

void deck_selection_controller::spawn_preview_units(ecs::state& state)
{
	state.each_id<components::game_piece>([&](entity_id id, components::game_piece& unit) {		
		for (auto& hp : unit.health_points)
		{
			_hydrater.remove_entity(hp.id());
		}
		_hydrater.remove_entity(id);
	});
	
	auto& board = _board->get_component<components::board>();
	auto& board_t = _board->get_component<transforms::transform>();
	auto board_id = _board->id();
	auto& selection = _deck_selection->get_component<components::deck_selection>();

	for (size_t i = 0; i < 2; ++i)
	{
		//components::card_enum selected_card_type = *(selection.card_iterators[i]);

		auto& player_c = _players[i]->get_component<components::player>();

		auto& option_id = player_c.team == 1.f
			? selection.p1_deck_options[player_c.deck_selection]
			: selection.p2_deck_options[player_c.deck_selection];

		auto& card = state.find_entity(option_id);
		auto& option_c = card.get_component<components::deck_option>();
		auto deck_index = (size_t) option_c.deck;

		auto& deck = _decks[deck_index];
		auto& preview_offsets = _preview_positions[deck_index];
		
		size_t offset_index = 0;
		for (auto card : deck)
		{
			auto& offset = _preview_positions[deck_index][offset_index++];

			auto coords = player_c.team == 1.f
				? offset
				: glm::ivec2(offset.x, board.rows - 1 - offset.y);
			
			board.spawner.emplace_back(coords.x, coords.y, player_c.team, card);
		}
	}
}

void deck_selection_controller::handle_unit_transform(
	transforms::transform& board_t, 
	transforms::transform& unit_t, 
	components::game_piece& game_piece, 
	components::board& board, 
	entity_id board_id)
{
	game_piece.continuous_board_location = game_piece.board_source;
	unit_t.position = board.grid_to_board(game_piece.continuous_board_location, board_t);
	unit_t.is_matrix_dirty = true;
}

bool deck_selection_controller::can_run() const
{
	return _players[0] != nullptr &&
		_players[1] != nullptr &&
		_deck_selection != nullptr &&
		_board != nullptr &&
		_deck_cursors[0] != nullptr &&
		_deck_cursors[1] != nullptr;
}

void deck_selection_controller::interrupt_animation(
	ecs::state& state, 
	components::deck_selection& selection, 
	unsigned short player_index)
{
	auto& current_id = selection.card_entity_current[player_index];
	auto& next_id = selection.card_entity_next[player_index];

	if (state.has_entity(current_id))
	{
		_hydrater.remove_entity(current_id);
	}

	if (state.has_entity(next_id))
	{
		_hydrater.remove_entity(next_id);
	}

	selection.card_entity_current[player_index] = 0;
	selection.card_entity_next[player_index] = 0;
	selection.card_animation_time[player_index] = 0;
	selection.preview_state[player_index] = components::preview_card_state::changing_forward;
}

void deck_selection_controller::position_cursors(
	ecs::state& state,
	components::deck_selection& deck_selection)
{
	if (!can_run()) return;

	auto& t_one = _deck_cursors[0]->get_component<transforms::transform>();
	auto& t_two = _deck_cursors[1]->get_component<transforms::transform>();

	auto& p1 = _players[0]->get_component<components::player>();
	auto& p2 = _players[1]->get_component<components::player>();

	auto x_increment = deck_selection.option_width + deck_selection.padding;
	
	t_one.position.z = t_two.position.z = -deck_selection.card_depth + 0.1f;
	t_one.position.y = t_two.position.y = deck_selection.card_y;

	t_one.position.x = deck_selection.player_1_options_x + p1.deck_selection * x_increment;
	t_two.position.x = deck_selection.player_2_options_x + p2.deck_selection * x_increment;	
}

void deck_selection_controller::handle_animation(ecs::state& state, components::deck_selection& selection)
{
	for (size_t i = 0; i < 2; i++)
	{
		auto player = _players[i];
		auto& player_comp = player->get_component<components::player>();

		switch (selection.preview_state[i])
		{
			case components::preview_card_state::displaying:
				break;
			case components::preview_card_state::changing_back:
			{
				components::card_enum card = *(selection.card_iterators[i]);
				auto& card_entity = _card_spawner.spawn(card);			
				position_card(player_comp.deck_selection, card_entity, *player, selection, glm::vec3(0.f, 0.f, 0.5f));
				selection.current_position_keyframes[i] = selection.position_keyframes;
				selection.card_entity_next[i] = card_entity.id();
				selection.preview_state[i] = components::preview_card_state::animating;
				break;
			}
			case components::preview_card_state::changing_forward:
			{	
				components::card_enum card = *(selection.card_iterators[i]);
				auto& card_entity = _card_spawner.spawn(card);
				position_card(player_comp.deck_selection, card_entity, *player, selection, glm::vec3(0.f, 0.f, -0.5f));
				selection.current_position_keyframes[i] = selection.position_keyframes_under;						
				selection.card_entity_next[i] = card_entity.id();
				selection.preview_state[i] = components::preview_card_state::animating;
				break;
			}
			case components::preview_card_state::animating:
			{
				animate(state, i, selection);
				break;
			}
		}
	}
}


void deck_selection_controller::animate_position(
	float animation_t, 
	components::deck_selection& selection, 
	unsigned short player_index,
	size_t deck_index,
	ecs::entity& entity,
	std::pair<float, glm::vec3>* current_position_keyframes)
{
	auto position_keyframes = current_position_keyframes;	

	size_t index = 0;
	while (position_keyframes[index].first < animation_t)
		index++;

	auto source = position_keyframes[index - 1].second;
	auto destination = position_keyframes[index].second;
	float t_first = position_keyframes[index - 1].first;
	float t_second = position_keyframes[index].first;
	float t_range = t_second - t_first;
	float t = (animation_t - t_first) / t_range;

	glm::vec3 offset = source + t * (destination - source);

	auto x_increment = selection.option_width + selection.padding;
	auto side_x_offset = 0;

	if (player_index == 0 && deck_index == 1)
		side_x_offset = -(x_increment);
	if (player_index == 1 && deck_index == 0)
		side_x_offset = (x_increment);

	offset.x += animation_t * side_x_offset;

	position_card(deck_index, entity, *_players[player_index], selection, offset);
}

void deck_selection_controller::animate_rotation(
	float animation_t,
	components::deck_selection& selection,
	size_t player_index,
	size_t deck_index,
	ecs::entity& entity)
{
	auto& transform = entity.get_component<transforms::transform>();
	float yaw = glm::pi<float>() * (1 - animation_t);
	transform.rotation.y = yaw;
	transform.is_matrix_dirty = true;
}

void deck_selection_controller::animate(ecs::state& state, size_t player_index, components::deck_selection& selection)
{
	auto deck_index = _players[player_index]->get_component<components::player>().deck_selection;		
	auto next = selection.card_entity_next[player_index];	
	auto& next_entity = state.find_entity(next);	
	auto& next_t = next_entity.get_component<transforms::transform>();

	selection.card_animation_time[player_index] += _timer.smoothed_delta_secs();

	auto animation_t = selection.card_animation_time[player_index] / selection.card_animation_time_total;
	animation_t = std::clamp(animation_t, 0.f, 1.f);
	auto animation_t_reverse = 1.f - animation_t;

	animate_position(animation_t, selection, player_index, deck_index, next_entity, selection.position_keyframes);	
	animate_rotation(animation_t, selection, player_index, deck_index, next_entity);
	
	next_t.is_matrix_dirty = true;	
	
	auto current = selection.card_entity_current[player_index];
	
	if (current != 0)
	{
		auto& current_entity = state.find_entity(current);
		auto& current_t = current_entity.get_component<transforms::transform>();

		animate_position(animation_t_reverse, selection, player_index, deck_index, current_entity, selection.position_keyframes_under);
		animate_rotation(animation_t_reverse, selection, player_index, deck_index, current_entity);
	}

	// clean up when animation is over
	if (animation_t >= 1.f)
	{
		selection.preview_state[player_index] = components::preview_card_state::displaying;
		selection.card_animation_time[player_index] = 0.f;
		selection.card_entity_current[player_index] = next;
		selection.card_entity_next[player_index] = 0;

		if (state.has_entity(current)) _hydrater.remove_entity(current);
	}
}
