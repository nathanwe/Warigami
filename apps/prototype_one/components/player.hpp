#ifndef GAME_COMPONENTS_PLAYER_HPP
#define GAME_COMPONENTS_PLAYER_HPP

#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdint>

// fishy dependencies 
#include <asset/scene_hydrater.hpp>
#include <transforms/transform.hpp>
#include "spawn_effect.hpp"	
#include "board.hpp"
//

#include <ecs/component.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/state.hpp>
#include "component_bits.hpp"
#include "card_enum.hpp"
#include "../param_models/anim_params.hpp"
#include "to_spawn.hpp"


namespace components
{
	enum class PLAYER_STATE
	{
		BASE,
		UNIT_PLACEMENT,
		DICE_PLACEMENT,
		LOST,
		WON
	};
	typedef std::uint8_t deck_index;
	const static std::vector<card_enum> spider_deck = {
			card_enum::SCISSORLING_TWIN,
			card_enum::SCISSOR_TROOPER,
			card_enum::SCISSORLING_EGG,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_GOLIATH,
			card_enum::SCISSOR_QUEEN
	};
	const static std::vector<card_enum> spider_deck_test = {
			card_enum::SCISSORLING_TWIN,
			card_enum::SCISSOR_TROOPER,
			card_enum::SCISSORLING_EGG,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_GOLIATH,
			card_enum::SCISSOR_QUEEN
	};

	const std::vector<card_enum> soldier_deck = {
		card_enum::LIGHT_TANK_SOLDIER,
		card_enum::LIGHT_TANK_SOLDIER,
		card_enum::LIGHT_RANGE_SOLDIER,
		card_enum::LIGHT_RANGE_SOLDIER,
		card_enum::LIGHT_FAST_SOLDIER,
		card_enum::LIGHT_FAST_SOLDIER,
		card_enum::MEDIUM_TANK_SOLDIER,
		card_enum::MEDIUM_FAST_SOLDIER,
		card_enum::MEDIUM_RANGE_SOLDIER,
		card_enum::HEAVY_TANK_SOLDIER,
		card_enum::HEAVY_FAST_SOLDIER,
		card_enum::HEAVY_RANGE_SOLDIER
	};
	const std::vector<card_enum> fantasy_deck = {
		card_enum::ERASER,
		card_enum::TROLL,
		card_enum::WIZARD,
		card_enum::ARCHER,
		card_enum::UNICORN,
		card_enum::KNIGHT
	};
	const std::vector<card_enum> fantasy_deck_test = {
		card_enum::ERASER,
		card_enum::ERASER,
		card_enum::TROLL,
		card_enum::TROLL,
		card_enum::WIZARD,
		card_enum::ARCHER,
		card_enum::UNICORN,
		card_enum::KNIGHT,
		card_enum::KNIGHT
	};

	const std::vector<card_enum> decks[]{
		spider_deck,
		fantasy_deck
	};

	const static std::vector<int> card_costanamos =
	{ 0, 0, 3, 3, 5, 4, 5, 4, 8, 3, 3, 3, 4, 4, 4, 5, 5, 5, 0, 0, 3, 5, 3, 5, 3, 5 };
	const static int dice_costanamos = 1;

	enum class dice_nets {
		T = 0,
		SEVEN = 1,
		DROOPY_SEVEN = 2,
		Z = 3,
		T_HAT = 4,
		WX = 5,
		W = 6,
		WM = 7,
		CROSS = 8,
		X = 9,
		I = 10,
		NUM = 11
	};

	enum class rotate_states {
		ZERO = 0,
		NINTY = 1,
		ONE_EIGHTY = 2,
		TWO_SEVENTY = 3,
		NUM = 4

	};

	struct player : ecs::component<player>
	{
		using row_t = std::int16_t;
		using column_t = std::int16_t;

		static constexpr float m_walk_recency_duration = 1.0f;
		static constexpr float m_time_between_place_sprites = 0.25f;
		static constexpr float m_time_between_walk_sprites = 0.25f;
		static constexpr std::uint8_t MaxCards = 4;


