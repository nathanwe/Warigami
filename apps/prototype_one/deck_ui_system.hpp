#ifndef __DECK_UI_SYSTEM_HPP_
#define __DECK_UI_SYSTEM_HPP_

#include <cstdint>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include "components/card.hpp"
#include "components/deck_ui.hpp"

class deck_ui_system : public ecs::system_base
{
public:
	void update(ecs::state& r_state) override
	{
		r_state.each<components::deck_ui>([&](components::deck_ui& deck_ui) {
			for (size_t i = 0; i < deck_ui.CardsPerPlayer; ++i)
			{
				if (deck_ui.p1_hand[i])
				{
					set_card_loc(deck_ui.p1_hand[i], i, 1);
				}

				if (deck_ui.p2_hand[i])
				{
					set_card_loc(deck_ui.p2_hand[i], i, -1);
				}
			}
		});
	}

private:
	static void set_card_loc(entity_id card_entity, std::uint32_t offset, std::int32_t team)
	{

	}

};

#endif