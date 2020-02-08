//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_ANIMATION_HPP
#define WIZARDENGINE_SKELETON_ANIMATION_HPP

#include <vector>
#include <rendering/skeletal_animation_frame.hpp>

namespace rendering
{
    struct skeletal_animation
    {
        std::vector<skeletal_animation_frame> frames {};
    };
}

#endif //WIZARDENGINE_SKELETON_ANIMATION_HPP
