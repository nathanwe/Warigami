//
// Created by sava on 2020-02-18.
//

#include <iostream>

#include <rendering/renderable_mesh_rigged.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>
#include <rendering/component_bits.hpp>


template<> const component_shift ecs::component<rendering::renderable_mesh_rigged>::component_bitshift =
    (component_shift)rendering::component_bits::renderable_mesh_rigged;


rendering::skeleton_node* rendering::renderable_mesh_rigged::allocate_node()
{
    if (bone_count == MaxBones)
    {
        std::cerr << "Could not allocate bone - out of space." << std::endl;
        return nullptr;
    }

    auto* ptr = bones + bone_count;
    bone_count++;
    return ptr;
}