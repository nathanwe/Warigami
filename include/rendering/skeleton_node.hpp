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
        static const std::uint32_t MaxChildren = 32;
        static const std::uint32_t MaxAnimations = 16;

        glm::mat4 base_transform;
        glm::mat4 offset;

        skeletal_animation animations[MaxAnimations];        
        skeleton_node* children[MaxChildren];
        std::uint16_t bone_id;
        std::uint8_t child_count{ 0 };        

        void add_child(skeleton_node* node);

        /**
         * Fills a buffer of bone matrices
         * @param parent_transform: the parent space, which is calculated at each frame of the recursion stack
         * @param t
         * @param animation_index
         * @param buffer
         */
        void compute_transform(
                glm::mat4& base_inverse,
                const glm::mat4& parent_transform,
                animation_time t,
                std::uint16_t animation_index,
                glm::mat4* buffer);
    };

}
#endif //WIZARDENGINE_SKELETON_NODE_HPP
