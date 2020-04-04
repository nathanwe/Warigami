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
#include "components/pause.hpp"

#include "game_util/player_specifics.hpp"

class player_controller : public ecs::system_base
{
public:

	player_controller(core::game_input_manager& input, core::frame_timer& timer,
		event::EventManager& _event_manager, asset::scene_hydrater& _hydrater) :
		m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater)
	{

	}

	void initialize(ecs::state& state) override 
	{
		state.each<components::player>([&](auto& player)
		{
			state.first<components::board>([&](auto& board)
			{
				player.selected_column = player.team > 0.f ? 0 : board.rows - 1;
				return true;
			});
		});
	}

	void update(ecs::state& r_state) override
	{
		// Skip system when paused
		auto e = r_state.first<components::pause>([](auto& pause)
		{
			return true;
		});
		if (e)
		{
			auto& pause = e->get_component<components::pause>();
			if (pause.is_game_paused || !pause.is_game_started || pause.is_game_over)
			{
				return;
			}
		}

		r_state.each<components::player>([&](auto& player)
		{
			r_state.each<components::selection_arrow>([&](auto& selector)
			{
				if (player.team == selector.team)
				{
					for (unsigned int i = 0; i < selector.energy_orbs.size(); ++i)
					{
						auto& render = selector.energy_orbs[i].get_component<rendering::renderable_mesh_static>();
						render.is_enabled = player.energy > i;
					}
				}
			});
		});

		r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, components::board& board) {
			if (board.state != components::game_state::gameplay) return;

			board_reset(r_state);

			r_state.each<components::player>([&](components::player& player)
				{
					player_specific_data player_specifics(player, m_input);
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

					handle_player_selection(player, forward, left, board);
					handle_controls(player, r_state, player_specifics, board, board_id);
					toggle_AI(player, player_specifics.controls);
					gain_flower_energy(r_state, player, controls);
					show_cursor(player, r_state, player_specifics);
					
				});
			});
	}

	void public_place_card(int loc, components::player& player, ecs::state& r_state, components::board& board) {
		place_card(loc, player, r_state, board);
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
	void gain_flower_energy(ecs::state& r_state, components::player& player, player_controls&controls ) {
		r_state.each<components::terrain, components::board_square, transforms::transform>
			([&](components::terrain& terrain, components::board_square& square, transforms::transform& square_t) {
			if (terrain.growth_stage == 3 && player.selected_row == terrain.location.x
				&& square.team == player.team && (m_input.is_input_active(controls.dice_button2)||player.controlled_by_AI)) {
				terrain.growth_stage--;
				square_t.rotation = glm::vec3(0, AI_MATH_HALF_PI * player.team, 0);
				player.energy += 2;
			}
			if (terrain.growth_stage == 2 && square.team == player.team 
				&& (player.selected_row != terrain.location.x || 
					(!player.controlled_by_AI &&!m_input.is_input_active(controls.dice_button2)))) {
				terrain.growth_stage--;
				square_t.rotation = glm::vec3(0, -AI_MATH_HALF_PI, 0);
			}
			});
	}
	void board_reset(ecs::state& r_state)
	{
		r_state.each<components::board_square, transforms::transform, rendering::renderable_mesh_static>([&](
			components::board_square& square,
			transforms::transform& transform,
			rendering::renderable_mesh_static& render_mesh_s)
			{
				render_mesh_s.material.tint_color = glm::vec3(0.4, 0.4, 0.4);
				if (square.x % 2 == 0) {
					render_mesh_s.material.tint_color -= glm::vec3(0.02f, 0.02f, 0.02f);
				}
				row_select(transform, 1);
				if (square.team == 1.0f) { //TODO: refactor so team color is not hardcoded
					render_mesh_s.material.tint_color += glm::vec3(0.2f, -0.1f, -0.1f);
				}
				if (square.team == -1.0f) { //TODO: refactor so team color is not hardcoded
					render_mesh_s.material.tint_color += glm::vec3(-0.1f, -0.1f, 0.2f);
				}
			});
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
		}
	}

	void handle_controls(
		components::player& player,
		ecs::state& r_state,
		player_specific_data& player_specifics,
		components::board& board,
		entity_id board_id)
	{
		auto& controls = player_specifics.controls;
		int loc = find_selected_card_index(controls);
		if (!m_input.is_input_active(controls.dice_button2)) {
			place_card(loc, player, r_state, board);
		}
		
	}
	void place_card(int loc, components::player& player, ecs::state& r_state, components::board& board) {
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
			}
		}
	}
	void toggle_AI(components::player& player, player_controls& controls) {
		if (m_input.is_input_started(controls.dice_button)) {
			player.controlled_by_AI = !player.controlled_by_AI;
		}
	}

	void show_cursor(
		components::player& player,
		ecs::state& r_state,
		player_specific_data& player_specifics)
	{
		r_state.each<components::board_square, transforms::transform, rendering::renderable_mesh_static>(
			[&](components::board_square& square, transforms::transform& transform, rendering::renderable_mesh_static& render_mesh_s)
			{
				if (square.x == player.selected_row && square.y == player.selected_column) {

					// Place arrow in selected tile
					glm::vec3 arrow_pos = transform.position;
					arrow_pos.y += 1;

					r_state.each<components::selection_arrow, transforms::transform, rendering::renderable_mesh_static>(
						[&](components::selection_arrow& arrow, transforms::transform& transform_arrow, rendering::renderable_mesh_static& render_mesh_s_arrow)
						{					
							if (arrow.team == player.team) 
							{
								transform_arrow.position = arrow_pos;
								transform_arrow.position.z -= player.team;

								transform_arrow.is_matrix_dirty = true;

								render_mesh_s_arrow.material.texture_offset = player.team > 0.f ? glm::vec2(0.25f, 0.f) : glm::vec2(0.f, 0.f);
							}
						});
				}
			});
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
};

#endif
