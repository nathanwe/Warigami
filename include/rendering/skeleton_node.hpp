//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_NODE_HPP
#define WIZARDENGINE_SKELETON_NODE_HPP


#include <vector>
#include <rendering/skeleton_animation.hpp>

namespace rendering
{

    struct skeleton_node
    {
        static const std::uint32_t MaxAnimations = 16;

        std::vector<skeletal_animation> animations {};
        std::uint32_t animation_count {0};
    };

}
#endif //WIZARDENGINE_SKELETON_NODE_HPP
