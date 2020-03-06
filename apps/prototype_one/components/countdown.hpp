//
// Created by sava on 2020-03-03.
//

#ifndef WIZARDENGINE_COUNTDOWN_HPP
#define WIZARDENGINE_COUNTDOWN_HPP

#include <cstdint>
#include <ecs/component.hpp>

namespace components
{
    struct countdown : public ecs::component<countdown>
    {
        float current_value{ 3 };
        float count_duration{ 60.f };
    };
}

#endif //WIZARDENGINE_COUNTDOWN_HPP
