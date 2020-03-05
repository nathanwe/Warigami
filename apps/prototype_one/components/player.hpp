#ifndef GAME_COMPONENTS_PLAYER_HPP
#define GAME_COMPONENTS_PLAYER_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <random>

namespace components
{
	enum class card_enum {
		NO_CARD = 0,
		BASIC_MELEE = 1,
		BASIC_RANGE = 2,
		BASIC_FAST = 3,
		TOTAL_CARDS
	};
	enum class PLAYER_STATE
	{
		BASE,
		UNIT_PLACEMENT,
		DICE_PLACEMENT,
		LOST,
		WON
	};

	const static std::vector<int> card_costanamos = { 0, 1, 2, 2 };

	const static std::vector<card_enum> start_deck = {
	        card_enum::BASIC_FAST,
			card_enum::BASIC_FAST,
			card_enum::BASIC_MELEE,
			card_enum::BASIC_MELEE,
			card_enum::BASIC_RANGE,
			card_enum::BASIC_RANGE
	};

	enum class dice_nets {
		T = 1,
		SEVEN = 2,
		DROOPY_SEVEN = 3,
		Z = 4,
		T_HAT = 5,
		WX = 6,
		W = 7,
		WM = 8,
		CROSS = 9,
		X = 10,
		I = 11
	};
	

	struct player : ecs::component<player>
	{
	    static const std::uint8_t MaxCards = 4;
        using row_t = std::int16_t;
		using column_t = std::int16_t;

        player()
        {
            regrow_deck();
            shuffle();
            for (auto & card_slot : hand)
                card_slot = draw();
        }

		int energy = 0;
		float health {100.f};
		int bonus_dice = 0;
		float team = 0.0f;
		card_enum selected_card;
		int selected_card_location;
		float select_delay = 0.0f;
		components::PLAYER_STATE state = components::PLAYER_STATE::BASE;

		// Needs a list of cards in hand and dice in pool

        row_t selected_row = 3;
		column_t selected_column = 4;
        card_enum hand[MaxCards];
        std::uint8_t card_count = 0;



		std::vector<card_enum> deck = start_deck;

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
			auto rng = std::default_random_engine{};
			std::shuffle(std::begin(deck), std::end(deck), rng);
		}

		void regrow_deck() {
			deck = start_deck;
		}
		bool net_check(glm::ivec2 test, glm::ivec2 center,
			dice_nets net, int rotate, bool flip) {
			std::vector<glm::ivec2> legal_placements;
			int tempx;
			bool ret = false;
			if (net == dice_nets::SEVEN) {
				legal_placements.push_back(glm::ivec2(0, 0));
				legal_placements.push_back(glm::ivec2(0, 1));
				legal_placements.push_back(glm::ivec2(0, -1));
				legal_placements.push_back(glm::ivec2(0, -2));
				legal_placements.push_back(glm::ivec2(-1, 1));
				legal_placements.push_back(glm::ivec2(1, 0));
			}

			if (flip) {
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					it->x = -it->x;
				}
			}

			if (rotate == 0) {

			}
			else if (rotate == 1)
			{
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					tempx = it->x;
					it->x = -it->y;
					it->y = tempx;
				}
			}
			else if (rotate == 2)
			{
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					it->x = -it->x;
					it->y = -it->y;
				}
			}
			else if (rotate == 3)
			{
				for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
					tempx = it->x;
					it->x = it->y;
					it->y = -tempx;
				}
			}
			for (std::vector<glm::ivec2>::iterator it = legal_placements.begin(); it != legal_placements.end(); ++it) {
				if (center + *it == test) {
					ret = true;
				}
			}

			return ret;
		}
	};
}

#endif
