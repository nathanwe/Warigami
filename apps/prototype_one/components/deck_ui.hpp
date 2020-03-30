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

    std::uint32_t child_count = 0;
    entity_id children[64];

    float min_card_x { -16.f };
    float max_card_x { 16.f };
    float card_y { -7.25f };
    float card_depth { 25.f };
    float deck_gap { 3.4f };

    void replace_child(entity_id old_val, entity_id new_val)
    {
        for (std::uint32_t i = 0; i < child_count; i++)
        {
            if (children[i] == old_val)
            {
                children[i] = new_val;
                return;
            }
        }
    }

    void remove_child(std::uint32_t index)
    {
        entity_id& last = children[child_count-1];
        entity_id& current = children[index];
        std::swap(last, current);
        child_count--;
    }
};


}



#endif //WIZARDENGINE_DECK_UI_HPP
