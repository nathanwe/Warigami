#ifndef GAME_PLAYER_CONTROLLER_HPP
#define GAME_PLAYER_CONTROLLER_HPP


#include "ecs/state.hpp"
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <asset/scene_hydrater.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <rendering/texture.hpp>
#include <util/math.hpp>
#include <util/sign.hpp>
#include <audio/audio_emitter.hpp>

#include "components/player.hpp" 
#include "components/board_square.hpp"
#include "components/terrain.hpp"
#include "components/game_piece.hpp"
#include "components/to_spawn.hpp"
#include "components/pause.hpp"
#include "components/spawn_effect.hpp"

#include "game_util/player_specifics.hpp"
#include "param_models/anim_params.hpp"
#include <algorithm>
#include <string>


class player_controller : public ecs::system_base
{


public:

	player_controller(core::game_input_manager& input, core::frame_timer& timer,
		event::EventManager& _event_manager, asset::scene_hydrater& _hydrater,
		rendering::asset_cache& render_assets) :
		m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater),
		m_render_assets(render_assets)
	{

	}

	void initialize(ecs::state& state) override
	{
		select_spawn_column(state);

		using namespace std::string_literals;
		capture_spider_tex_id = m_render_assets.get<rendering::texture>("assets/textures/terrain/spiderweb.png"s).id;
		capture_fantasy_tex_id = m_render_assets.get<rendering::texture>("assets/textures/terrain/starryTerrain.png"s).id;
	}

	void update(ecs::state& r_state) override
	{
		if (should_skip_update(r_state))
		{
			return;
		}
		toggle_owned_energy_orbs(r_state);

		r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, components::board& board) {
			if (!(board.state == components::game_state::gameplay || board.state == components::game_state::countdown)) return;

			board_reset(r_state);

			r_state.each<components::player>([&](components::player& player)
				{
					player_specific_data player_specifics(player, m_input);
					auto& controls = player_specifics.controls;
					auto forward = player_specifics.values.forward;
					auto left = player_specifics.values.left;
					update_pull(player, player_specifics.values, board.state == components::game_state::gameplay);
					if (player.pull) {
						r_state.each_id< transforms::transform, components::selection_arrow, audio::audio_emitter>([&](
							entity_id id1,
							transforms::transform& transform,
							components::selection_arrow& select,
							audio::audio_emitter& emitter)
							{
								auto sound = player.deck_selection == 0 ? 3 : 6;

								if (select.team == player.team) {
									if (emitter.emitter_sounds[sound].state != audio::sound_state::playing)
									{
										emitter.set_sound_state(sound, audio::sound_state::playback_requested);
									}
								}
							});
					}
					else {
						r_state.each_id< transforms::transform, components::selection_arrow, audio::audio_emitter>([&](
							entity_id id1,
							transforms::transform& transform,
							components::selection_arrow& select,
							audio::audio_emitter& emitter)
							{
								auto sound = player.deck_selection == 0 ? 3 : 6;

								if (select.team == player.team) {
									if (emitter.emitter_sounds[sound].state == audio::sound_state::playing)
									{
										emitter.set_sound_state(sound, audio::sound_state::stop_requested);
									}
								}
							});
					}
					handle_player_selection(r_state, player, forward, left, board);
					handle_controls(player, r_state, player_specifics, board, board_id);
					//toggle_AI(player, controls);
					gain_flower_energy(r_state, player, controls);
					show_cursor(player, r_state, player_specifics);
				});
			});
		select_sprite_for_player(r_state);
	}

	
