//
// Created by sava on 2020-02-18.
//

#ifndef WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP
#define WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP

#include <cstdint>

#include <rendering/skeleton_node.hpp>
#include <rendering/mesh_static.hpp>>
#include <rendering/material_pbr.hpp>
#include <ecs/component.hpp>

namespace rendering
{

struct renderable_mesh_rigged : ecs::component<renderable_mesh_rigged>
{
    static const std::uint32_t MaxBones = 64;

    mesh_static mesh;
    material_pbr material;
    skeleton_node bones[MaxBones];
    std::uint32_t bone_count {0};
    float t {0};
};

}


#endif //WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP
