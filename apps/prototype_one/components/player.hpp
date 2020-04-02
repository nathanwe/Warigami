#ifndef GAME_COMPONENTS_PLAYER_HPP
#define GAME_COMPONENTS_PLAYER_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"
#include "card_enum.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdint>

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

	const static std::vector<int> card_costanamos = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 2, 2, 2, 5, 5, 5, 8, 8, 8 };
	const static int dice_costanamos = 1;

	const std::vector<card_enum> spider_deck = {
			card_enum::SCISSORLING_TWIN,
			card_enum::SCISSORLING_TWIN,
			card_enum::SCISSORLING_TWIN,
			card_enum::SCISSORLING_TWIN,
			card_enum::SCISSOR_TROOPER,
			card_enum::SCISSOR_TROOPER,
			card_enum::SCISSOR_TROOPER,
			card_enum::SCISSOR_TROOPER,
			card_enum::SCISSOR_TROOPER,
			card_enum::SCISSORLING_EGG,
			card_enum::SCISSORLING_EGG,
			card_enum::SCISSORLING_EGG,
			card_enum::SCISSORLING_EGG,
			card_enum::SCISSORLING_EGG,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_WEBBER,
			card_enum::SCISSOR_GOLIATH,
			card_enum::SCISSOR_GOLIATH,
			card_enum::SCISSOR_GOLIATH,
			card_enum::SCISSOR_QUEEN,
			card_enum::SCISSOR_QUEEN,
			card_enum::SCISSOR_QUEEN,
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

	const std::vector<card_enum> decks[]{
		spider_deck,
		soldier_deck
	};

	const std::vector<card_enum> start_deck = soldier_deck;

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
	    static constexpr std::uint8_t MaxCards = 4;
        using row_t = std::int16_t;
		using column_t = std::int16_t;

        player()
        {
            regrow_deck();
            shuffle();
            for (auto & card_slot : hand)
                card_slot = safe_draw();
        }

		float max_energy{ 10.0f };
		float energy = 5.0f;
		float health {100.f};
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

		float ticks_per_energy_grow{ 2.f };
		float ticks_to_energy_grow{ ticks_per_energy_grow };

		deck_index deck_selection{ 0 };
		bool is_ready{ false };

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
			auto rng = std::default_random_engine{rd()};
			std::shuffle(std::begin(deck), std::end(deck), rng);
		}

		void regrow_deck() {
			deck = start_deck;
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
	};
}

#endif
