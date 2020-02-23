//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_ANIMATION_HPP
#define WIZARDENGINE_SKELETON_ANIMATION_HPP

#include <vector>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include <rendering/skeletal_animation_frame.hpp>


namespace rendering
{
    struct skeletal_animation
    {
        static const std::uint32_t MaxAnimationFrames = 300;

        animation_time duration {0};
        keyframes<glm::quat, MaxAnimationFrames> rotation;
        keyframes<glm::vec3, MaxAnimationFrames> position;
        keyframes<glm::vec3, MaxAnimationFrames> scale;

        glm::mat4 eval(animation_time t);

        bool is_blank() const { return duration == animation_time::zero(); }
    };
}

#endif //WIZARDENGINE_SKELETON_ANIMATION_HPP
