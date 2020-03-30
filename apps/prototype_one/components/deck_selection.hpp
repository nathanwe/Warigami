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
        entity_id p1_deck_options[3]{};
        entity_id p2_deck_options[3]{};

        float min_card_x{ -16.f };
        float max_card_x{ 16.f };
        float card_y{ -7.25f };
        float card_depth{ 25.f };
        float deck_gap{ 3.4f };

        size_t num_decks{ 2 };
        float option_width{ 4.f };
        float padding{ 0.5f };
        float total_width{ 32.f };
        float half_width{ total_width / 2.f };
        float quarter_width{ total_width / 4.f };
        float deck_container_width{ num_decks * (padding + option_width) };
        float half_deck_container_width{ deck_container_width / 2 };
        
        float player_1_options_x{ -quarter_width - half_deck_container_width };
        float player_2_options_x{ quarter_width - half_deck_container_width };


        std::uint32_t child_count = 0;
        entity_id children[16];
    };
}


#endif