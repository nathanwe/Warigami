//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_ANIMATION_HPP
#define WIZARDENGINE_SKELETON_ANIMATION_HPP

#include <vector>
#include <cstdint>
#include <rendering/skeletal_animation_frame.hpp>

namespace rendering
{
    struct skeletal_animation
    {
        static const std::uint32_t MaxAnimationFrames = 256;

        skeletal_animation_frame frames[MaxAnimationFrames];
        std::uint32_t frame_count {0};
    };
}

#endif //WIZARDENGINE_SKELETON_ANIMATION_HPP
