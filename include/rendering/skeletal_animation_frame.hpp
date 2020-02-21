//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP
#define WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP

#include <rendering/animation_time.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

namespace rendering
{ 
    template <typename T, std::uint32_t max_frames>
    struct keyframes
    {
        T values[max_frames];
        animation_time times[max_frames];
        std::uint16_t frame_count{ 0 };
    };
}

#endif //WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP
