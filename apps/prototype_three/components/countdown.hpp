//
// Created by sava on 2020-03-03.
//

#ifndef WIZARDENGINE_COUNTDOWN_HPP
#define WIZARDENGINE_COUNTDOWN_HPP

#include <cstdint>
#include <ecs/component.hpp>

class countdown : public ecs::component<countdown>
{
    std::uint8_t current_value {3};
    float count_duration {1.f};
};


#endif //WIZARDENGINE_COUNTDOWN_HPP
