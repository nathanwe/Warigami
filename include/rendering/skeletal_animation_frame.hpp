//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP
#define WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

namespace rendering
{
    struct skeletal_animation_frame
    {
        float t {0};
        float scale {1.f};
        glm::quat rotation;
        glm::vec3 position {0.f};

    };
}

#endif //WIZARDENGINE_SKELETAL_ANIMATION_FRAME_HPP
