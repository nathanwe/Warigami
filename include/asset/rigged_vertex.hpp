//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_RIGGED_VERTEX_HPP
#define WIZARDENGINE_RIGGED_VERTEX_HPP

#include <cstdint>
#include <limits>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


namespace asset
{
    struct rigged_vertex
    {
        glm::vec3 position = glm::vec3(0);
        glm::vec3 normal = glm::vec3(0);
        glm::vec2 tex_coord = glm::vec2(0);
        glm::vec3 tangent = glm::vec3(0);
        glm::vec3 bitangent = glm::vec3(0);

        glm::ivec4 bone_ids = glm::ivec4(0);
        glm::vec4 weights = glm::vec4(0);

        void add_weight(std::uint32_t bone_index, float weight)
        {
            if (_weight_count < 4)
            {
                bone_ids[_weight_count] = bone_index;
                weights[_weight_count++] = weight;
            } else
            {
                replace_smallest_weight(bone_index, weight);
            }
        }

    //private:
        std::uint32_t _weight_count {0};

        void replace_smallest_weight(std::uint32_t bone_index, float weight)
        {
            std::uint8_t min_ind {0};
            float min = std::numeric_limits<float>::max();

            for (std::uint8_t i = 0; i < 4; ++i)
            {
                if (weights[i] < min)
                {
                    min_ind = i;
                    min = weights[i];
                }
            }

            if (weight > min)
            {
                weights[min_ind] = weight;
                bone_ids[min_ind] = bone_index;
            }
        }
    };

}

#endif //WIZARDENGINE_RIGGED_VERTEX_HPP
