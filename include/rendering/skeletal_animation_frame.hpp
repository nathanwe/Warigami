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
    struct skeletal_animation_frame
    {
        glm::quat rotation;
        glm::vec3 position {0.f};
        animation_time t {0};
        float scale {1.f};
    };

    template <typename T, std::uint32_t max_frames>
    struct keyframes
    {
        T values[max_frames];
        animation_time times[max_frames];
    };
}

#endif //WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP
