#ifndef GAME_PLAYER_CONTROLLER_HPP
#define GAME_PLAYER_CONTROLLER_HPP


#include "ecs/state.hpp"
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <asset/scene_hydrater.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>

#include "components/player.hpp" 
#include "components/board_square.hpp"
#include "components/terrain.hpp"
#include "components/game_piece.hpp"
#include <util/sign.hpp>
#include "components/to_spawn.hpp"

struct player_controls
{
	core::controls card1;
	core::controls card2;
	core::controls card3;
	core::controls card4;
	core::controls dice_button;	
};

struct player_values
{
	float forward = 0;
	float left = 0;
	glm::vec3 team_color;
};

struct player_specific_data
{
	player_controls& controls;
	player_values values;
};

class player_controller : public ecs::system_base
{
public:

	player_controller(core::game_input_manager& input, core::frame_timer& timer,
		event::EventManager& _event_manager, asset::scene_hydrater& _hydrater) :
		m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater)
	{

	}


	void update(ecs::state& r_state) override
	{
		r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, components::board& board) {

			board_reset(r_state);

			r_state.each<components::player>([&](components::player& player)
				{
					auto player_specifics = build_player_specific_data(player);
					auto& controls = player_specifics.controls;
					auto forward = player_specifics.values.forward;
					auto left = player_specifics.values.left;

					//count the board squares controled by both players at the same time
					int p_1_squares = 0;
					int p_minus1_squares = 0;
					r_state.each<components::board_square>([&](components::board_square& square)
						{
							if (square.team == 1.0f) {
								++p_1_squares;
							}
							if (square.team == -1.0f) {
								++p_minus1_squares;
							}
						});
					draw_goalposts(r_state, player);
					gather_points(r_state, player);					
					handle_player_selection(player, forward, left, board);
					do_everything(player, r_state, player_specifics, board, board_id);
					gain_energy(r_state, player, board, p_1_squares, p_minus1_squares);
					
				});
			});
	}

private:
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
	asset::scene_hydrater& hydrater;

	int find_selected_card_index(player_controls& controls)
	{
		if (m_input.is_input_started(controls.card1))
			return 0;
		else if (m_input.is_input_started(controls.card2))
			return 1;
		else if (m_input.is_input_started(controls.card3))
			return 2;
		else if (m_input.is_input_started(controls.card4))
			return 3;

		return -1;
	}
	void gain_energy(ecs::state& r_state, components::player& player
		, components::board& board, float p1_squares, float pminus1_squares) {

		float square_percent = player.team == 1.0f ? p1_squares : pminus1_squares;
		square_percent /= (board.columns * board.rows);
		player.ticks_to_energy_grow -= board.ticker_dt;
		if (player.ticks_to_energy_grow < 0)
		{
			player.ticks_to_energy_grow = player.ticks_per_energy_grow;
			player.energy = std::min(player.energy + 1.0f * square_percent, player.max_energy);
		}
	}
	void board_reset(ecs::state& r_state)
	{
		r_state.each<components::board_square, transforms::transform, rendering::renderable_mesh_static>([&](
			components::board_square& square,
			transforms::transform& transform,
			rendering::renderable_mesh_static& render_mesh_s)
			{
				render_mesh_s.material.param_diffuse = glm::vec3(0.4, 0.4, 0.4);
				if (square.x % 2 == 0) {
					render_mesh_s.material.param_diffuse -= glm::vec3(0.1f, 0.1f, 0.1f);
				}
				row_select(transform, 1);
				if (square.team == 1.0f) { //TODO: refactor so team color is not hardcoded
					render_mesh_s.material.param_diffuse += glm::vec3(0.4f, -0.2f, -0.2f);
				}
				if (square.team == -1.0f) { //TODO: refactor so team color is not hardcoded
					render_mesh_s.material.param_diffuse += glm::vec3(-0.2f, -0.2f, 0.4f);
				}
			});
	}
	void draw_goalposts(ecs::state& r_state, components::player& player) {
		r_state.each<components::board_square, transforms::transform, rendering::renderable_mesh_static>([&](
			components::board_square& square,
			transforms::transform& transform,
			rendering::renderable_mesh_static& render_mesh_s)
			{
				if (square.y == player.score_column) {
					render_mesh_s.material.param_diffuse += glm::vec3(0.225f, 0.215f, 0.0f);
				} 
			});
	}

	player_specific_data build_player_specific_data(components::player& player)
	{
		//set p1&p2 diffrences
		auto& controls = player.team == 1.f ? p1_controls : p2_controls;
		auto forward = player.team == 1.0f ? m_input.forward() : m_input.forward_player2();
		auto left = player.team == 1.0f ? m_input.strafe() : m_input.strafe_player2();
		auto team_color = player.team ==1.0f ? glm::vec3(1,0,0) : glm::vec3(0, 0, 1);
		return { controls, {forward, left, team_color } };
	}

	void handle_player_selection(components::player& player, float forward, float left, components::board& board)
	{
		if (player.select_delay > 0.f)
		{
			player.select_delay -= m_timer.smoothed_delta_secs();
		}
		else
		{		
			auto vertical_input_active = std::abs(forward) > .4f;
			auto dir_v = util::sign(forward);
			auto under_limit_v = dir_v < 0 ? player.selected_row < board.columns-1 : player.selected_row > 0;
			player.select_delay = 0.1f;

			if (vertical_input_active && under_limit_v)
			{
				player.select_delay = 0.1f;
				player.selected_row -= dir_v;
			}

			auto horizontal_input_active = std::abs(left) > .4f;
			auto dir_h = -util::sign(left);
			auto under_limit_h = dir_h < 0 ? player.selected_column < board.rows-1 : player.selected_column > 0;
			player.select_delay = 0.1f;

			if (horizontal_input_active && under_limit_h)
			{
				player.select_delay = 0.1f;
				player.selected_column -= dir_h;
			}
		}
	}

	void gather_points(ecs::state& r_state, components::player& player)
	{
		r_state.each<components::game_piece>([&](components::game_piece& game_piece)
			{
				if (game_piece.team != player.team) {
					player.points += game_piece.give_points;
				}
				else
				{
					player.points += game_piece.give_points / 2;
				}
			});
		if (player.points >= 20) { //TODO: fix hardcode
			player.points -= 20;
			player.bonus_dice++;
		}
	}
	void do_everything(
		components::player& player,
		ecs::state& r_state,
		player_specific_data& player_specifics,
		components::board& board,
		entity_id board_id)
	{
		auto& controls = player_specifics.controls;
		int loc = find_selected_card_index(controls);
		std::random_device rd;
		auto rng = std::default_random_engine{ rd() };
		std::uniform_int_distribution<int> dice(0, 3);
		std::uniform_int_distribution<int> dice_shapes(0, (int)components::dice_nets::NUM - 1);
		auto d4 = std::bind(dice, rng);

		//place card
		if (loc != -1) {
			player.selected_card = player.hand[loc];
			player.selected_card_location = loc;
			bool taken = false;
			r_state.each<components::game_piece>([&](components::game_piece& piece)
				{
					if (piece.board_source == glm::ivec2(player.selected_row, player.score_column))
					{
						taken = true;
					}
				});
			
			if (!taken && player.energy >= components::card_costanamos[(int)player.hand[player.selected_card_location]])
			{
				board.spawner.emplace_back(
					player.selected_row,
					player.spawn_column,
					player.team,
					player.hand[player.selected_card_location]);

				player.energy -= components::card_costanamos[(int)player.hand[player.selected_card_location]];
				player.hand[player.selected_card_location] = player.safe_draw();
				player.state = components::PLAYER_STATE::BASE;
			}
		}
		// place dice
		if (m_input.is_input_started(controls.dice_button) && player.bonus_dice > 0) {
			
			std::vector<glm::ivec2> net = player.create_shifted_net(glm::ivec2(player.selected_row, player.selected_column),
				player.current_dice_shape, (components::rotate_states)d4(), d4()%2==0);
			r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
				{

					bool foo = player.net_check(glm::ivec2(square.x, square.y), net);		
					if (foo)
					{
						create_terrain(r_state, transform.position, board_id, glm::ivec2(square.x, square.y), components::TERRAIN_ENUM::FIRE, player.team);
					}
				});			
			player.bonus_dice--;			
			auto next_dice_shape = static_cast<components::dice_nets>(dice_shapes(rng));			
			player.current_dice_shape = next_dice_shape;
			
		}
		//show cursor
		r_state.each<components::board_square, transforms::transform, rendering::renderable_mesh_static>(
			[&](components::board_square& square, transforms::transform& transform, rendering::renderable_mesh_static& render_mesh_s)
			{		
				if (square.x == player.selected_row) {
					//row_increase(transform, 0.3f);
					render_mesh_s.material.param_diffuse += player_specifics.values.team_color * 0.3f;
					if (square.y == player.selected_column) {
						row_increase(transform, 0.3f);
						//render_mesh_s.material.param_diffuse += player_specifics.values.team_color * 0.3f;
					}
				}													
			});

	}

	void do_base(components::player& player, ecs::state& r_state, player_specific_data& player_specifics)
	{
		auto& controls = player_specifics.controls;
		int loc = find_selected_card_index(controls);

		if (loc != -1) {
			player.selected_card = player.hand[loc];
			player.selected_card_location = loc;
			player.state = components::PLAYER_STATE::UNIT_PLACEMENT;
		}

		if (m_input.is_input_started(controls.dice_button) && player.bonus_dice > 0) {
			player.state = components::PLAYER_STATE::DICE_PLACEMENT;
		}

		r_state.each<components::board_square, transforms::transform>(
			[&](components::board_square& square, transforms::transform& transform)
			{
				if (square.y == player.spawn_column)
				{
					square.x == player.selected_row
						? row_increase(transform, 0.3f)
						: row_increase(transform, 0);
				}
			});
	}

	void do_placement(components::player& player, ecs::state& r_state, player_specific_data& player_specifics, components::board& board)
	{
		auto& controls = player_specifics.controls;
		int loc = find_selected_card_index(controls);

		if (loc != player.selected_card_location && loc != -1)
		{
			player.state = components::PLAYER_STATE::BASE;
		}

		r_state.each<components::board_square, transforms::transform>([&](
			components::board_square& square,
			transforms::transform& transform)
			{
				if (square.y == player.spawn_column)
				{
					square.x == player.selected_row
						? row_increase(transform, 0.5f)
						: row_increase(transform, 0);
				}
			});

		if (m_input.is_input_started(controls.card1))
		{
			bool taken = false;
			r_state.each<components::game_piece>([&](components::game_piece& piece)
				{
					if (piece.board_source == glm::ivec2(player.selected_row, player.spawn_column))
					{
						taken = true;
					}
				});

			if (!taken && player.energy >= components::card_costanamos[(int)player.hand[player.selected_card_location]])
			{
				board.spawner.emplace_back(
					player.selected_row,
					player.team >= 0 ? 0 : 8,
					player.team,
					player.hand[player.selected_card_location]);

				player.energy -= components::card_costanamos[(int)player.hand[player.selected_card_location]];
				player.hand[player.selected_card_location] = player.safe_draw();
				player.state = components::PLAYER_STATE::BASE;
			}
		}
		else if (m_input.is_input_started(controls.card2))
		{
			player.state = components::PLAYER_STATE::BASE;
		}
	}

	void do_dice_placement(
		components::player& player, 
		ecs::state& r_state, 
		player_specific_data& player_specifics, 
		components::board& board, 
		entity_id board_id)
	{
		auto& controls = player_specifics.controls;

		std::vector<glm::ivec2> net = player.create_shifted_net(glm::ivec2(player.selected_row, player.selected_column),
			player.current_dice_shape, player.rotate_state, player.flip_state);
		r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
			{

				bool foo = player.net_check(glm::ivec2(square.x, square.y), net);
				foo ? row_increase(transform, 0.5f) : row_increase(transform, 0);
				if (m_input.is_input_started(controls.card1) && foo && player.energy >= components::dice_costanamos)
				{
					row_select(transform, 1);
					create_terrain(r_state, transform.position, board_id, glm::ivec2(square.x, square.y), components::TERRAIN_ENUM::FIRE, player.team);
				}
			});

		if (m_input.is_input_started(controls.card1) && player.energy >= components::dice_costanamos) {
			player.bonus_dice--;
			player.energy -= components::dice_costanamos;
			auto next_dice_shape = static_cast<components::dice_nets>(static_cast<int>(player.current_dice_shape) + 1);
			if (next_dice_shape == components::dice_nets::NUM) { // replace with random next dice, rather than iterating through dice shapes
				next_dice_shape = components::dice_nets::T;
			}
			player.current_dice_shape = next_dice_shape;
			player.state = components::PLAYER_STATE::BASE;
		}
		else if (m_input.is_input_started(controls.card2))
		{
			player.state = components::PLAYER_STATE::BASE;
		}
		else if (m_input.is_input_started(controls.card3)) {
			player.flip_state = !player.flip_state;
		}
		else if (m_input.is_input_started(controls.card4)) {
			auto next_rotate = static_cast<components::rotate_states>(static_cast<int>(player.rotate_state) + 1);
			if (next_rotate == components::rotate_states::NUM) {
				next_rotate = components::rotate_states::ZERO;
			}
			player.rotate_state = next_rotate;
		}

	}

	static components::card_enum card_select(components::player& player, components::PLAYER_STATE* playerState, int num)
	{
		if (components::card_costanamos[(int)player.hand[num]] > player.energy)
		{
			*playerState = components::PLAYER_STATE::BASE;
		}
		else
		{
			*playerState = components::PLAYER_STATE::UNIT_PLACEMENT;
		}
		return player.hand[num];
	}

	static void row_select(transforms::transform& transform, float num)
	{
		transform.scale.y = num;
		transform.is_matrix_dirty = true;
	}
	static void row_increase(transforms::transform& transform, float num)
	{
		transform.scale.y += num;
		transform.is_matrix_dirty = true;
	}


	void create_terrain(ecs::state& r_state, glm::vec3 relitive_pos, entity_id parent,
		glm::ivec2 location, components::TERRAIN_ENUM type, float team, int damage = 1, int charges = -1, int duration = -1)
	{
		int existing_terrain_here = 0;
		r_state.each<components::terrain>([&](components::terrain& that_terrain)
			{
				if (that_terrain.location == location) {
					++existing_terrain_here;
				}
			});

		static const std::string TerrainPrototypes[5] = {
			"assets/prototypes/terrain.json",
			"assets/prototypes/terrain_fire_p1.json",
			"assets/prototypes/terrain_fire_p2.json",
			"assets/prototypes/terrain_web_p1.json",
			"assets/prototypes/terrain_web_p2.json"
		};

		size_t type_index = 0;
		if (team == 1.0f && type == components::TERRAIN_ENUM::FIRE) {
			type_index = 1;
		}
		else if (team == -1.0f && type == components::TERRAIN_ENUM::FIRE) {
			type_index = 2;
		}
		else if (team == 1.0f && type == components::TERRAIN_ENUM::WEB) {
			type_index = 3;
		}
		else if (team == -1.0f && type == components::TERRAIN_ENUM::WEB) {
			type_index = 4;
		}
		else {
			type_index = 0; // this should never happen
		}

		ecs::entity& nerd = hydrater.add_from_prototype(TerrainPrototypes[type_index]);

		auto& nerdT = nerd.get_component<transforms::transform>();
		nerdT.position = relitive_pos;
		nerdT.position.y += .6 + existing_terrain_here * 0.3f;
		nerdT.has_parent = true;
		nerdT.parent = parent;
		nerdT.is_matrix_dirty = true;

		auto& nerdTerrain = nerd.get_component<components::terrain>();
		nerdTerrain.charges = charges;
		nerdTerrain.damage = damage;
		nerdTerrain.duration = duration;
		nerdTerrain.location = location;
		nerdTerrain.team = team;
		nerdTerrain.type = type;
	}



	player_controls p1_controls{
			core::CARD1_CONTROL,
			core::CARD2_CONTROL,
			core::CARD3_CONTROL,
			core::CARD4_CONTROL,
			core::DIE1_CONTROL
	};

	player_controls p2_controls{
			core::CARD1_CONTROL_PLAYER2,
			core::CARD2_CONTROL_PLAYER2,
			core::CARD3_CONTROL_PLAYER2,
			core::CARD4_CONTROL_PLAYER2,
			core::DIE1_CONTROL_PLAYER2
	};


};

#endif
