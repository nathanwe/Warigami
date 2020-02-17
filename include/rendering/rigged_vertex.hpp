//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_RIGGED_VERTEX_HPP
#define WIZARDENGINE_RIGGED_VERTEX_HPP

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace rendering
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
    };

}

#endif //WIZARDENGINE_RIGGED_VERTEX_HPP