private:
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
	asset::scene_hydrater& hydrater;
	rendering::asset_cache& m_render_assets;

	int capture_spider_tex_id = 0;
	int capture_fantasy_tex_id = 0;

	static constexpr std::array<glm::vec2, 1> m_queen_energy_offsets{ glm::vec2(0.f, 0.f) };
	static constexpr std::array<glm::vec2, 2> m_queen_place_offsets{ glm::vec2(0.f, 0.3333f), glm::vec2(0.25f, 0.3333f) };
	static constexpr std::array<glm::vec2, 4> m_queen_move_offsets{ glm::vec2(0.f, 0.6666f), glm::vec2(0.25f, 0.6666f), glm::vec2(0.5f, 0.6666f), glm::vec2(0.75f, 0.6666f) };


	void select_spawn_column(ecs::state& state)
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

	bool should_skip_update(ecs::state& r_state)
	{
		auto e = r_state.first<components::pause>([](auto& pause)
			{
				return true;
			});
		if (e)
		{
			auto& pause = e->get_component<components::pause>();
			if (pause.is_game_paused || !pause.is_game_started || pause.is_game_over)
			{
				return true;
			}
		}
		return false;
	}

	void select_sprite(components::player& player, components::selection_arrow& selector, rendering::renderable_mesh_static& render)
	{
		auto& anim_data = player.animation_parameters;
		auto& offset = render.material.texture_offset;
		float time = m_timer.total_s();

		if (anim_data.m_is_placing_unit)
		{
			float time_diff = time - anim_data.m_time_started_placing_unit;
			if (time_diff < components::player::m_time_between_place_sprites)
			{
				offset = m_queen_place_offsets[0];
			}
			else
			{
				offset = m_queen_place_offsets[1];
			}
		}
		else if (anim_data.m_is_vacuuming_energy)
		{
			offset = m_queen_energy_offsets[0];
		}
		else if (anim_data.m_walked_recently)
		{
			float time_diff = time - anim_data.m_time_last_walked;
			float steps = time_diff / components::player::m_time_between_walk_sprites;
			int floored_steps = static_cast<int>(steps);
			anim_data.m_current_move_sprite = floored_steps;
			anim_data.m_current_move_sprite %= m_queen_move_offsets.size();
			offset = m_queen_move_offsets[anim_data.m_current_move_sprite];
		}
		else
		{
			offset = m_queen_move_offsets[0];
		}
		if (player.deck_selection == components::player::spider_deck)
		{
			if (selector.team > 0.f)
			{
				offset += glm::vec2(0.25f, 0.f);
			}
		}
		else if (player.deck_selection == components::player::fantasy_deck)
		{
			if (selector.team > 0.f)
			{
				render.material.texture_scale = { 0.25f, 0.3333f };
			}
			else
			{
				render.material.texture_scale = { -0.25f, 0.3333f };
				offset += glm::vec2(0.25f, 0.f);
			}
		}
	}

	void select_sprite_for_player(ecs::state& state)
	{
		state.each<components::player>(
			[&](auto& player)
			{
				state.each<components::selection_arrow, rendering::renderable_mesh_static>(
					[&](auto& selector, auto& render)
					{
						if (player.team == selector.team)
						{
							select_sprite(player, selector, render);
						}
					}
				);
			});
	}

	void toggle_owned_energy_orbs(ecs::state& r_state)
	{
		r_state.each<components::player>([&](auto& player)
			{
				r_state.each<components::selection_arrow>([&](auto& selector)
					{
						if (player.team == selector.team)
						{
							for (unsigned int i = 0; i < selector.energy_orbs.size(); ++i)
							{
								auto& render = selector.energy_orbs[i].template get_component<rendering::renderable_mesh_static>();
								render.is_enabled = player.energy > i;
								//render.material.param_diffuse = player.team == 1.0f ? glm::vec3(1, .5, .5) : glm::vec3(.5, .5, 1);
							}
						}
					});
			});
	}

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

	void gain_flower_energy(ecs::state& r_state, components::player& player, player_controls& controls)
	{
		auto& anim_data = player.animation_parameters;
		anim_data.m_is_vacuuming_energy = !anim_data.m_is_placing_unit && player.pull;
		if (anim_data.m_is_vacuuming_energy)
		{
			r_state.each<components::terrain, components::board_square, transforms::transform>(
				[&](auto& terrain, auto& square, auto& square_t)
				{
					bool is_flower_grown = terrain.growth_stage == 3;
					bool is_flower_selected = player.selected_row == terrain.location.x;
					bool is_flower_captured = square.team == player.team;
					if (is_flower_grown && is_flower_selected && is_flower_captured)
					{
						terrain.growth_stage--;
						square_t.rotation = glm::vec3(0, AI_MATH_HALF_PI * player.team, 0);

						auto& ball = hydrater.add_from_prototype("assets/prototypes/energy_ball_detached.json");
						auto& ball_t = ball.get_component<transforms::transform>();
						auto& ball_c = ball.get_component<components::energy_ball>();
						auto& ball_r = ball.get_component<rendering::renderable_mesh_static>();
						ball_t.position = square_t.local_to_world[3];
						ball_t.local_to_world = square_t.local_to_world;
						ball_t.position[1] = ball_t.local_to_world[3][1] += 4.f;
						ball_c.team = player.team;
						ball_c.lane = player.selected_row;
						//ball_r.material.param_diffuse = player.team == 1.0f ? glm::vec3(1, .5, .5) : glm::vec3(.5, .5, 1);
					}

				}
			);
		}
		r_state.each<components::terrain, components::board_square, transforms::transform>(
			[&](auto& terrain, auto& square, auto& square_t)
			{
				bool is_flower_selected = player.selected_row == terrain.location.x;
				bool is_flower_captured = square.team == player.team;
				bool is_flower_bent = terrain.growth_stage == 2;
				if (is_flower_bent && is_flower_captured &&
					(!is_flower_selected || !player.pull))
				{
					terrain.growth_stage--;
					square_t.rotation = glm::vec3(0, -AI_MATH_HALF_PI, 0);
				}
			});
	}

	void board_reset(ecs::state& r_state)
	{
		r_state.each<components::board_square, transforms::transform, rendering::renderable_mesh_static>(
			[&](auto& square, auto& transform, auto& render_mesh_s)
			{
				transform.is_matrix_dirty = true;
				r_state.each<components::player>(
					[&](auto& player)
					{
						if (square.team == player.team)
						{
							auto& capture_render = square.captured->get_component<rendering::renderable_mesh_static>();
							capture_render.is_enabled = square.team != 0.f;
							if (square.team == 1.f)
							{
								capture_render.material.tint_color = glm::vec3(1.f, 0.f, 0.f);
							}
							else if (square.team == -1.f)
							{
								capture_render.material.tint_color = glm::vec3(0.f, 0.f, 1.f);
							}

							switch (player.deck_selection)
							{
							case components::player::spider_deck:
								capture_render.material.texture_diffuse = capture_spider_tex_id;
								break;
							case components::player::fantasy_deck:
								capture_render.material.texture_diffuse = capture_fantasy_tex_id;
								break;
							default:
								std::cout << "Unexpected deck selection." << std::endl;
								assert(false);
								break;
							}
						}
					});
			});
	}

	void handle_player_selection(ecs::state& r_state, components::player& player, float forward, float left, components::board& board)
	{
		if (player.select_delay > 0.f)
		{
			player.select_delay -= m_timer.smoothed_delta_secs();
		}
		else if (!player.pull)
		{
			auto& anim_data = player.animation_parameters;
			if (!anim_data.m_is_placing_unit)
			{
				auto vertical_input_active = std::abs(forward) > .4f;
				auto dir_v = util::sign(forward);
				auto under_limit_v = dir_v < 0 ? player.selected_row < board.columns - 1 : player.selected_row > 0;
				player.select_delay = 0.1f;

				bool changed_selection = false;
				if (vertical_input_active && under_limit_v)
				{
					player.select_delay = 0.1f;
					player.selected_row -= dir_v;
					changed_selection = true;
					r_state.each_id< transforms::transform, components::selection_arrow, audio::audio_emitter>([&](
						entity_id id1,
						transforms::transform& transform,
						components::selection_arrow& select,
						audio::audio_emitter& emitter)
						{
							if (select.team == player.team) {
								//if (emitter.emitter_sounds[0].state != audio::sound_state::playing)
								//{
									emitter.set_sound_state(2, audio::sound_state::playback_requested);
								//}
							}
						});
				}

				auto& anim_data = player.animation_parameters;
				if (changed_selection)
				{
					anim_data.m_walked_recently = true;
					anim_data.m_time_last_walked = m_timer.total_s();
				}
				else if (anim_data.m_walked_recently && m_timer.total_s() - anim_data.m_time_last_walked > components::player::m_walk_recency_duration)
				{
					anim_data.m_walked_recently = false;
				}
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
		bool placed = false;
		if (!player.pull && board.state == components::game_state::gameplay) {
			placed = player.place_card(loc, m_timer.total_s(), r_state, board.spawner, hydrater);
		}
		if(placed && loc != -1){
			r_state.each_id< transforms::transform, components::selection_arrow, audio::audio_emitter>([&](
				entity_id id1,
				transforms::transform& transform,
				components::selection_arrow& select,
				audio::audio_emitter& emitter)
				{
					if (select.team == player.team) {
						//if (emitter.emitter_sounds[0].state != audio::sound_state::playing)
						//{
						emitter.set_sound_state(1, audio::sound_state::playback_requested);
						//}
					}
				});
		}
		else if (loc != -1) {
			r_state.each_id< transforms::transform, components::selection_arrow, audio::audio_emitter>([&](
				entity_id id1,
				transforms::transform& transform,
				components::selection_arrow& select,
				audio::audio_emitter& emitter)
				{
					if (select.team == player.team) {
						//if (emitter.emitter_sounds[0].state != audio::sound_state::playing)
						//{
						emitter.set_sound_state(0, audio::sound_state::playback_requested);
						//}
					}
				});
		}
	}

	
	void update_pull(components::player& player, player_values& values, bool playing) {
		if (!player.controlled_by_AI && playing) {
			player.pull = std::abs(values.pull) > 0.4f;
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
			[&](auto& square, auto& transform, auto& render_mesh_s)
			{
				if (square.x == player.selected_row && square.y == player.selected_column) {

					// Place arrow in selected tile
					glm::vec3 arrow_pos = transform.position;
					arrow_pos.y += 1;
					arrow_pos.z -= player.team;

					r_state.each<components::selection_arrow, transforms::transform, rendering::renderable_mesh_static>(
						[&](auto& arrow, auto& transform_arrow, auto& render_mesh_s_arrow)
						{
							if (arrow.team == player.team)
							{
								if (transform_arrow.position != arrow_pos)
								{
									auto& anim_data = player.animation_parameters;
									float t = (m_timer.total_s() - anim_data.m_time_last_walked) / 0.125f;
									t = std::clamp(t, 0.f, 1.f);
									transform_arrow.position = util::lerp(transform_arrow.position, arrow_pos, t);
									transform_arrow.is_matrix_dirty = true;
								}
							}
						}
					);
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
};

#endif