		player()
		{
			regrow_deck();
			shuffle();
			redraw();
		}

		float max_energy{ 10.0f };
		float energy = 5.0f;
		float health{ 100.f };
		int points = 0;
		int bonus_dice = 0;
		float team = 0.0f;
		card_enum selected_card;
		int selected_card_location;
		float select_delay = 0.0f;
		components::PLAYER_STATE state = components::PLAYER_STATE::BASE;
		std::vector<card_enum> deck;
		row_t selected_row = 2;
		column_t selected_column = 4;
		int spawn_column = 0;
		int score_column = 1; //If units get here, this player gets hurt
		rotate_states rotate_state = components::rotate_states::ZERO;
		bool flip_state = false;
		components::dice_nets current_dice_shape = dice_nets::SEVEN;
		card_enum hand[MaxCards];
		std::uint8_t card_count = 0;
		bool controlled_by_AI = false;
		int AI_movement_direction = 1;
		bool succ = false;

		float ticks_per_energy_grow{ 2.f };
		float ticks_to_energy_grow{ ticks_per_energy_grow };

		std::vector<card_enum> start_deck{ soldier_deck };

		static constexpr components::deck_index spider_deck = 0;
		static constexpr components::deck_index fantasy_deck = 1;
		deck_index deck_selection{ 0 };

		bool is_ready{ false };

		anim_params animation_parameters{};

		card_enum draw() {
			card_count++;
			if (!deck.empty()) {
				card_enum temp = deck.back();
				deck.pop_back();
				return temp;
			}
			else {
				return card_enum::NO_CARD;
			}
		}
		card_enum safe_draw() {
			card_enum ret = draw();
			if (ret == card_enum::NO_CARD) {
				regrow_deck();
				shuffle();
				ret = draw();
			}
			return ret;
		}

		void shuffle() {
			static std::random_device rd;
			auto rng = std::default_random_engine{ rd() };
			std::shuffle(std::begin(deck), std::end(deck), rng);
		}

		void regrow_deck() {
			deck = start_deck;
		}

		void redraw()
		{
			for (auto& card_slot : hand)
				card_slot = safe_draw();
		}

