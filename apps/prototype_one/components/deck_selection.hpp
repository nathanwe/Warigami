#ifndef __DECK_SELECTION_HPP_
#define __DECK_SELECTION_HPP_

#include <set>
#include <cstdint>
#include <optional>
#include <ecs/component.hpp>
#include "player.hpp"
#include "card_enum.hpp"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace components
{
    enum class preview_card_state
    {
        displaying,
        changing_forward,
        changing_back,
        animating
    };

    struct deck_selection : public ecs::component<deck_selection>
    {
        entity_id p1_deck_options[3]{};
        entity_id p2_deck_options[3]{};

        // for players 1 and 2
        entity_id card_entity_current[2] {0, 0};
        entity_id card_entity_next[2]{ 0,0 };
        preview_card_state preview_card_state[2] { preview_card_state::changing_forward, preview_card_state::changing_forward };
        std::pair<float, glm::vec3>* current_position_keyframes[2];
        std::set<components::card_enum>::iterator card_iterators[2];

        float card_animation_time_total{ .35f };
        float card_animation_time[2]{ 0, 0 };

        std::pair<float, glm::vec3> position_keyframes[6]{
            {std::numeric_limits<float>::min(), glm::vec3(0.f)},
            {0.0f, glm::vec3(0.f)},
            {0.5f,glm::vec3(0.f, 5.f, 5.f)},
            {0.6f,glm::vec3(0.f, 5.f, 5.f)},
            {1.f, glm::vec3(0.f, 10.f, 0.f) },
            {std::numeric_limits<float>::max(), glm::vec3(0.f, 10.f, 0.f) }
        };

        std::pair<float, glm::vec3> position_keyframes_under[6]{
            {std::numeric_limits<float>::min(), glm::vec3(0.f)},
            {0.0f, glm::vec3(0.f)},
            {0.5f,glm::vec3(0.f, 5.f, -5.f)},
            {0.6f,glm::vec3(0.f, 5.f, -5.f)},
            {1.f, glm::vec3(0.f, 10.f, 0.f) } ,
            {std::numeric_limits<float>::max(), glm::vec3(0.f, 10.f, 0.f)}
        };

        float card_y{ -6.f };
        float card_depth{ 25.f };

        size_t num_decks{ 2 };
        float option_width{ 4.f };
        float half_option_width{ option_width/2.f };
        float padding{ 0.5f };
        float total_width{ 37.f };
        float half_width{ total_width / 2.f };
        float quarter_width{ total_width / 4.f };
        float deck_container_width{ num_decks * (padding + option_width) };
        float half_deck_container_width{ deck_container_width / 2.f };
        
        float player_1_options_x{ -quarter_width - half_deck_container_width + half_option_width + 1.f };
        float player_2_options_x{ quarter_width - half_deck_container_width + half_option_width  - 1.f };


        std::uint32_t child_count = 0;
        entity_id children[32];
    };
}


#endif