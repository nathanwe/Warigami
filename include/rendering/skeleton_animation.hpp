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

        keyframes<glm::quat, MaxAnimationFrames> rotation;
        keyframes<glm::vec3, MaxAnimationFrames> position;
        keyframes<glm::vec3, MaxAnimationFrames> scale;
    };
}

#endif //WIZARDENGINE_SKELETON_ANIMATION_HPP
