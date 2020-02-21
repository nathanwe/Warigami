//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_SKELETON_NODE_HPP
#define WIZARDENGINE_SKELETON_NODE_HPP

#include <cstdint>
#include <vector>
#include <rendering/skeleton_animation.hpp>
#include <glm/mat4x4.hpp>

namespace rendering
{

    struct skeleton_node
    {
        static const std::uint32_t MaxChildren = 16;
        static const std::uint32_t MaxAnimations = 8;


        glm::mat4 base_transform;
        glm::mat4 calculated_transform;
        skeletal_animation animations[MaxAnimations];        
        skeleton_node* children[MaxChildren];
        std::uint16_t bone_id;
        std::uint8_t child_count{ 0 };        

        inline void add_child(skeleton_node* node);
        void compute_transform(const glm::mat4& frame, animation_time t, std::uint16_t animation_index);
    };

}
#endif //WIZARDENGINE_SKELETON_NODE_HPP
