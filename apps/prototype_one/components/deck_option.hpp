#ifndef __DECK_OPTION_HPP_
#define __DECK_OPTION_HPP_


#include <cstdint>
#include <optional>
#include <ecs/component.hpp>
#include <../deck_type.hpp>
#include "player.hpp"

namespace components
{
    struct deck_option : public ecs::component<deck_option>
    {
        deck_type deck;
        bool is_selected{ false };
        float team{ 1.f };
    };
}

#endif