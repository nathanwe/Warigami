#ifndef __DECK_SELECTION_HPP_
#define __DECK_SELECTION_HPP_

#include <cstdint>
#include <optional>
#include <ecs/component.hpp>
#include "player.hpp"

namespace components
{
    struct deck_selection : public ecs::component<deck_selection>
    {
        std::optional<entity_id> p1_decks[3]{};
        std::optional<entity_id> p2_decks[3]{};

        float min_card_x{ -16.f };
        float max_card_x{ 16.f };
        float card_y{ -7.25f };
        float card_depth{ 25.f };
        float deck_gap{ 3.4f };
    };
}


#endif