//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_NODE_HPP
#define WIZARDENGINE_SKELETON_NODE_HPP

#include <vector>
#include <rendering/skeletal_animation>

namespace rendering
{

    struct skeleton_node
    {
        std::vector<skeletal_animation> animations {};
    };

}
#endif //WIZARDENGINE_SKELETON_NODE_HPP