		std::vector<glm::ivec2> create_shifted_net(glm::ivec2 center,
			dice_nets net, rotate_states rotate, bool flip) {
			std::vector<glm::ivec2> legal_placements;
			int tempx;

			switch (net) {
			case dice_nets::T:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(0, 1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(1, 1));
				break;
			case dice_nets::SEVEN:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(0, 1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(1, 0));
				break;
			case dice_nets::DROOPY_SEVEN:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(0, 1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(1, -1));
				break;
			case dice_nets::Z:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(0, 1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(1, -2));
				break;
			case dice_nets::T_HAT:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(-1, 0));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(1, 0));
				break;
			case dice_nets::WX:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(1, -1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(-1, 0));
				break;
			case dice_nets::W:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(1, -2));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(-1, 0));
				break;
			case dice_nets::WM:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(1, -1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(1, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(-1, 0));
				break;
			case dice_nets::CROSS:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(0, 1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 0));
				legal_placements.push_back(glm::ivec2(1, 0));
				break;
			case dice_nets::X:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(0, 1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 0));
				legal_placements.push_back(glm::ivec2(1, -1));
				break;
			case dice_nets::I:
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(-1, 0));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(-1, 2));
				break;
			}

			if (flip) {
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					it->x = -it->x;
				}
			}

			if (rotate == rotate_states::ZERO) {

			}
			else if (rotate == rotate_states::NINTY)
			{
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					tempx = it->x;
					it->x = -it->y;
					it->y = tempx;
				}
			}
			else if (rotate == rotate_states::ONE_EIGHTY)
			{
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					it->x = -it->x;
					it->y = -it->y;
				}
			}
			else if (rotate == rotate_states::TWO_SEVENTY)
			{
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					tempx = it->x;
					it->x = it->y;
					it->y = -tempx;
				}
			}
			for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
				it->x += center.x;
				it->y += center.y;
			}
			return legal_placements;

		}
		bool net_check(glm::ivec2 test, std::vector<glm::ivec2> legal_placements) {
			bool ret = false;
			for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
				if (*it == test) {
					ret = true;
				}
			}

			return ret;
		}


		void place_card(
			int loc,
			float total_s,
			ecs::state& r_state,
			std::vector<to_spawn>& spawner,
			asset::scene_hydrater& hydrater)
		{
			bool placed = false;
			if (loc != -1) {
				selected_card = hand[loc];
				selected_card_location = loc;
				bool taken = false;
				r_state.each<components::game_piece>([&](components::game_piece& piece)
					{
						if (piece.board_source == glm::ivec2(selected_row, score_column))
						{
							taken = true;
						}
					});

				if (!taken && energy >= components::card_costanamos[(int)hand[selected_card_location]])
				{
					spawner.emplace_back(
						selected_row,
						spawn_column,
						team,
						hand[selected_card_location]);

					energy -= components::card_costanamos[(int)hand[selected_card_location]];
					hand[selected_card_location] = safe_draw();
					placed = true;

					start_spawn_effect(r_state, hydrater, total_s);
					r_state.each_id< transforms::transform, components::selection_arrow, audio::audio_emitter>([&](
						entity_id id1,
						transforms::transform& transform,
						components::selection_arrow& select,
						audio::audio_emitter& emitter)
						{
							if (select.team == team) {
								emitter.set_sound_state(1, audio::sound_state::playback_requested);
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
							if (select.team == team) {
								emitter.set_sound_state(0, audio::sound_state::playback_requested);
							}
						});
				}
			}
			start_end_place_animation(placed, total_s);
		}

		void start_spawn_effect(ecs::state& r_state, asset::scene_hydrater& hydrater, float total_s)
		{
			auto& spawn_effect_entity = deck_selection == components::player::spider_deck ?
				hydrater.add_from_prototype("assets/prototypes/spider_spawn.json") :
				hydrater.add_from_prototype("assets/prototypes/fantasy_spawn.json");
			auto& spawn_effect_transform = spawn_effect_entity.get_component<transforms::transform>();
			auto& spawn_effect = spawn_effect_entity.get_component<components::spawn_effect>();

			spawn_effect.time_started = total_s;

			// copied from spawner and board update system
			auto p_board_entity = r_state.first<components::board>();
			if (p_board_entity != nullptr)
			{
				auto& board = p_board_entity->get_component<components::board>();
				auto& board_transform = p_board_entity->get_component<transforms::transform>();
				glm::vec2 location_on_board = { selected_row, spawn_column };
				spawn_effect_transform.position = board.grid_to_board(location_on_board, board_transform);

				// place in front of units
				spawn_effect_transform.position.x -= 0.1f;

				spawn_effect_transform.is_matrix_dirty = true;
				spawn_effect_transform.has_parent = true;
				spawn_effect_transform.parent = p_board_entity->id();
				spawn_effect_transform.rotation.y = glm::half_pi<float>();
				spawn_effect_transform.scale = glm::vec3(0.45f);
			}
		}

		void start_end_place_animation(bool placed, float total_s)
		{
			if (placed)
			{
				try_start_place_animation(animation_parameters, total_s);
			}
			else
			{
				try_end_place_animation(animation_parameters, total_s);
			}
		}

		void try_start_place_animation(anim_params& anim_data, float total_s)
		{
			if (!anim_data.m_is_placing_unit)
			{
				anim_data.m_is_placing_unit = true;
				anim_data.m_time_started_placing_unit = total_s;
			}
		}

		void try_end_place_animation(anim_params& anim_data, float total_s)
		{
			if (anim_data.m_is_placing_unit)
			{
				bool is_finished_placing = total_s - anim_data.m_time_started_placing_unit > m_time_between_place_sprites * 2.f;
				if (is_finished_placing)
				{
					anim_data.m_is_placing_unit = false;
				}
			}
		}
	};
}

#endif
