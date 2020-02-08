//
// Created by sava on 2/7/20.
//

#ifndef WIZARDENGINE_RIGGED_VERTEX_HPP
#define WIZARDENGINE_RIGGED_VERTEX_HPP

struct vertex
{
    glm::vec3 position  = glm::vec3(0);
    glm::vec3 normal    = glm::vec3(0);
    glm::vec2 tex_coord = glm::vec2(0);
    glm::vec3 tangent   = glm::vec3(0);
    glm::vec3 bitangent = glm::vec3(0);
};

#endif //WIZARDENGINE_RIGGED_VERTEX_HPP
