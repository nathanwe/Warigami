//
// Created by sava on 2020-03-07.
//

#ifndef WIZARDENGINE_DECK_UI_HPP
#define WIZARDENGINE_DECK_UI_HPP

#include <cstdint>
#include <optional>
#include <ecs/component.hpp>
#include "player.hpp"

namespace components
{


struct deck_ui : public ecs::component<deck_ui>
{
    std::optional<entity_id> p1_hand[player::MaxCards] {};
    std::optional<entity_id> p2_hand[player::MaxCards] {};

    float min_card_x { -16.f };
    float max_card_x { 16.f };
    float card_y { -7 };
    float card_depth { 25.f };
    float deck_gap { 3.4f };
};


}



#endif //WIZARDENGINE_DECK_UI_HPP
