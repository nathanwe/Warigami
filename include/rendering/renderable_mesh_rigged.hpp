//
// Created by sava on 2020-02-18.
//

#ifndef WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP
#define WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP

#include <vector>
#include <cstdint>

#include <rendering/skeleton_node.hpp>
#include <rendering/mesh_static.hpp>
#include <rendering/material_pbr.hpp>
#include <ecs/component.hpp>

namespace rendering
{

    struct renderable_mesh_rigged : ecs::component<renderable_mesh_rigged>
    {
        static const std::uint32_t MaxBones = 64;

        //skeleton_node bones[MaxBones];
        std::vector<skeleton_node> bones;
        mesh_static mesh;
        material_pbr material;
        animation_time t{ 0 };
        skeleton_node* root{ nullptr };
        std::uint32_t animation_count{ 0 }; // possibly unneeded
        std::uint32_t bone_count{ 0 };
        std::uint16_t animation_index{ 0 };
    };

}


#endif //WIZARDENGINE_RENDERABLE_MESH_RIGGED_HPP
